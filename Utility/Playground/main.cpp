#include "utl/messenger.hpp"
#include <iostream>






struct MyMessage: utl::message<MyMessage>
{
	int i = -1;
};

struct AnotherMessage: utl::message<AnotherMessage>
{
	float f = 0.4;
};

int main() {
	int i = 0, j = 0;
	auto&& [x, y] = std::tie(i, j);
	x = 1;
	std::cout << i << std::endl;
	return 0;
	std::cout << "id<MyMessage> = " << utl::__make_message_id<MyMessage> << std::endl;
	std::cout << "id<AnotherMessage> = " << utl::__make_message_id<AnotherMessage> << std::endl;
	
	utl::messenger m;
	
	utl::listener_id id[4]{};
	
	id[0] = m.register_listener([](MyMessage m){
		std::cout << "recieved i = " << m.i << std::endl;
	});
	
	id[1] = m.register_listener([](MyMessage const& m){
		std::cout << "also recieved i = " << m.i << std::endl;
	});
	
	id[2] = m.register_listener([](AnotherMessage const& m){
		std::cout << "recieved another message f = " << m.f << std::endl;
	});
	
	id[3] = m.register_listener<AnotherMessage>([](auto m){
		std::cout << "recieved yet another message f = " << m.f << std::endl;
	});
	
	

	m.send_message(MyMessage{ {}, -2 });
	m.send_message(AnotherMessage{ {}, 0.5f });
	
	m.remove_listener(id[1]);
	
	std::cout << "------------------------------------------\n";
	m.send_message(MyMessage{ {}, -2 });
	
	m.remove_listener(id[0]);
	m.remove_listener(id[2]);
	m.remove_listener(id[3]);
	m.send_message(MyMessage{ {}, -2 });
	m.send_message(AnotherMessage{ {}, 0.5f });
}
