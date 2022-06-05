#pragma once
#ifndef TIMEDTASKS_HPP
#define TIMEDTASKS_HPP
#include <defines.hpp>
#include <timeObj.h>
#include <idlers.h>

class TimedTasks : public timeObj, public idler
{
public:
  TimedTasks(void);
  virtual ~TimedTasks(void);

  void updateCurrentData(void);
#if ENABLE_I2C_SCANNER
  void ScanI2CBus(void);
#endif // ENABLE_I2C_SCANNER
  void SetupTimers();
  void accumulateSensorData(void);
  void NTPService(void);
  void checkNetwork(void);
  void checkTurnOffSettings();
  void Run_Check_DataJSON_5();
  void Run_mDNS_Background_every_10_Seconds();

  /* Callback Functionality */
  void setCallback(void (*funct)(void));
  void setSeconds(float seconds);
  virtual void idle(void);

private:
  // Timers
  void (*callback)(void);
  timeObj _Timer_1s;
  timeObj _Timer_5s;
  timeObj _Timer_5s_2;
  timeObj _Timer_10s;
  timeObj _Timer_10s_2;
  timeObj _Timer_30s;
  timeObj _Timer_1m;
  timeObj _Timer_5m;
};

extern TimedTasks timedTasks;

#endif