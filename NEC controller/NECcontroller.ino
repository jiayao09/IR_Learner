#include <IRremote.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SEND_PIN 3
#define RECV_PIN 7

int codetype;
int pushButton_pin3 = 8;
int pushButton_pin2 = 5;
int pushButton_pin1 = 4;
bool pushButton1;
bool pushButton2;
bool pushButton3;
unsigned long codeValue;
unsigned long codeValue_previous;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);


void setup() {
  pinMode(pushButton_pin1,INPUT_PULLUP);
  pinMode(pushButton_pin2,INPUT_PULLUP);
  pinMode(pushButton_pin3,INPUT_PULLUP);
  IrSender.begin(SEND_PIN, ENABLE_LED_FEEDBACK);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  Serial.begin(9600);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("NEC Controller Begin"));
  delay(5000); 
  display.display();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

}

void loop() {
    
  display.clearDisplay();
  display.setCursor(0, 0);
  
  pushButton1 = digitalRead(pushButton_pin1);
  pushButton2 = digitalRead(pushButton_pin2);
  pushButton3 = digitalRead(pushButton_pin3);
  if (!pushButton1) {
    IrSender.sendNECRaw(0xFFC23D, 0);      // PLAY/PAUSE
    display.println(" ");
    display.println("PLAY/PAUSE");
    delay(40);
  } else if (!pushButton2) {
    IrSender.sendNECRaw(0xFF22DD, 0);      // PREV
    display.println(" ");
    display.println("PREV");
    delay(40);
  }else if (!pushButton3) {
    IrSender.sendNECRaw(0xFF02FD, 0);     // NEXT
    display.println(" ");
    display.println("NEXT");
    delay(40);
  }else {
    display.println(" ");
    display.println("Waiting for action");
    delay(40);
  }
  delay(100);
  display.display();
  delay(100);
   
  codeValue = IrReceiver.decodedIRData.decodedRawData;
  Serial.println(codeValue);
  if (IrReceiver.decode() && codeValue != codeValue_previous) {

        // Print a short summary of received data
        IrReceiver.printIRResultShort(&Serial);
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print more info
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        }
        Serial.println();
  }
  
  codeValue_previous =  codeValue;
  IrReceiver.resume();
  delay(1000);

  
  
}
