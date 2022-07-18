#ifndef HWIOAB_H_
#define HWIOAB_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "move_cmd.h"

void hwioab_init();
void hwioab_output(int speed, enum move_t move);

#ifdef __cplusplus
}
#endif


#endif /* HWIOAB_H_ */
