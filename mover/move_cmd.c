#include "move_cmd.h"

#define CMD_DELIM (4)
#define CMD_MOVE  ((cmd_t)((1 << CMD_DELIM) - 1))
#define CMD_SPEED ((cmd_t)(~CMD_MOVE))


int get_speed(cmd_t cmd)
{
	return (cmd & CMD_SPEED) >> CMD_DELIM;
}

enum move_t get_move(cmd_t cmd)
{
	int id = (cmd & CMD_MOVE);
	enum move_t move = STOP;
	switch (id) {
		case 0:
			move = STOP;
			break;
		case 1:
			move = FORWARD;
			break;
		case 2:
			move = BACKWARD;
			break;
		case 3:
			move = LEFT;
			break;
		case 4:
			move = RIGHT;
			break;
		case 5:
			move = FAST_RIGHT;
			break;
		case 6:
			move = FAST_LEFT;
			break;
		default:
			move = STOP;
			break;
	}
	return move;
}
