# IR_Learner

![alt text](https://github.com/jiayao09/IR_Learner/blob/a742e04683f8c1fc1be3c7f63574f3ec148cf390/Media/IR_learner.png)

What Can I do:
  - Able to learn and send multiple types of IR signals, for example, NEC, SONY, or unrecognized PULSE_DISTANCE.
  - Able to store and send up to three learned IR signals in EEPROM.

Check out this video: (Still making it!)


what you need to build it:

Hardware:
  Arduino Nano X 1
  Breadboard X 1
  Button X 1
  Potentiometer X 1
  IR Receiver X 1
  IR Emitter X 1
  220-ohm resistor X 1
 
Library:
  #include <IRremote.hpp>
  #include <EEPROM.h>

How to put everything together:

  The layout for the connection is shown below or check the fzz file for a detailed setup.
  
![alt text](https://github.com/jiayao09/IR_Learner/blob/a742e04683f8c1fc1be3c7f63574f3ec148cf390/Media/fzz%20layout.png)

Further development:

  - Add more learning type.
