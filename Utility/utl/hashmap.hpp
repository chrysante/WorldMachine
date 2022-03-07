#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "__private/robin_hood.h"
#include "hash.hpp"

namespace utl {
	
	template <typename Key, typename T, typename Hash = robin_hood::hash<Key>,
			  typename KeyEqual = std::equal_to<Key>>
	using flat_hashmap = robin_hood::unordered_flat_map<Key, T, Hash, KeyEqual>;

	template <typename Key, typename T, typename Hash = robin_hood::hash<Key>,
			  typename KeyEqual = std::equal_to<Key>>
	using node_hashmap = robin_hood::unordered_node_map<Key, T, Hash, KeyEqual>;
	
	template <typename Key, typename T, typename Hash = robin_hood::hash<Key>,
			  typename KeyEqual = std::equal_to<Key>>
	using hashmap = robin_hood::unordered_map<Key, T, Hash, KeyEqual>;
	
}
