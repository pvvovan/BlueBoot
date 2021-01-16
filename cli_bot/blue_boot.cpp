#include "blue_boot.hpp"
#include "udp_handler.hpp"

namespace cli_bot {
	static void command(move direction, int seconds) {
		udp_handler handle{};
		handle.run(direction, 15, seconds);
	}

	void blue_boot::run() {
		command(move::forward, 1);
		command(move::left, 1);
		command(move::right, 1);
	}
}
