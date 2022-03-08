#pragma once

#ifndef __UTL_MESSENGER_HPP_INCLUDED__
#define __UTL_MESSENGER_HPP_INCLUDED__

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <compare>

#include "functional.hpp"
#include "vector.hpp"
#include "hashmap.hpp"
#include "concepts.hpp"
#include "typeinfo.hpp"

namespace utl {

	class messenger;
	
	/// MARK: - struct message
	struct __message {
		std::strong_ordering operator<=>(__message const&) const = default;
		
	private:
		template <typename>
		friend class message;
		friend class messenger;
		friend class std::hash<__message>;
		__message(std::size_t id, std::string_view name): id(id), _name(name) {}
		std::size_t id;
		std::string_view _name;
	};

	template <typename Message>
	constexpr auto __make_message_id = utl::hash_string(utl::nameof<Message>);
	
	template <typename Message>
	struct message: __message {
		message(): __message(__make_message_id<Message>,
							 utl::nameof<Message>)
		{}
	};

	template <typename F>
	using __msgl_arg0 = typename utl::function_traits<std::decay_t<F>>::template argument<0>;

	/// MARK: - concept listener_function
	template <typename  F>
	concept listener_function =
		utl::any_invocable<F> &&
		utl::function_traits<std::decay_t<F>>::argument_count == 1 &&
		std::is_base_of_v<__message, std::remove_cvref_t<__msgl_arg0<F>>> &&
		!(std::is_lvalue_reference_v<__msgl_arg0<F>> && !std::is_const_v<std::remove_reference_t<__msgl_arg0<F>>>);

	/// MARK: - class listener_id
	class listener_id {
	public:
		listener_id() = default;
		listener_id(listener_id&&);
		listener_id& operator=(listener_id&&);
		~listener_id();
		
		std::strong_ordering operator<=>(listener_id const&) const = default;
		listener_id(__private_tag, std::size_t mid, std::size_t lid):
			_mid(mid),
			_lid(lid)
		{
			
		}
		
	private:
		friend class messenger;
		friend class listener;
		
		listener_id(listener_id const&);
		listener_id& operator=(listener_id const&);
		
		messenger* _messenger = nullptr;
		std::size_t _mid = 0;
		std::size_t _lid = 0;
	};
	
	
	/// MARK: - class listener
	class listener {
	public:
		listener(__private_tag) {}
//		listener(listener_id id, utl::function<void(__message const&)> f):
//			id(std::move(id)),
//			f(std::move(f))
//		{}
		
//	private:
		friend class messenger;
		listener_id id;
		utl::function<void(__message const&)> f;
	};
	
	template <typename Message, utl::invocable<Message> F>
	requires std::is_base_of_v<__message, Message>
	listener make_listener(F&& f) {
		listener result(__private_tag{});
		result.id = listener_id{ __private_tag{}, __make_message_id<Message>, 0 };
		result.f = [=](__message const& msg) {
			f(static_cast<Message const&>(msg));
		};
		return result;
	}
	
	listener make_listener(listener_function auto&& f) {
		using Traits = utl::function_traits<std::decay_t<decltype(f)>>;
		using Message = std::remove_cvref_t<typename Traits::template argument<0>>;
		return make_listener<Message>(UTL_FORWARD(f));
	}
	
	/// MARK: - class messenger
	class messenger {
	public:
		messenger() = default;
		messenger(messenger const&) = delete;
		
		[[nodiscard]] listener_id register_listener(listener);
		
		[[nodiscard]] listener_id register_listener(listener_function auto&& f) {
			return register_listener(make_listener(UTL_FORWARD(f)));
		}
		
		template <typename Message, utl::invocable<Message> F>
		requires std::is_base_of_v<__message, Message>
		[[nodiscard]] listener_id register_listener(F&& f) {
			return register_listener(make_listener<Message>(UTL_FORWARD(f)));
		}
		
		template <typename It>
		[[nodiscard]] utl::small_vector<listener_id> register_listeners(It begin, It end) {
			utl::small_vector<listener_id> result;
			for (; begin != end; ++begin) {
				result.push_back(register_listener(*begin));
			}
			return result;
		}
		
		void remove_listener(listener_id&);
		
		void send_message(__message const&);
		
	private:
		
		
	private:
		utl::hashmap<std::size_t, utl::vector<listener>> _listeners;
		std::size_t _listener_id = 0;
	};
	
	class listener_id_bag {
	public:
		void insert(listener_id&& id) {
			_ids.push_back(std::move(id));
		}
	private:
		utl::vector<listener_id> _ids;
	};
	
}



#endif // __UTL_MESSENGER_HPP_INCLUDED__
