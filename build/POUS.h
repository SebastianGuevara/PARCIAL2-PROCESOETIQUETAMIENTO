#include "beremiz.h"
#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

__DECLARE_ENUMERATED_TYPE(LOGLEVEL,
  LOGLEVEL__CRITICAL,
  LOGLEVEL__WARNING,
  LOGLEVEL__INFO,
  LOGLEVEL__DEBUG
)
// FUNCTION_BLOCK LOGGER
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,TRIG)
  __DECLARE_VAR(STRING,MSG)
  __DECLARE_VAR(LOGLEVEL,LEVEL)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,TRIG0)

} LOGGER;

void LOGGER_init__(LOGGER *data__, BOOL retain);
// Code part
void LOGGER_body__(LOGGER *data__);
// PROGRAM PROGRAM0
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_LOCATED(BOOL,START)
  __DECLARE_LOCATED(BOOL,STOP)
  __DECLARE_LOCATED(BOOL,INPUT0)
  __DECLARE_LOCATED(BOOL,OUTPUT0)
  __DECLARE_LOCATED(BOOL,OUTPUT1)
  __DECLARE_LOCATED(BOOL,STOP_LIGHT)
  __DECLARE_LOCATED(BOOL,START_LIGHT)
  __DECLARE_VAR(BOOL,IR1)
  __DECLARE_VAR(BOOL,IR2)
  __DECLARE_VAR(BOOL,IR3)
  __DECLARE_VAR(BOOL,IR4)
  __DECLARE_VAR(BOOL,IR5)
  __DECLARE_VAR(TIME,TON_TIME1)
  __DECLARE_VAR(INT,COUNT_START)
  TON TON0;
  CTU CTU0;
  R_TRIG R_TRIG1;

} PROGRAM0;

void PROGRAM0_init__(PROGRAM0 *data__, BOOL retain);
// Code part
void PROGRAM0_body__(PROGRAM0 *data__);
#endif //__POUS_H
