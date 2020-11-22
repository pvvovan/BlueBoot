#ifndef HWIOAB_H_
#define HWIOAB_H_

#ifdef __cplusplus
extern "C" {
#endif

enum move_t { FORWARD, BACKWARD, LEFT, RIGHT, STOP };

void hwioab_output(int speed, enum move_t move);

#ifdef __cplusplus
}
#endif

#endif /* HWIOAB_H_ */
