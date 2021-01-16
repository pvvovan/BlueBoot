#include <iostream>
#include "blue_boot.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	cli_bot::blue_boot bot{};
	bot.run();
	return 0;
}
