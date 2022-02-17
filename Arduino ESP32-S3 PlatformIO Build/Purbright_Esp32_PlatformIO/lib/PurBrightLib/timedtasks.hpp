#pragma once
#ifndef TIMEDTASKS_hpp
#define TIMEDTASKS_hpp
#include <blinker.h>
/* #include <iostream>     // std::cout
#include <functional>   // std::bind */
#include <defines.hpp> // std::thread

class TimedTasks : public timeObj,
                   public idler
{
public:
  TimedTasks(void);
  virtual ~TimedTasks(void);
  virtual void Run_NetworkCheck_Background_every_10_Seconds();
  // virtual void idle();
  virtual void Run_mDNS_Background_every_10_Seconds();
  virtual void checkTurnOffSettings();
  virtual void Run_Check_DataJSON_5();
  void setCallback(void (*funct)(void));
  void setSeconds(float seconds);
  virtual void idle(void);

  void (*callback)(void);
  void SetupTimers();
  timeObj ReadTimer;
  timeObj ReadTimer_10;
  timeObj ReadTimer2_10;
  timeObj ReadTimer3_10;
  timeObj ReadTimer2;
  timeObj ReadTimer3;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TIMEDTASKS)
extern TimedTasks timedTasks;
#endif

#endif