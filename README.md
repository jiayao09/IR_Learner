# IR_Learner

![alt text](https://github.com/jiayao09/IR_Learner/blob/a742e04683f8c1fc1be3c7f63574f3ec148cf390/Media/IR_learner.png)

<br/> What Can I Do:
  - Able to learn and send multiple types of IR signals, for example, NEC, SONY, or unrecognized PULSE_DISTANCE.
  - Able to store and send up to three learned IR signals in EEPROM.
  
  <br/> Check out this video: (Still making it!)
  
<br/> What You Need To Build It:

  - Hardware:

        - Arduino Nano X 1
        - Breadboard X 1
        - Button X 1  
        - Potentiometer X 1  
        - IR Receiver X 1
        - IR Emitter X 1
        - 220-ohm resistor X 1 

  - Library:

        #include <IRremote.hpp>
  
        #include <EEPROM.h>


<br/> How to Put Everything Together:

  - The layout for the connection is shown below or check the fzz file for a detailed setup.
  
![alt text](https://github.com/jiayao09/IR_Learner/blob/a742e04683f8c1fc1be3c7f63574f3ec148cf390/Media/fzz%20layout.png)

<br/> Further Development:

  - Add more learning type.
