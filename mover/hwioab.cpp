#include "hwioab.h"
#include "control_helper.hpp"

namespace iohwab
{
	static control_helper::handle s_handler{};
}

void hwioab_output(int speed, enum move_t move)
{
	iohwab::s_handler.set_cmd(control_helper::cmd_t{move, speed});
}
