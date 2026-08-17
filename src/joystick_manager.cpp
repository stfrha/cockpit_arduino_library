#include "joystick_manager.h"

#include "i2c_comm.h"



JoystickManager::JoystickManager(
  const int numOfDevices, 
  const uint8_t* deviceList, 
  const int numOfSignalsPerDevice,
  const int numOfJoystickButtons,
  uint8_t* signalToButtonTable,
  uint8_t* joystickButtonUpdates,
  int8_t* rotaryEncoderJoystickButtons) :
   c_numOfDevices(numOfDevices),
   c_deviceList(deviceList),
   c_numOfSignalsPerDevice(numOfSignalsPerDevice),
   c_numOfJoystickButtons(numOfJoystickButtons),
   m_signalToButtonTable(signalToButtonTable),
   m_joystickButtonUpdates(joystickButtonUpdates),
   m_rotaryEncoderJoystickButtons(rotaryEncoderJoystickButtons)
{
  m_joystick = new Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 
     122, 0,
     true,    // x-axis
     true,    // y-axis
     true,    // z-axis
     true,    // rx-axis
     true,    // ry-axis
     true,    // rz-axis
     false,   // rudder-axis
     false,   // throttle-axis
     false,   // accelerator-axis
     false,   // brake-axis
     false,   // steering-axis
     true,    // slider-axis
     true);   // dial-axis
   
    
   
  for (int deviceIndex = 0; deviceIndex < c_numOfDevices; deviceIndex++)
  {
    m_devices[deviceIndex] = new DeviceHandler(
      deviceIndex,
      c_deviceList[deviceIndex], 
      c_i2cAddr[c_deviceList[deviceIndex]],
      c_numOfSignalsPerDevice,
      &m_signalToButtonTable[deviceIndex * c_numOfSignalsPerDevice],
      m_joystickButtonUpdates,
      m_joystick,
      &m_rotaryEncoderJoystickButtons[deviceIndex * 4]);
  }

  // Initiate button sequencer	
  m_sequenceRunning = false;
  m_buttonDepressed = false;
  m_pressCounter = 0;
  m_numOfPresses = 0;

}

void JoystickManager::handleButtonSequence(void)
{
  if (m_sequenceRunning)
  {
    m_timeCounter++;

    if (m_buttonDepressed)
    {
      if (m_timeCounter > m_timeOn)
      {
        // Send button off
        m_joystick->setButton(m_buttonId, false);
        m_buttonDepressed = false;
        m_timeCounter = 0;
      }
    } 
    else
    {
      if (m_timeCounter > m_timeOff)
      {
        m_pressCounter++;

        // Check if the sequence is finished
        if (m_pressCounter >= m_numOfPresses)
        {
          m_sequenceRunning = false;
          return;
        }

        // If we get here, we are not yet finished
        // Send button on
        m_joystick->setButton(m_buttonId, true);
        m_buttonDepressed = true;
        m_timeCounter = 0;
      }
    }
  }
}

void JoystickManager::initiateAllDevices(void)
{
   
  m_joystick->setXAxisRange(0, 255);
  m_joystick->setYAxisRange(0, 1023);
  m_joystick->setZAxisRange(0, 1023);
  m_joystick->setRxAxisRange(0, 1023);
  m_joystick->setRyAxisRange(0, 1023);
  m_joystick->setRzAxisRange(0, 1023);
  // m_joystick->setRudderRange(0, 1023);
  // m_joystick->setAcceleratorRange(0, 1023);
  // m_joystick->setThrottleRange(0, 1023);
  // m_joystick->setBrakeRange(0, 1023);
  // m_joystick->setSteeringRange(0, 255);
  m_joystick->setSliderRange(0, 1023);
  m_joystick->setDialRange(0, 1023);
  
  m_joystick->begin();
 
  I2cCommunication::initializeI2c();

  Serial1.println("Initiating devices...");
  for (uint8_t device = 0; device < c_numOfDevices; device++)
  {
    m_devices[device]->initiateDevice();
  }
}

void JoystickManager::processDevices(void)
{
  for (uint8_t device = 0; device < c_numOfDevices; device++)
  {
    m_devices[device]->processDevice();
  }
  
  // Run button sequence (if non is started, nothing will be done)
  handleButtonSequence();

}

void JoystickManager::sendJoystickButtons(void)
{
  // Loop all bits of the joystickButtonUpdate vector to see if any joystick button should be changed.
  for (uint8_t i = 0; i < c_numOfJoystickButtons; i++)
  {
    if (m_joystickButtonUpdates[i] == 3)
    {
      // Serial1.print("Joystick button: ");
      // Serial1.print(i);
      // Serial1.println(" is pressed.");
      m_joystick->setButton(i, true);
    }
    else if (m_joystickButtonUpdates[i] == 2)
    {
      // Serial1.print("Joystick button: ");
      // Serial1.print(i);
      // Serial1.println(" is released.");
      m_joystick->setButton(i, false);
    }

    // Reset state
    m_joystickButtonUpdates[i] = 0;
  }

  // Serial1.print(", start time after sendJoystickButtons: ");
  // Serial1.print(bmStartTime);

}


// Starts a button press sequence. If a sequence is already running, this method will
// return false and the new sequence will be ignored (the current sequence will continue).
bool JoystickManager::executeButtonSequence(
  uint8_t buttonId, 
  uint8_t numOfPresses, 
  uint8_t timeOn, 
  uint8_t timeOff)
{
  if (m_sequenceRunning)
  {
    return false;
  }

  m_sequenceRunning = true;
  m_pressCounter = 0;
  m_numOfPresses = numOfPresses;
  m_buttonId = buttonId;
  m_timeOn = timeOn;
  m_timeOff = timeOff;
  
  // We depress the first iteration immediately
  m_joystick->setButton(m_buttonId, true);
  m_buttonDepressed = true;
  return true;
}


void JoystickManager::initiateTestMode(void)
  {
    Serial1.println("This data printout is best viewed in a terminal where carrige return works as expected.");
    Serial1.println("");
    Serial1.println("               Device 0                               |               Device 1                                |               Device 2                                |               Device 3                                |");
    Serial1.println("ADC0H- ADC0L- ADC1H- ADC1L-   5  -   6  -   7  -   8  -ADC0H -ADC0L -ADC1H -ADC1L -   5  -   6  -   7  -   8  -ADC0H -ADC0L -ADC1H -ADC1L -   5  -   6  -   7  -   8  -ADC0H -ADC0L -ADC1H -ADC1L -   5  -   6  -   7  -   8  -");
    Serial1.println("R0 | AD0    |R1 |AD1      |EX |RO  |RC  |BR  |LC  |TR |R0 | AD0     |R1 |AD1      |EX |RO  |RC  |BR  |LC  |TR |R0 | AD0     |R1 |AD1      |EX |RO  |RC  |BR  |LC  |TR |R0 | AD0     |R1 |AD1      |EX |RO  |RC  |BR  |LC  |TR |");
  }
  
  
  void JoystickManager::executeTestMode(uint8_t* buf)
  {
        for (int i = 1; i < 9; i++)
        {
          serialHex(buf[i]);
          Serial1.print(" - ");
        }
  }


void JoystickManager::serialHex(uint8_t v)
{
  Serial1.print("0x");

  if (v < 0x10)
  {
    Serial1.print('0');
  }
  Serial1.print(v, HEX);
}

