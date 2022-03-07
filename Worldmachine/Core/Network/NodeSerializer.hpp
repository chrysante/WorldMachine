#pragma once

#include <utl/concepts.hpp>
#include <utl/vector.hpp>
#include <utl/functional.hpp>

namespace YAML {
	class Emitter;
	class Node;
}

namespace worldmachine {
	
	class NodeSerializer {
	public:
		void serialize(YAML::Emitter&) const;
		void deserialize(YAML::Node&) const;
		
		template <utl::arithmetic T>
		void addMember(T* data, char const* name);
		
		template <typename T>
		requires(std::is_enum_v<T>)
		void addMember(T* data, char const* name) {
			addMember((std::underlying_type_t<T>*)data, name);
		}
		
	private:
		utl::vector<utl::function<void(YAML::Emitter&)>> _serializers;
		utl::vector<utl::function<void(YAML::Node&)>> _deserializers;
	};
	
}
