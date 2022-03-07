#include "NodeSerializer.hpp"

#include <yaml-cpp/yaml.h>

namespace worldmachine {
	
	void NodeSerializer::serialize(YAML::Emitter& out) const {
		using namespace YAML;
		out << Value << "Implementation Fields" << BeginMap;
		
		for (auto& f: _serializers) {
			f(out);
		}
		
		out << EndMap;
	}
	void NodeSerializer::deserialize(YAML::Node& data) const {
		YAML::Node node = data["Implementation Fields"];
		for (auto& f: _deserializers) {
			f(node);
		}
	}
	
	template <utl::arithmetic T>
	void NodeSerializer::addMember(T* data, char const* name) {
		using namespace YAML;
		_serializers.push_back([=](YAML::Emitter& out) {
			out << Key << name << Value << *data;
		});
		
		_deserializers.push_back([=](YAML::Node& node){
			using namespace YAML;
			
			auto elem = node[name];
			if (elem) {
				*data = elem.as<T>();
			}
		});
	}
	
	template void NodeSerializer::addMember(float*, char const*);
	template void NodeSerializer::addMember(int*, char const*);
	template void NodeSerializer::addMember(bool*, char const*);
	
}
