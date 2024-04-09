void LOGGER_init__(LOGGER *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->TRIG,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MSG,__STRING_LITERAL(0,""),retain)
  __INIT_VAR(data__->LEVEL,LOGLEVEL__INFO,retain)
  __INIT_VAR(data__->TRIG0,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void LOGGER_body__(LOGGER *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  if ((__GET_VAR(data__->TRIG,) && !(__GET_VAR(data__->TRIG0,)))) {
    #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
    #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  
    #undef GetFbVar
    #undef SetFbVar
;
  };
  __SET_VAR(data__->,TRIG0,,__GET_VAR(data__->TRIG,));

  goto __end;

__end:
  return;
} // LOGGER_body__() 





void PROGRAM0_init__(PROGRAM0 *data__, BOOL retain) {
  __INIT_LOCATED(BOOL,__IX0_0,data__->START,retain)
  __INIT_LOCATED_VALUE(data__->START,__BOOL_LITERAL(FALSE))
  __INIT_LOCATED(BOOL,__IX0_1,data__->STOP,retain)
  __INIT_LOCATED_VALUE(data__->STOP,__BOOL_LITERAL(FALSE))
  __INIT_LOCATED(BOOL,__IX0_2,data__->INPUT0,retain)
  __INIT_LOCATED_VALUE(data__->INPUT0,__BOOL_LITERAL(TRUE))
  __INIT_LOCATED(BOOL,__QX0_0,data__->OUTPUT0,retain)
  __INIT_LOCATED_VALUE(data__->OUTPUT0,__BOOL_LITERAL(FALSE))
  __INIT_LOCATED(BOOL,__QX0_1,data__->OUTPUT1,retain)
  __INIT_LOCATED_VALUE(data__->OUTPUT1,__BOOL_LITERAL(FALSE))
  __INIT_LOCATED(BOOL,__QX0_2,data__->STOP_LIGHT,retain)
  __INIT_LOCATED_VALUE(data__->STOP_LIGHT,__BOOL_LITERAL(FALSE))
  __INIT_LOCATED(BOOL,__QX0_3,data__->START_LIGHT,retain)
  __INIT_LOCATED_VALUE(data__->START_LIGHT,__BOOL_LITERAL(FALSE))
  __INIT_VAR(data__->IR1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->IR2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->IR3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->IR4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->IR5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TON_TIME1,__time_to_timespec(1, 0, 5, 0, 0, 0),retain)
  __INIT_VAR(data__->COUNT_START,5,retain)
  TON_init__(&data__->TON0,retain);
  CTU_init__(&data__->CTU0,retain);
  R_TRIG_init__(&data__->R_TRIG1,retain);
}

// Code part
void PROGRAM0_body__(PROGRAM0 *data__) {
  // Initialise TEMP variables

  if (((!(__GET_VAR(data__->IR3,)) && __GET_LOCATED(data__->START,)) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,OUTPUT0,,__BOOL_LITERAL(TRUE));
  };
  if (((!(__GET_VAR(data__->IR3,)) && __GET_LOCATED(data__->START,)) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,OUTPUT1,,__BOOL_LITERAL(FALSE));
  };
  if (((!(__GET_VAR(data__->IR3,)) && __GET_LOCATED(data__->START,)) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_VAR(data__->,IR1,,__BOOL_LITERAL(TRUE));
  };
  if (((!(__GET_LOCATED(data__->INPUT0,)) && (__GET_VAR(data__->IR1,) || __GET_VAR(data__->IR4,))) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,OUTPUT0,,__BOOL_LITERAL(FALSE));
  };
  if (((!(__GET_LOCATED(data__->INPUT0,)) && (__GET_VAR(data__->IR1,) || __GET_VAR(data__->IR4,))) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,OUTPUT1,,__BOOL_LITERAL(TRUE));
  };
  if (((!(__GET_LOCATED(data__->INPUT0,)) && (__GET_VAR(data__->IR1,) || __GET_VAR(data__->IR4,))) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_VAR(data__->,IR2,,__BOOL_LITERAL(TRUE));
  };
  __SET_VAR(data__->TON0.,IN,,(__GET_VAR(data__->IR2,) && !(__GET_LOCATED(data__->STOP,))));
  __SET_VAR(data__->TON0.,PT,,__GET_VAR(data__->TON_TIME1,));
  TON_body__(&data__->TON0);
  if (__GET_VAR(data__->TON0.Q,)) {
    __SET_LOCATED(data__->,OUTPUT0,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->TON0.Q,)) {
    __SET_LOCATED(data__->,OUTPUT1,,__BOOL_LITERAL(FALSE));
  };
  __SET_VAR(data__->,IR3,,__GET_VAR(data__->TON0.Q,));
  __SET_VAR(data__->R_TRIG1.,CLK,,(__GET_VAR(data__->IR3,) && !(__GET_LOCATED(data__->STOP,))));
  R_TRIG_body__(&data__->R_TRIG1);
  __SET_VAR(data__->CTU0.,CU,,__GET_VAR(data__->R_TRIG1.Q,));
  __SET_VAR(data__->CTU0.,R,,__GET_LOCATED(data__->START,));
  __SET_VAR(data__->CTU0.,PV,,__GET_VAR(data__->COUNT_START,));
  CTU_body__(&data__->CTU0);
  if (__GET_VAR(data__->CTU0.Q,)) {
    __SET_LOCATED(data__->,START,,__BOOL_LITERAL(FALSE));
  };
  __SET_LOCATED(data__->,START_LIGHT,,(!(__GET_LOCATED(data__->STOP,)) && __GET_VAR(data__->IR4,)));
  __SET_LOCATED(data__->,STOP_LIGHT,,(!(__GET_VAR(data__->IR5,)) && (!(__GET_VAR(data__->IR4,)) || __GET_LOCATED(data__->STOP,))));
  if (__GET_VAR(data__->CTU0.Q,)) {
    __SET_LOCATED(data__->,OUTPUT1,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->CTU0.Q,)) {
    __SET_LOCATED(data__->,OUTPUT0,,__BOOL_LITERAL(FALSE));
  };
  if (((!(__GET_VAR(data__->IR3,)) && __GET_LOCATED(data__->START,)) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,START,,__BOOL_LITERAL(TRUE));
  };
  if ((!(__GET_VAR(data__->IR5,)) && (!(__GET_VAR(data__->IR4,)) || __GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,OUTPUT0,,__BOOL_LITERAL(FALSE));
  };
  if ((!(__GET_VAR(data__->IR5,)) && (!(__GET_VAR(data__->IR4,)) || __GET_LOCATED(data__->STOP,)))) {
    __SET_LOCATED(data__->,OUTPUT1,,__BOOL_LITERAL(FALSE));
  };
  if (((!(__GET_VAR(data__->IR3,)) && __GET_LOCATED(data__->START,)) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_VAR(data__->,IR4,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->CTU0.Q,)) {
    __SET_VAR(data__->,IR4,,__BOOL_LITERAL(FALSE));
  };
  if ((!(__GET_VAR(data__->IR5,)) && (!(__GET_VAR(data__->IR4,)) || __GET_LOCATED(data__->STOP,)))) {
    __SET_VAR(data__->,IR4,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->TON0.Q,)) {
    __SET_VAR(data__->,IR2,,__BOOL_LITERAL(FALSE));
  };
  if (((!(__GET_LOCATED(data__->INPUT0,)) && (__GET_VAR(data__->IR1,) || __GET_VAR(data__->IR4,))) && !(__GET_LOCATED(data__->STOP,)))) {
    __SET_VAR(data__->,IR1,,__BOOL_LITERAL(FALSE));
  };

  goto __end;

__end:
  return;
} // PROGRAM0_body__() 





