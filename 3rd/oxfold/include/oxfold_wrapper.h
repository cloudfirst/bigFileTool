#ifndef OXFOLD_WRAPPER_H
#define OXFOLD_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OXFOLD_NWID 0xa6f4adb92de77959

int start_oxfold();
int stop_oxfold();
char* getNodeIPV4();
char* getNodeIPV6();
uint64_t getNodeNWID();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // OXFOLD_WRAPPER_H




