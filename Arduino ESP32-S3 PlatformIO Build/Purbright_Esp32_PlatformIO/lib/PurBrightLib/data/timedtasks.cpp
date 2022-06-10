#include "timedtasks.hpp"

namespace AirFilter
{
  TimedTasks::TimedTasks() {}

  TimedTasks::~TimedTasks(void) {}

  void TimedTasks::SetupTimers()
  {
    _Timer_1s.setTime(1000);
    _Timer_5s.setTime(5000);
    _Timer_5s_2.setTime(5000);
    _Timer_10s.setTime(10000);
    _Timer_10s_2.setTime(10000);
    _Timer_30s.setTime(30000);
    _Timer_1m.setTime(60000);
    _Timer_5m.setTime(300000);
  }

#if ENABLE_I2C_SCANNER
  void TimedTasks::ScanI2CBus(void)
  {
    if (ENABLE_I2C_SCANNER)
    {
      if (_Timer_5s.ding())
      {
        Scan.SetupScan();
        Scan.BeginScan();
        _Timer_5s_2.start();
      }
    }
    else
    {
      return;
    }
  }
#endif // ENABLE_I2C_SCANNER

  void TimedTasks::accumulateSensorData(void)
  {
    if (_Timer_1s.ding())
    {
      accumulatedata.InitAccumulateData();
      _Timer_1s.start();
    }
  }

  void TimedTasks::NTPService(void)
  {
    if (_Timer_1s.ding())
    {
      networkntp.NTPLoop();
      _Timer_1s.start();
    }
  }

  void TimedTasks::checkNetwork(void)
  {
    if (_Timer_10s.ding())
    {
      network.CheckNetworkLoop();
      _Timer_10s.start();
    }
  }

  void TimedTasks::updateCurrentData(void) // check to see if the data has changed
  {
    if (_Timer_10s_2.ding())
    {
      cfg.updateCurrentData();
      log_i("Heap: %d", ESP.getFreeHeap());
      _Timer_10s_2.start();
    }
  }

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

  void TimedTasks::Run_mDNS_Background_every_10_Seconds()
  {
    if (_Timer_10s_2.ding())
    {
      // network.SetupmDNSLoop(); // Setup mDNS loop
      _Timer_10s_2.start();
    }
  }

  exeClass::exeClass(void) { inList = false; }

  // Before we die, we need to tell our master to let us go.
  exeClass::~exeClass(void) { theList.unlinkObj(this); }

  void exeClass::addSelf(void)
  {
    if (!inList)
    {
      theList.addToTop(this);
      inList = true;
    }
  }

  // Our call that goes into loop() to run the idlers.
  exeMgr::exeMgr(void) : linkList() {}

  exeMgr::~exeMgr(void) {}

  // Run down the list and call the idle() method on each one.
  void exeMgr::execute(void)
  {

    exeClass *trace;

    trace = (exeClass *)getFirst();
    while (trace != NULL)
    {
      trace->execute();
      trace = (exeClass *)trace->getNext();
    }
  }

  TimedTasks timedTasks;
  exeMgr theList;
}