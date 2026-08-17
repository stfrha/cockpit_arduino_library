#include "device_handler.h"
#include <Arduino.h>
#include <Joystick.h>

#ifndef JoystickManager_h
#define JoystickManager_h


class JoystickManager
{
private:
  const int c_numOfDevices;
  const uint8_t c_i2cAddr[4] = {0xC, 0xD, 0xE, 0xF};
  const int8_t* c_deviceList;
  const int c_numOfSignalsPerDevice;
  const int c_numOfJoystickButtons;
  uint8_t* m_signalToButtonTable;
  uint8_t* m_joystickButtonUpdates;  // 0 = no change, 2 = change to release ed, 3 = change to pressed
  int8_t* m_rotaryEncoderJoystickButtons;

  // Members for button sequence
  bool m_sequenceRunning;
  bool m_buttonDepressed;
  uint8_t m_timeCounter;
  uint8_t m_pressCounter;
  uint8_t m_numOfPresses
  uint8_t m_buttonId;
  uint8_t m_timeOn;
  uint8_t m_timeOff;

  Joystick_* m_joystick;
   
  DeviceHandler* m_devices[4];

  void serialHex(uint8_t v);

  void handleButtonSequence(void);



public:
  JoystickManager(
    const int numOfDevices, 
    const uint8_t* deviceList, 
    const int numOfSignalsPerDevice,
    const int numOfJoystickButtons,
    uint8_t* signalToButtonTable,
    uint8_t* joystickButtonUpdates,
    int8_t* rotaryEncoderJoystickButtons);

  void initiateAllDevices(void);
  void processDevices(void);
  void sendJoystickButtons(void);
  bool executeButtonSequence(
    uint8_t buttonId, 			// Joystick button to send
    uint8_t numOfPresses,               // Number of button presses produced by the sequence
    uint8_t timeOn,                     // Number of cycles (60 Hz) for depressed button
    uint8_t timeOff);                   // Number of cycles (60 Hz) for unpressed button



  void initiateTestMode(void);
  void executeTestMode(uint8_t* buf);
};

#endif
