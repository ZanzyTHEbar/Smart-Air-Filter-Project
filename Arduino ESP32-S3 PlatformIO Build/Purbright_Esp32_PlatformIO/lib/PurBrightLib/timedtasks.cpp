
#include "timedtasks.hpp"

TimedTasks::TimedTasks(void)
{
}

TimedTasks::~TimedTasks(void)
{
}

void TimedTasks::SetupTimers() // TODO: CALL IN THE MAIN SETUP
{
  ReadTimer.setTime(5000);
  ReadTimer_10.setTime(10000);
  ReadTimer2_10.setTime(10000);
  ReadTimer3_10.setTime(10000);
  ReadTimer2.setTime(5000);
  ReadTimer3.setTime(5000);
}

/* void TimedTasks::idle()
{
} */

// Timer delay Settings
void TimedTasks::checkTurnOffSettings()
{
  // check if delay has timed out
  if (DelayRunningSettings && ((millis() - DelayStartSettings) >= 4000))
  {
    DelayRunningSettings = false;
    S_Menu = false;
  }
}

void TimedTasks::setCallback(void (*funct)(void))
{
  callback = funct;
  hookup();
}

void TimedTasks::setSeconds(float seconds) { setTime(seconds * 1000); }

void TimedTasks::idle(void)
{

  if (ding() && callback)
  {
    stepTime();
    callback();
  }
}

void TimedTasks::Run_NetworkCheck_Background_every_10_Seconds()
{
  if (ReadTimer_10.ding())
  {
    network.CheckNetworkLoop(); // Check the network state and connect to the AP if needed
    ReadTimer_10.start();
  }
}

void TimedTasks::Run_mDNS_Background_every_10_Seconds()
{
  if (ReadTimer2_10.ding())
  {
    network.SetupmDNSLoop(); // Setup mDNS loop
    ReadTimer2_10.start();
  }
}

void TimedTasks::Run_Check_DataJSON_5()
{
  if (ReadTimer2.ding())
  {
    accumulateData.InitAccumulateDataJson();
    ReadTimer2.start();
  }
}

TimedTasks timedTasks;