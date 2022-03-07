#include "Core/Plugin.hpp"

#include <imgui/imgui.h>
#include <random>
#include <utl/mdvector_adaptor.hpp>

using namespace mtl;

namespace worldmachine {
	
	struct ErosionParameters {
		int iterations = 200;
		
		int seed = std::rand();
//		[Range (2, 8)]
		int erosionRadius = 3;
//		[Range (0, 1)]
		float inertia = .05f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction.
		float sedimentCapacityFactor = 4; // Multiplier for how much sediment a droplet can carry
		float minSedimentCapacity = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
//		[Range (0, 1)]
		float erodeSpeed = .3f;
//		[Range (0, 1)]
		float depositSpeed = .3f;
//		[Range (0, 1)]
		float evaporateSpeed = .01f;
		float gravity = 4;
		int maxDropletLifetime = 30;

		float initialWaterVolume = 1;
		float initialSpeed = 1;
		
		utl::vector<utl::vector<int>> erosionBrushIndices;
		utl::vector<utl::vector<float>> erosionBrushWeights;
		
		utl::mdvector_adaptor<float, 2> brushWeights;
		
		std::mt19937_64 rng{ std::random_device{}() };

		int currentSeed;
		int currentErosionRadius;
		int currentMapSize;
		
		void init(usize2 mapSize);
		void initializeBrushIndices(usize2 mapSize, int radius);
	};
	
	class ErosionNode: public ImageNodeImplementationT<ErosionNode, "Erosion"> {
	public:
		
		
		bool displayControls() override;
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override;
		static NodeDescriptor staticDescriptor();
		
		
	private:
		
		
		ErosionParameters params;
	};
	
	WM_RegisterNode(ErosionNode);
	
	bool ErosionNode::displayControls() {
		bool result = false;
		result |= ImGui::DragInt("Iterations", &params.iterations, 1000, 0, 10'000'000);
		result |= ImGui::DragInt("Radius", &params.erosionRadius, 1, 1, 30);
		result |= ImGui::DragFloat("Sediment Capacity", &params.sedimentCapacityFactor, 0.01, 1, 10);
		result |= ImGui::DragFloat("Initial Water Volume", &params.initialWaterVolume, 0.01, 0.1, 10);
		
		
		return result;
	}
	
	NodeDescriptor ErosionNode::staticDescriptor() {
		return {
			.name = "Erosion",
			.category = NodeCategory::natural,
			.pinDescriptorArray = {
				.input = {
					{ "Input", DataType::float1, mandatory },
					{ "Hardness", DataType::float1 },
					{ "Rain", DataType::float1 }
				},
				.output = {
					{ "Default", DataType::float1 },
					{ "Flowmap", DataType::float1 }
				},
				.parameterInput = {
					{ "Scale", DataType::none },
					{ "Seed", DataType::none },
					{ "", DataType::none },{ "", DataType::none },{ "", DataType::none },{ "", DataType::none },{ "", DataType::none }
				},
				.maskInput = {
					{ "Mask", DataType::float1 }
				}
			}
		};
	}
	
	using AtomicFloat = float;//std::atomic<float>;
	
	static void simulateDrops(ImageView<AtomicFloat> dest, ErosionParameters const& p, float2 dropPosition, int numDrops);
	static std::pair<float, float2> calculateHeightAndGradient(ImageView<AtomicFloat const> nodes, float posX, float posY);
	
	
	
	BuildJob ErosionNode::makeBuildJob(NodeDependencyMap dependencies) {
		ErosionParameters p = this->params;
		
		ImageView<float> dest = getBuildDest(0);
		ImageView<float const> input = dependencies.getInput<float>(0);
		WM_Assert(dest.size() == input.size());
		
		if (currentBuildType() == BuildType::preview) {
			usize2 const prevRes = buildResolution(BuildType::preview);
			usize2 const highRes = buildResolution(BuildType::highResolution);
			
			float2 const ratio2 = (float2)highRes / prevRes;
			float const ratio = ratio2.x * ratio2.y;
			p.iterations /= ratio;
			p.erosionRadius /= ratio2.x;
			if (p.erosionRadius < 1) p.erosionRadius = 1;
		}
		
		
		BuildJob job;
		
		auto* atomicDest = new utl::vector<AtomicFloat>(dest.size().x * dest.size().y);
		job.onCleanup([dest, atomicDest]{
			std::copy(atomicDest->begin(), atomicDest->end(), dest.begin());
			delete atomicDest;
		});
		
		std::memcpy(atomicDest->data(), input.data(), dest.size().fold(utl::multiplies) * sizeof(float));
		ImageView<AtomicFloat> adv(atomicDest->data(), dest.size());
		
		p.init(dest.size());
		p.iterations = utl::round_up(p.iterations, 200);
		WM_Log("Starting Erosion with {} iterations", p.iterations);
//		for (std::size_t i = 0; i < p.iterations; i += 200) {
			job.add([=]{
				simulateDrops(adv, p, {}, p.iterations);
			});
//		}
		
		
		
		return job;
	}
	
	static std::atomic<float>& operator+=(std::atomic<float>& atomicFloat, float increment)
	{
		float oldValue;
		float newValue;
		
		do
		{
			oldValue = atomicFloat.load (std::memory_order_relaxed);
			newValue = oldValue + increment;
		} while (! atomicFloat.compare_exchange_weak (oldValue, newValue,
													  std::memory_order_release,
													  std::memory_order_relaxed));
		return atomicFloat;
	}
	
	static std::atomic<float>& operator-=(std::atomic<float>& atomicFloat, float increment)
	{
		float oldValue;
		float newValue;
		
		do
		{
			oldValue = atomicFloat.load (std::memory_order_relaxed);
			newValue = oldValue - increment;
		} while (! atomicFloat.compare_exchange_weak (oldValue, newValue,
													  std::memory_order_release,
													  std::memory_order_relaxed));
		return atomicFloat;
	}
	
	static bool isInfOrNaN(float x) {
		return std::isinf(x) || std::isnan(x);
	}
	
	static void simulateDrops(ImageView<AtomicFloat> dest, ErosionParameters const& p, float2 dropPosition, int numDrops) {
		
		std::mt19937_64 rng(std::random_device{}());
		std::uniform_int_distribution<> dist(0, dest.size().x - 1);
		
		for (int iteration = 0; iteration < numDrops; iteration++) {
		
			float posX = dist(rng);
			float posY = dist(rng);
			float dirX = 0;
			float dirY = 0;
			float speed = p.initialSpeed;
			float water = p.initialWaterVolume;
			float sediment = 0;
			
			
			for (int lifetime = 0; lifetime < p.maxDropletLifetime; lifetime++) {
				int const nodeX = (int) posX;
				int const nodeY = (int) posY;
				int2 const dropIndex = { nodeX, nodeY };
				int const dropletIndex = nodeY * dest.size().x + nodeX;
				// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
				float const cellOffsetX = posX - nodeX;
				float const cellOffsetY = posY - nodeY;

				// Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
				auto const [height, gradient] = calculateHeightAndGradient(dest, posX, posY);

				// Update the droplet's direction and position (move position 1 unit regardless of speed)
				dirX = (dirX * p.inertia - gradient.x * (1 - p.inertia));
				dirY = (dirY * p.inertia - gradient.y * (1 - p.inertia));
				// Normalize direction
				float const len = std::sqrt(dirX * dirX + dirY * dirY);
				if (len != 0) {
					dirX /= len;
					dirY /= len;
				}
				WM_Assert(!isInfOrNaN(dirX));
				WM_Assert(!isInfOrNaN(dirY));
				posX += dirX;
				posY += dirY;

				// Stop simulating droplet if it's not moving or has flowed over edge of map
				if ((dirX == 0 && dirY == 0) || posX < 0 || posX >= dest.size().x - 1 || posY < 0 || posY >= dest.size().x - 1) {
					break;
				}

				// Find the droplet's new height and calculate the deltaHeight
				float newHeight = calculateHeightAndGradient(dest, posX, posY).first;
				float deltaHeight = newHeight - height;

				// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
				float sedimentCapacity = std::max(-deltaHeight * speed * water * p.sedimentCapacityFactor, p.minSedimentCapacity);
				WM_Assert(!isInfOrNaN(sedimentCapacity));
				// If carrying more sediment than capacity, or if flowing uphill:
				if (sediment > sedimentCapacity || deltaHeight > 0) {
					// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
					float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * p.depositSpeed;
					sediment -= amountToDeposit;

					// Add the sediment to the four nodes of the current cell using bilinear interpolation
					// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
					dest[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
					dest[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
					dest[dropletIndex + dest.size().x] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
					dest[dropletIndex + dest.size().x + 1] += amountToDeposit * cellOffsetX * cellOffsetY;
					
					
					WM_Assert(!isInfOrNaN(dest[dropletIndex]));
					WM_Assert(!isInfOrNaN(dest[dropletIndex + 1]));
					WM_Assert(!isInfOrNaN(dest[dropletIndex + dest.size().x]));
					WM_Assert(!isInfOrNaN(dest[dropletIndex + dest.size().x + 1]));

				} else {
					// Erode a fraction of the droplet's current carry capacity.
					// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
					float amountToErode = std::min((sedimentCapacity - sediment) * p.erodeSpeed, -deltaHeight);
					WM_Assert(!isInfOrNaN(amountToErode));
					
					// Use erosion brush to erode from all nodes inside the droplet's erosion radius
					for (int2 const index: utl::iota<int2>(-p.erosionRadius, p.erosionRadius + 1)) {
						int2 const globalIndex = dropIndex + index;
						
						if (globalIndex.x < 0 || globalIndex.x >= dest.size().x) {
							continue;
						}
						if (globalIndex.y < 0 || globalIndex.y >= dest.size().y) {
							continue;
						}
						
						float weighedErodeAmount = amountToErode * p.brushWeights[index + p.erosionRadius];
						float deltaSediment = (dest[globalIndex] < weighedErodeAmount) ? (float)dest[globalIndex] : weighedErodeAmount;
						dest[globalIndex] -= deltaSediment;
						WM_Assert(!isInfOrNaN(dest[globalIndex]));
						sediment += deltaSediment;
					}
					
					
//					for (int brushPointIndex = 0; brushPointIndex < p.erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
//						int nodeIndex = p.erosionBrushIndices[dropletIndex][brushPointIndex];
//						float weighedErodeAmount = amountToErode * p.erosionBrushWeights[dropletIndex][brushPointIndex];
//						float deltaSediment = (dest[nodeIndex] < weighedErodeAmount) ? (float)dest[nodeIndex] : weighedErodeAmount;
//						dest[nodeIndex] -= deltaSediment;
//						WM_Assert(!isInfOrNaN(dest[nodeIndex]));
//						sediment += deltaSediment;
//					}
				}

				// Update droplet's speed and water content
				speed = std::sqrt (std::max(0.0f, speed * speed + deltaHeight * p.gravity));
				WM_Assert(!isInfOrNaN(speed));
				water *= (1 - p.evaporateSpeed);
			}
			
			WM_Log("Droplet died; sediment = {}", sediment);
		}
	}
	
	
	static std::pair<float, float2> calculateHeightAndGradient(ImageView<AtomicFloat const> nodes, float posX, float posY) {
		int coordX = (int) posX;
		int coordY = (int) posY;

		// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
		float x = posX - coordX;
		float y = posY - coordY;

		// Calculate heights of the four nodes of the droplet's cell
		int nodeIndexNW = coordY * nodes.size().x + coordX;
		int2 coordNW = { coordX, coordY };
		int2 coordSE = mtl::map(coordNW + 1, (int2)nodes.size() - 1, utl::min);
		float heightNW = nodes(coordNW.x, coordNW.y);
		float heightNE = nodes(coordSE.x, coordNW.y);
		float heightSW = nodes(coordNW.x, coordSE.y);
		float heightSE = nodes(coordSE.x, coordSE.y);

		WM_Assert(!isInfOrNaN(heightNW));
		WM_Assert(!isInfOrNaN(heightNE));
		WM_Assert(!isInfOrNaN(heightSW));
		WM_Assert(!isInfOrNaN(heightSE));
		
		// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
		float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
		float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

		// Calculate height with bilinear interpolation of the heights of the nodes of the cell
		float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

		
		WM_Assert(!isInfOrNaN(height));
		WM_Assert(!isInfOrNaN(gradientX));
		WM_Assert(!isInfOrNaN(gradientY));
		return { height, float2{ gradientX, gradientY } };
	}
	
	void ErosionParameters::init(usize2 mapSize) {
		
		int radius = erosionRadius;
		
		brushWeights.resize({ radius * 2 + 1, radius * 2 + 1 });
		float sum = 0;
		for (auto index: utl::iota<int2>(-radius, radius+1)) {
			float weigth = std::max(0.0f, (radius + 1 - mtl::fast_norm((float2)index)) / radius);
			brushWeights[index + radius] = weigth;
			sum += weigth;
		}
		WM_Assert(sum > 0);
//		WM_Log("Brush Weights:");
		for (auto& i: brushWeights) {
			i /= sum;
//			WM_Log("    {}", i);
		}
		
		
		
		
//		initializeBrushIndices(mapSize, erosionRadius);
	}
	
	
	
	void ErosionParameters::initializeBrushIndices(usize2 mapSize, int radius) {
		erosionBrushIndices.resize(mapSize.x * mapSize.y);
		erosionBrushWeights.resize(mapSize.x * mapSize.y);
		
		utl::vector<int> xOffsets(radius * radius * 4);
		utl::vector<int> yOffsets(radius * radius * 4);
		utl::vector<float> weights(radius * radius * 4);
		float weightSum = 0;
		int addIndex = 0;

		
		for (int i = 0; i < erosionBrushIndices.size(); i++) {
			int centreX = i % mapSize.x;
			int centreY = i / mapSize.x;

			if (centreY <= radius || centreY >= mapSize.x - radius || centreX <= radius + 1 || centreX >= mapSize.x - radius) {
				weightSum = 0;
				addIndex = 0;
				for (int y = -radius; y <= radius; y++) {
					for (int x = -radius; x <= radius; x++) {
						float sqrDst = x * x + y * y;
						if (sqrDst < radius * radius) {
							int coordX = centreX + x;
							int coordY = centreY + y;

							if (coordX >= 0 && coordX < mapSize.x && coordY >= 0 && coordY < mapSize.x) {
								float weight = 1 - std::sqrt(sqrDst) / radius;
								weightSum += weight;
								weights[addIndex] = weight;
								xOffsets[addIndex] = x;
								yOffsets[addIndex] = y;
								addIndex++;
							}
						}
					}
				}
			}

			int numEntries = addIndex;
			
			erosionBrushIndices[i].resize(numEntries);
			erosionBrushWeights[i].resize(numEntries);

			for (int j = 0; j < numEntries; j++) {
				erosionBrushIndices[i][j] = (yOffsets[j] + centreY) * mapSize.x + xOffsets[j] + centreX;
				erosionBrushWeights[i][j] = weights[j] / weightSum;
			}
		}
	}
	
//	public class Erosion : MonoBehaviour {
//
//		public void Erode (float[] map, int mapSize, int numIterations = 1, bool resetSeed = false) {
//			init (mapSize, resetSeed);
//
//			for (int iteration = 0; iteration < numIterations; iteration++) {
//				// Create water droplet at random point on map
//				
//			}
//		}
//
//		HeightAndGradient CalculateHeightAndGradient (float[] nodes, int mapSize, float posX, float posY) {
//			int coordX = (int) posX;
//			int coordY = (int) posY;
//
//			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
//			float x = posX - coordX;
//			float y = posY - coordY;
//
//			// Calculate heights of the four nodes of the droplet's cell
//			int nodeIndexNW = coordY * mapSize + coordX;
//			float heightNW = nodes[nodeIndexNW];
//			float heightNE = nodes[nodeIndexNW + 1];
//			float heightSW = nodes[nodeIndexNW + mapSize];
//			float heightSE = nodes[nodeIndexNW + mapSize + 1];
//
//			// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
//			float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
//			float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;
//
//			// Calculate height with bilinear interpolation of the heights of the nodes of the cell
//			float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
//
//			return new HeightAndGradient () { height = height, gradientX = gradientX, gradientY = gradientY };
//		}
//
//		
//
//		struct HeightAndGradient {
//			public float height;
//			public float gradientX;
//			public float gradientY;
//		}
//	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
