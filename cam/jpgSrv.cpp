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

static std::atomic<bool> s_stop_required;
static std::atomic<bool> s_copying;
static int s_server_sockfd;
static void signal_handler([[maybe_unused]] int signum)
{
	std::cout << "Interrupted, closing socket..." << std::endl;
	s_stop_required.store(true);
	if (s_copying.load() == false) {
//		::close(s_server_sockfd);
	}
}

static const std::string input_file = "./ramdisk/output.jpg";

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

static void error(const char* msg)
{
    std::cerr << msg << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try {
		s_stop_required.store(false);
		// register signal SIGINT and signal handler  
		::signal(SIGINT, signal_handler);

		int client_sockfd;
		struct sockaddr_in serv_addr, cli_addr;

		s_server_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (s_server_sockfd < 0) {
			::error("ERROR opening socket");
			return 1;
		}
		::bzero((char *) &serv_addr, sizeof(serv_addr));
		constexpr int port = 8035;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
		serv_addr.sin_port = ::htons(port);
		if (::bind(s_server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			::error("ERROR on binding");
			return 1;
		}
		::listen(s_server_sockfd, 5);
		socklen_t clilen = sizeof(cli_addr);
		for ( ; ; ) {
			client_sockfd = ::accept(s_server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
			if (client_sockfd > 0) {
				::send_file(client_sockfd);
			}
			if (s_stop_required.load() == true) {
				break;
			}
		}
	} catch (...) {
		std::cerr << "Exception occurred!" << std::endl;
	}

	::close(s_server_sockfd);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return 0; 
}
