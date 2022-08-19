
#include <IRremote.hpp>
#include <EEPROM.h>

#define RECV_PIN 5                 // Port for IR Receiver
#define SEND_PIN 3                 // Port for IR Sender
#define STATUS_PIN LED_BUILTIN     // Setup Status pin
#define DELAY_AFTER_SEND 2000

int pushButton_pin = 4;
bool pushButton;
unsigned long codeValue;
unsigned long codeValue_previous;
int codeType;
int codeLen;
int Datalength;
uint8_t Address;
uint8_t Command;

int analogInputVal; 
int output_m;



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Enabling IRin"));
  pinMode(pushButton_pin, INPUT_PULLUP);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(SEND_PIN, ENABLE_LED_FEEDBACK); 
  Serial.println(F("Enabled IRin"));
}

void loop() {
  unsigned int *irSignal;    
  analogInputVal = analogRead(A0);           // Read A0 port for required output memory
  pushButton = digitalRead(pushButton_pin);
  output_m = checkanalog(analogInputVal);   
  if (!pushButton){                              // Button press
  irSignal = loadEepromValues(output_m);   
      if (codeValue != 0 || Command!= 0){                        // Able to read data from EEPROM in the system
      Serial.print("Pressed, sending setup ");
      digitalWrite(STATUS_PIN, HIGH);
      Serial.println(output_m); 
      // Serial.print(" : ");
      // Serial.println(codeValue, HEX); 
      // Printeverything(codeLen, irSignal, output_m, codeType, codeValue, Command, Address, Datalength);
      SendCode(Address, Command, irSignal, Datalength);
      digitalWrite(STATUS_PIN, LOW);
      codeValue_previous = codeValue;
      delay(50);
    }else{
     Serial.println("Please input data first!");  // Unable to read data from EEPROM in the system
     delay(50);
    }
  }else if(IrReceiver.decode()){ //&& codeValue != codeValue_previous
    // IrReceiver.printIRResultRawFormatted(&Serial, true);
    irSignal = StoreCode(irSignal, output_m);
    IrReceiver.resume();
  }
}


unsigned int *StoreCode(unsigned int *Signal, int analog){
  codeLen = IrReceiver.decodedIRData.numberOfBits;
  Datalength = IrReceiver.decodedIRData.rawDataPtr -> rawlen;
  codeValue = IrReceiver.decodedIRData.decodedRawData;
  Address = IrReceiver.decodedIRData.address;
  Command = IrReceiver.decodedIRData.command;
  codeType = IrReceiver.decodedIRData.protocol;
  Signal = allocate_a(Datalength);
  for(int i = 1; i< Datalength; i++){
    Signal[i - 1] = IrReceiver.decodedIRData.rawDataPtr -> rawbuf[i]*USECPERTICK;
  }
  if (codeType == UNKNOWN){
    Serial.println(F("Recived Unknown Signal"));
    return 0;
  } else if (codeLen > 32 || Datalength > 69){
    Serial.print(F("Recived Signal larger than 32 bit, please change a new one or restart the device"));
    Signal = loadEepromValues(output_m);
    return Signal;
  } else{
      Serial.print(F("Recived "));
      Serial.print(codeLen);
      Serial.print(F(" bit "));
      Serial.print(getProtocolString(IrReceiver.decodedIRData.protocol));
      Serial.print(F(":"));
      Serial.println(codeValue, HEX);
      digitalWrite(STATUS_PIN, HIGH); 
      writeEepromVal(Signal,analog);
      digitalWrite(STATUS_PIN, LOW);
      return Signal;
  }
}

  
#define codeTypeEAddr     0    //0,1,2 to store 3 different codeType
#define codeLenEAddr      3    //3,4,5 to store 3 different codeLen
#define AddressEAddr      6    //6,7,8 to store 3 different Address
#define CommandEAddr      9    //9,10,11 to store 3 different Command
#define codeValueEAddr    12   //12,13,14 to store 3 different Command

#define DatalengthEAddr   15   //15,16,17 to store 3 different Command

//irSignalEAddr 20 ~ 160 for array 1 and each address for 2 bit data
//irSignalEAddr 160 ~ 300 for array 2 and each address for 2 bit data
//irSignalEAddr 300 ~ 440 for array 3 and each address for 2 bit data

void writeEepromVal(unsigned int *irSignal, int analog){ 
   int irSignalEAddr = 20;
   EEPROM.write(codeTypeEAddr + analog, codeType);
   delay(10);
   EEPROM.write(codeLenEAddr + analog, codeLen);
   delay(10);
   EEPROM.put(codeValueEAddr + analog, codeValue);
   delay(10);
   EEPROM.put(AddressEAddr + analog, Address);
   delay(10);
   EEPROM.put(CommandEAddr + analog, Command);
   if(codeType == PULSE_DISTANCE){
    EEPROM.write(DatalengthEAddr + analog, Datalength);
    delay(10);  
    irSignalEAddr = irSignalEAddr + 140*analog;

    for (int i = 0; i < Datalength-1; i++) {
      EEPROM.write(irSignalEAddr, irSignal[i] >> 8);
      EEPROM.write(irSignalEAddr + 1, irSignal[i] & 0xFF);
//      Serial.print(irSignalEAddr);
//      Serial.print(" : ");
//      Serial.print(irSignal[i]);
//      Serial.print("    ");      
      delay(10);
      irSignalEAddr += 2;
      }
      Serial.println("    "); 
      // PrintArray(irSignal,analog);
  }   
}


void SendCode(uint8_t sAddress, uint8_t sCommand, unsigned int *irSignal, int Datalength){ 
  uint8_t sRepeats = 0;
  if (codeType == NEC){
    Serial.flush();
    IrSender.sendNEC(sAddress, sCommand, sRepeats);
    delay(DELAY_AFTER_SEND);     
  }else if (codeType == PULSE_DISTANCE){
    Serial.flush();
    IrSender.sendRaw(irSignal, Datalength - 1, NEC_KHZ);
    delay(DELAY_AFTER_SEND); 
  }else if (codeType == SONY){
    Serial.flush();
    IrSender.sendSony(sAddress & 0x1FFF, sCommand & 0x7F, sRepeats, SIRCS_20_PROTOCOL);
    delay(DELAY_AFTER_SEND);
  }else{
    Serial.print(F("Recived Unknown Signal"));
  }
} 
  


unsigned int * loadEepromValues(int analog){
  unsigned int *Signal;
  int irSignalEAddr = 20;

  codeType = EEPROM.read(codeTypeEAddr + analog);
  delay(10);

  if (codeType == UNKNOWN){
    Serial.println(F("UNKNOWN SIGNAL, LOADING BREAK"));
    return 0;
  }
  codeLen = EEPROM.read(codeLenEAddr + analog);
  delay(10);   
  EEPROM.get(codeValueEAddr + analog, codeValue);
  delay(10);
  EEPROM.get(AddressEAddr + analog, Address);
  delay(10);
  EEPROM.get(CommandEAddr + analog, Command); 
  Serial.println("Loading Data");
  if(codeType == PULSE_DISTANCE){
    Datalength= 52;
    Signal = allocate_a(Datalength - 1);
    delay(10);  
    irSignalEAddr = irSignalEAddr + 140*analog;
    for (int i = 0; i < Datalength-1; i++) {
      Signal[i] = (EEPROM.read(irSignalEAddr) << 8) + EEPROM.read(irSignalEAddr + 1);      
      delay(10); 
      irSignalEAddr += 2;
     } 
  return Signal;
 }
}

void printrest(int irSignalEAddr, int Datalength){
    unsigned int *Signaltest;
    Signaltest = allocate_a(Datalength - 1);
    for (int i = 0; i < Datalength-1; i++) {
      Signaltest[i] = (EEPROM.read(irSignalEAddr) << 8) + EEPROM.read(irSignalEAddr + 1);  
      Serial.print(irSignalEAddr);
      Serial.print(" : ");
      Serial.print(Signaltest[i]);
      Serial.print("    ");      
      delay(10); 
      irSignalEAddr += 2;
     }
     Serial.println("    "); 
     delete[] Signaltest;
}


void Printeverything(int codeLen, unsigned int *irSignal, int analog, int codeType, unsigned long codeValue, uint8_t Command, uint8_t Address, int Datalength){
  Serial.print(F(" codeLen : "));
  Serial.println(codeLen);
  for(int i = 0; i < Datalength-1; i++ ){
    Serial.print(irSignal[i]);
    Serial.print(F(" "));
  }
  Serial.println(F(" "));
  Serial.print(F("Setup : "));  
  Serial.println(analog);
  Serial.print(F("codeType : "));
  Serial.print(codeType);  
  Serial.print(F(" codeValue : "));
  Serial.print(codeValue, HEX); 
  Serial.print(F(" Address : "));
  Serial.print(Address, HEX);
  Serial.print(F(" Command :  "));
  Serial.print(Command, HEX);
  Serial.print(F(" Datalength : "));
  Serial.println(Datalength); 
}

unsigned int *allocate_a( int size )
{
    unsigned int * data = new unsigned int[size];
    return data;
}



int checkanalog (int analogInput){
  if (analogInput < 400){
    return 0;
  }else if (analogInput >= 400 && analogInput < 800){
    return 1;
  } else {
    return 2;
  }
}
