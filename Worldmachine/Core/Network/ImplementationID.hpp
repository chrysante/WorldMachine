#pragma once

#include <compare>
#include <cstddef>
#include <functional>

namespace worldmachine {
	
	class ImplementationID {
	public:
		using ValueType = std::size_t;
		
		ImplementationID() = default;
		explicit ImplementationID(std::size_t value): _value(value) {}
		std::strong_ordering operator<=>(ImplementationID const&) const = default;
		std::size_t value() const { return _value; }
		
	private:
		std::size_t _value;
	};
	
	inline std::ostream& operator<<(std::ostream& str, ImplementationID id) {
		return str << id.value();
	}
	
}

template <>
class std::hash<worldmachine::ImplementationID> {
public:
	std::size_t operator()(worldmachine::ImplementationID id) const {
		return std::hash<std::size_t>{}(id.value());
	}
};
