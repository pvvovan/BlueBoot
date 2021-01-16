#include "udp_handler.hpp"
#include <iostream>
#include <thread>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <stdlib.h>


namespace cli_bot {
	static int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
	{
		int result;
		addrinfo* result_list = NULL;
		addrinfo hints = {};
		hints.ai_family = family;
		hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
		result = getaddrinfo(hostname, service, &hints, &result_list);
		if (result == 0)
		{
			//ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
			memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
			freeaddrinfo(result_list);
		}

		return result;
	}

	static void send_udp(unsigned char cmd) {
		int sock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_storage addrDest = {};
    	resolvehelper("192.168.10.156", AF_INET, "8035", &addrDest);
		sendto(sock, &cmd, 1, 0, (sockaddr*)&addrDest, sizeof(addrDest));
	}

	void udp_handler::run(move cmd, int speed, int seconds) {
		unsigned char msg = '\0';
		switch (cmd) {
			case move::forward:
				msg = 1;
				break;
			case move::left:
				msg = 3;
				break;
			case move::right:
				msg = 4;
				break;
			default:
				std::cout << "undefined move ";
				break;
		}
		for (int i = 0; i < seconds * 10; i++) {
			send_udp(static_cast<unsigned char>((speed << 4) | msg));
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}
