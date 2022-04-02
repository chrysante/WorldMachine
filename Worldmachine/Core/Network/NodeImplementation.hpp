#pragma once

#include <utl/UUID.hpp>
#include <string>
#include <utl/static_string.hpp>
#include <utl/hash.hpp>
#include <atomic>

#include "Core/Base.hpp"
#include "Core/BuildSystemFwd.hpp"
#include "Core/Image/Image.hpp"

#include "Pin.hpp"
#include "NodeSerializer.hpp"
#include "ImplementationID.hpp"

namespace worldmachine {
	
	class NodeSerializer;
	class NodeDependencyMap;
	
	enum class NodeType {
		none = 0,
		image = 1,
		parameter = 2
	};
	
	/// MARK: - NodeImplementation
	class NodeImplementation {
		friend class NodeCollection; // for NodeCollection::addNode() to access dynamicInit() and setNodeID()
		friend class BuildSystem;
		friend class ImageNodeImplementation;
		friend class NodeDependencyMap;
		friend class PluginManager;
		friend class Network;
		friend class Registry;
		
	public:
		NodeImplementation(NodeType type): _type(type) {}
		virtual ~NodeImplementation() = default;

		/// Interface to override
		virtual bool displayControls() = 0;
		virtual BuildJob makeBuildJob(NodeDependencyMap dependencies) = 0;
		
		std::string_view implementationName() const noexcept { return _implName(); }
		ImplementationID implementationID() const noexcept { return _implID(); };
		
		utl::UUID nodeID() const { return _nodeID; }
		NodeSerializer& serializer() { return _serializer; }
		NodeSerializer const& serializer() const { return _serializer; }
		
		NodeType type() const { return _type; }
		
		bool isBuilding() const { return _isBuilding; }
		BuildType currentBuildType() const { return _currentBuildType; }
		
		bool built() const { return _built; }
		bool previewBuilt() const { return _previewBuilt; }
		
	protected:
		mtl::usize2 buildResolution(BuildType type) const;
		mtl::usize2 currentBuildResolution() const { return buildResolution(currentBuildType()); }
		
	private:
		virtual std::string_view _implName() const noexcept = 0;
		virtual ImplementationID _implID() const noexcept = 0;
		virtual void dynamicInit() = 0;
		
	private:
		utl::UUID _nodeID;
		NodeSerializer _serializer;
		mtl::usize2 _previewBuildResolution = 0;
		mtl::usize2 _highresBuildResolution = 0;
		NodeType _type;
		std::atomic<BuildType> _currentBuildType = BuildType::none;
		std::atomic_bool _isBuilding = false;
		std::atomic_bool _built = false;
		std::atomic_bool _previewBuilt = false;
	};

	/// MARK: FallbackNodeImplementation
	class FallbackNodeImplementation: public NodeImplementation {
	public:
		FallbackNodeImplementation(): NodeImplementation(NodeType::none) {}
		bool displayControls() override { return false; }
		BuildJob makeBuildJob(NodeDependencyMap) override;
		
	private:
		std::string_view _implName() const noexcept override {
			return "Fallback";
		}
		ImplementationID _implID() const noexcept override {
			return ImplementationID(235892367492305893);
		}
		void dynamicInit() override {}
	};
	
	/// MARK: - ImageNodeImplementation
	class ImageNodeImplementation: public NodeImplementation {
		friend class BuildSystem;
	public:
		ImageNodeImplementation(): NodeImplementation(NodeType::image) {}
		
		Image const& previewImage(std::size_t index) const {
			return _previewOutputs[index];
		}
		Image const& highResImage(std::size_t index) const {
			return _highresOutputs[index];
		}
		
		Image const& getImage(std::size_t index, BuildType type) const;
		
	protected:
		Image& getBuildDest(std::size_t index);
		Image const& getBuildDest(std::size_t index) const;
		
		void clearBuildDest();
		
	private:
		void dynamicInit() override;
		
	private:
		utl::small_vector<Image, 2> _previewOutputs;
		utl::small_vector<Image, 2> _highresOutputs;
	};
	
	/// MARK: - NodeImplementationT
	template <typename NodeType, typename Base, utl::basic_static_string Name>
	class NodeImplementationT: public Base {
	public:
		static ImplementationID staticID() noexcept  {
			constexpr std::size_t ID = utl::hash_string(Name);
			return ImplementationID(ID);
		}
		static std::string_view staticName() noexcept  { return Name; }
	private:
		ImplementationID _implID() const noexcept final { return staticID(); }
		std::string_view _implName() const noexcept final { return staticName(); }
	};

	template <typename NodeType, utl::basic_static_string Name>
	using ImageNodeImplementationT = NodeImplementationT<NodeType, ImageNodeImplementation, Name>;
	
}
