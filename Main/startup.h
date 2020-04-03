#ifndef __STARTUP_H__
#define __STARTUP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx.h"

void initialize();
void setup();
void loop();

#ifdef __cplusplus
}
#endif

#endif /* __STARTUP_H__ */
