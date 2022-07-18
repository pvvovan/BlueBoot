#include "hwioab.h"
#include "control_helper.hpp"


namespace iohwab
{
	static control_helper::handle s_handler{};
}

void hwioab_init() { }

void hwioab_output(int speed, enum move_t move)
{
	control_helper::cmd_t cmd;
	cmd.move = move;
	cmd.speed = speed;
	iohwab::s_handler.set_cmd(cmd);
}
