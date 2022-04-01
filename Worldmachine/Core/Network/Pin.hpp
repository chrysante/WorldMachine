#pragma once

#include <mtl/mtl.hpp>
#include <utl/vector.hpp>
#include <string>
#include <utl/hash.hpp>
#include <ostream>

#include "Core/Base.hpp"
#include "Core/DataType.hpp"

namespace worldmachine {
	
	enum struct PinKind {
		none = -1,
		input,
		output,
		parameterInput,
		maskInput
	};
	
	inline std::ostream& operator<<(std::ostream& str, PinKind k) {
		return str << std::array{
			"none",
			"input",
			"output",
			"parameterInput",
			"maskInput"
		}[static_cast<std::size_t>(utl::to_underlying(k) + 1)];
	}
	
	constexpr inline bool isCompatibleForImageNode(PinKind a, PinKind b) {
		auto const i = utl::to_underlying(a);
		auto const j = utl::to_underlying(b);
		return mtl::bool4x4{
			0, 1, 0, 0,
			1, 0, 0, 1,
			0, 0, 0, 0,
			0, 1, 0, 0
		}(static_cast<std::size_t>(i), static_cast<std::size_t>(j));
	}
	
	// some quick tests
	static_assert(isCompatibleForImageNode(PinKind::output, PinKind::input));
	static_assert(isCompatibleForImageNode(PinKind::input, PinKind::output));
	static_assert(isCompatibleForImageNode(PinKind::output, PinKind::maskInput));
	static_assert(!isCompatibleForImageNode(PinKind::output, PinKind::output));
	static_assert(!isCompatibleForImageNode(PinKind::output, PinKind::parameterInput));
	
	class NodeImplementation;
	
	inline constexpr auto mandatory = true;
	inline constexpr auto optional = false;
	
	class PinDescriptor {
	public:
		PinDescriptor(std::string name, DataType dataType, bool mandatory = false):
			_name(name), _dataType(dataType), _mandatory(mandatory)
		{}
		
		std::string_view name() const { return _name; }
		DataType dataType() const { return _dataType; }
		bool mandatory() const { return _mandatory; };
		
		bool operator==(PinDescriptor const&) const = default;
		
		
	private:
		std::string _name;
		DataType _dataType;
		bool _mandatory = false;
	};
	
	struct NodePinDescriptorArray {
		utl::small_vector<PinDescriptor, 4> input;
		utl::small_vector<PinDescriptor, 4> output;
		utl::small_vector<PinDescriptor, 8> parameterInput;
		utl::small_vector<PinDescriptor, 2> maskInput;
		
		utl::vector<PinDescriptor> const& get(PinKind ifKind) const {
			switch (ifKind) {
				case PinKind::input:
					return input;
				case PinKind::output:
					return output;
				case PinKind::parameterInput:
					return parameterInput;
				case PinKind::maskInput:
					return maskInput;
				default: throw;
			}
		}
		
		utl::vector<PinDescriptor>& get(PinKind ifKind) {
			return utl::as_mutable(utl::as_const(*this).get(ifKind));
		}
	};
	
}

template <>
class std::hash<worldmachine::PinDescriptor> {
public:
	std::size_t operator()(worldmachine::PinDescriptor const& desc) const {
		std::size_t seed = 0x5f23ef3b;
		seed = utl::hash_combine(seed, desc.name());
		seed = utl::hash_combine(seed, desc.mandatory());
		return seed;
	}
};
