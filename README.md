### Readme for cockpit joystick library

This is the arduino library for the cockpit projekt. It is used by all Arduino joystick boards with minimal changes for each individual board. 

The library requires the stfrha/ArduinoJoystickLibrary8axis repository to be added as a library to the scetch. 

To build a project, clone the following repository:
git@github.com:stfrha/cockpit_arduino.git

Build the project using the arduino ide. 

## Create new project using this library

At the writing time, the above mentioned repository has two arduino projects for two different joysticks. These works as examples for how to create a new arduino joystick project. 

## To debug:

To debug the library together with a arduino joystick project the following steps needs to be done:

* Copy all src files from this repository to the arduino project (where the .ino-file is).
* In the .ino-file, change the #include directive from #include <cockpit.h> to #include "cockpit.h"
* Now building and debugging of both library files and the .ino-file can be done.
* When complete, copy the debugged source files of the library back to its clone. 
* Change the .ino-file back to #include <cockpit.h>
* Commit and push both the library and the arduino project (.ino-file)
* Build and test the other arduino projects so that they work with the updated library.

