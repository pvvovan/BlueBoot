#ifndef MOVE_CMD_H_
#define MOVE_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UDP_SERVER_PORT	8035

enum move_t { FORWARD, BACKWARD, LEFT, RIGHT, STOP, FAST_RIGHT, FAST_LEFT };

typedef unsigned char cmd_t;

int get_speed(cmd_t cmd);
enum move_t get_move(cmd_t cmd);

#ifdef __cplusplus
}
#endif

#endif /* MOVE_CMD_H_ */
