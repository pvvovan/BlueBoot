#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <memory>
#include <filesystem>
#include <chrono>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <atomic>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>


static const std::string input_file = "./ramdisk/output.jpg";
static constexpr int BACKLOG{5};

static void send_file(int sock_fd)
{
	std::ifstream in_file(input_file, std::ios::binary);
	int len = static_cast<int>(std::filesystem::file_size(input_file));
	auto mem_block = std::make_unique<char[]>(static_cast<unsigned long>(len));
	in_file.read(mem_block.get(), len);
	in_file.close();

	[[maybe_unused]] auto err = ::write(sock_fd, &len, sizeof(len));
	err = ::write(sock_fd, mem_block.get(), static_cast<unsigned long>(len));
	::close(sock_fd);
}

static void error_exit(std::string msg) {
	std::cerr << msg << std::endl;
	std::terminate();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	int srv_fd{};

	try {
		struct addrinfo hints{}, *result{nullptr}, *p_res{nullptr};

		::memset(&hints, 0, sizeof(hints));
		hints.ai_canonname = nullptr;
		hints.ai_addr = nullptr;
		hints.ai_next = nullptr;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

		constexpr std::string_view port{"8035"};
		if (::getaddrinfo(nullptr, port.data(), &hints, &result) != 0) {
			::error_exit("getaddrinfo failure");
		}

		int op{1};
		for (p_res = result; p_res != nullptr; p_res = p_res->ai_next) {
			srv_fd = ::socket(p_res->ai_family, p_res->ai_socktype,
									p_res->ai_protocol);
			if (srv_fd == -1) {
				continue;
			}
			if (::setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1) {
				::error_exit("setsockopt failure");
			}
			if (::bind(srv_fd, p_res->ai_addr, p_res->ai_addrlen) == 0) {
				break; /* Successful server */
			}
			::close(srv_fd); /* bind failed, try another */
		}
		if (p_res == nullptr) {
			::error_exit("server socket failed");
		}
		if (::listen(srv_fd, BACKLOG) == -1) {
			::error_exit("listen failure");
		}
		::freeaddrinfo(result);

		for ( ; ; ) {
			sockaddr_storage cl_addr;
			unsigned int addr_len = sizeof(cl_addr);
			int cl_fd = ::accept(srv_fd, (sockaddr *)&cl_addr, &addr_len);
			if (cl_fd > 0) {
				::send_file(cl_fd);
			}
		}
	} catch (...) {
		std::cerr << "Exception occurred!" << std::endl;
	}

	::close(srv_fd);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	return 0; 
}
