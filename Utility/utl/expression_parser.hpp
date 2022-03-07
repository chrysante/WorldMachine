#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <type_traits>
#include "strcat.hpp"

namespace utl {
	
	struct expression_parser {
		std::string expression;
	};
	
	
#define UTL_PARSE_EXPRESSION(...) ((::utl::expression_parser{} ->* __VA_ARGS__).expression)
	
	namespace _private {
		struct _expression_parser {
			std::string expression;
		};
	}
	
	namespace _private {
		template <typename T>
		_expression_parser operator_impl(auto&& p, T&& rhs, char const* name) {
			if constexpr (std::is_same_v<std::decay_t<T>, bool>)
				return { utl::strcat(UTL_FORWARD(p).expression, name, rhs ? "true" : "false") };
			else
			{
				if constexpr (_printable<T>) {
					return { utl::strcat(UTL_FORWARD(p).expression, name, rhs) };
				}
				else {
					return { utl::strcat(UTL_FORWARD(p).expression, name, "???") };
				}
			}
		}
	}
	
	// init
	template <typename T>
	_private::_expression_parser operator->*(expression_parser const& p, T&& rhs) {
		return _private::operator_impl(p, UTL_FORWARD(rhs), "");
	}
	template <typename T>
	_private::_expression_parser operator->*(expression_parser&& p, T&& rhs) {
		return _private::operator_impl(std::move(p), UTL_FORWARD(rhs), "");
	}
	
#define UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(op) \
	template <typename T> \
_private::_expression_parser operator op(_private::_expression_parser const& p, T&& rhs) { \
		return _private::operator_impl(p, UTL_FORWARD(rhs), " "#op" "); \
	} \
	template <typename T> \
_private::_expression_parser operator op(_private::_expression_parser&& p, T&& rhs) { \
		return _private::operator_impl(std::move(p), UTL_FORWARD(rhs), " "#op" "); \
	}
	
	namespace _private {
	
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(->*);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(+);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(-);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(*);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(/);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(%);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(&&);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(||);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(&);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(|);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(^);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(<<);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(>>);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(==);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(!=);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(<);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(<=);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(>);
		UTL_INTERNAL_EXPRESSION_PARSER_OP_IMPL(>=);
		
	}
	
}
