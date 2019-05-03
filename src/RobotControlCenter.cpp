/*
 * ExampleRobot.cpp
 *  Remember, Remember the 5th of november
 *  Created on: Nov 5, 2018
 *      Author: hephaestus
 */

#include "RobotControlCenter.h"

void RobotControlCenter::loop() {
  if (esp_timer_get_time() - lastPrint > 500 ||
      esp_timer_get_time() < lastPrint // check for the wrap over case
  ) {
    switch (state) {
    case Startup:
      setup();
      state = WaitForConnect;
      break;
    case WaitForConnect:
#if defined(USE_WIFI)
      if (manager.getState() == Connected)
#endif
        state = run; // begin the main operation loop
      break;
    default:
      break;
    }
    lastPrint = esp_timer_get_time(); // ensure 0.5 ms spacing *between* reads for Wifi to transact
  }
  if (state != Startup) {
    // If this is run before the sensor reads, the I2C will fail because the time it takes to send
    // the UDP causes a timeout
    fastLoop(); // Run PID and wifi after State machine on all states
  }
}

RobotControlCenter::RobotControlCenter(String *mn) {
  state = Startup;
  name = mn;
}

void RobotControlCenter::setup() {
  if (state != Startup)
    return;
  state = WaitForConnect;
#if defined(USE_WIFI)
  manager.setup();
#else
  Serial.begin(115200);
#endif

#if defined(USE_WIFI)
  // Attach coms
  coms.attach(new NameCheckerServer(name));
  coms.attach(new DiscoveryPacket(&coms));
#endif
}

void RobotControlCenter::fastLoop() {
  if (state == Startup) // Do not run before startp
    return;
#if defined(USE_WIFI)
  manager.loop();
  if (manager.getState() == Connected) {
    coms.server(); // @suppress("Method cannot be resolved")
  } else {
    return;
  }
#endif
}
