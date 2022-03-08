#include "messenger.hpp"

#include "debug.hpp"

namespace utl {
	
	listener_id::listener_id(listener_id const& rhs):
		_messenger(nullptr),
		_mid(rhs._mid),
		_lid(rhs._lid)
	{
		utl_assert(rhs._messenger == nullptr);
	}
	
	listener_id::listener_id(listener_id&& rhs):
		_messenger(rhs._messenger),
		_mid(rhs._mid),
		_lid(rhs._lid)
	{
		rhs._messenger = nullptr;
		rhs._mid = 0;
		rhs._lid = 0;
	}
	
	listener_id& listener_id::operator=(listener_id const& rhs) {
		utl_assert(_messenger == nullptr);
		utl_assert(rhs._messenger == nullptr);
		_mid = rhs._mid;
		_lid = rhs._lid;
		return *this;
	}
	
	listener_id& listener_id::operator=(listener_id&& rhs) {
		_messenger = rhs._messenger;
		_mid = rhs._mid;
		_lid = rhs._lid;
		rhs._messenger = nullptr;
		rhs._mid = 0;
		rhs._lid = 0;
		return *this;
	}
	
	listener_id::~listener_id() {
		if (!_messenger) {
			return;
		}
		_messenger->remove_listener(*this);
	}
	
	
	[[nodiscard]] listener_id messenger::register_listener(listener l) {
		utl_log(info, "Registering listener for id {}",
				l.id._mid);
		++_listener_id;
		l.id._lid = _listener_id;
		listener_id result = l.id;
		result._messenger = this;
		_listeners[l.id._mid].push_back(std::move(l));
		return result;
	}
	
	void messenger::remove_listener(listener_id& id) {
		[[maybe_unused]] auto const itr = _listeners.find(id._mid);
		utl_expect(itr != _listeners.end());
		
		auto& v = itr->second;
		auto const litr = std::find_if(v.begin(), v.end(), [&](listener const& l) {
			return l.id._lid == id._lid;
		});
		id._messenger = nullptr;
		utl_expect(litr != v.end());
		v.erase(litr);
	}
	
	void messenger::send_message(__message const& msg) {
		auto const itr = _listeners.find(msg.id);
		if (itr == _listeners.end() || itr->second.empty()) {
			utl_log(warning,
					"No listener for message '{}' [id = {}]\n"
					"	{}",
					msg._name, msg.id,
					itr == _listeners.end() ? "Table Entry does not exist" : "Table Entry is empty");
		}
		else {
			for (auto& [_, f]: itr->second) {
				f(msg);
			}
		}
	}
	
}
