/*
       __          ____        _____                       __    _ __
      / /   ____  / __ \____ _/ ___/____  ____ _________  / /   (_) /_
     / /   / __ \/ /_/ / __ `/\__ \/ __ \/ __ `/ ___/ _ \/ /   / / __ \
    / /___/ /_/ / _, _/ /_/ /___/ / /_/ / /_/ / /__/  __/ /___/ / /_/ /
   /_____/\____/_/ |_|\__,_//____/ .___/\__,_/\___/\___/_____/_/_.___/
                                /_/
   Author: m1nhle, mtnguyen
   Code : RF210_RAK3172_bridge : Provide on ESP32 Serial port a direct access in write and read to Rak3172 module for AT Command mode


*/

// ESP32 C3 SERIAL1 (second UART)
HardwareSerial mySerial1(1);

int rxPin = 20;
int txPin = 21;
uint8_t button = 0;

String devAddr = "260B244A";
String nwkkey = "6A6C173B8C621968EED7B5C2200E9F1D";
String appskey = "EF9694A568FDC7B7FF2E6AD34CB54769";

void setup()
{
  Serial.begin(115200);
  delay(1000);
 Serial.println("Accelero Test");
  
  pinMode(txPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(10, OUTPUT); //Rak enable
  pinMode(4, OUTPUT); // LED
   pinMode(9, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(9), i_button_isr, RISING);
 // pinMode(1, OUTPUT); // GNSS enable
  digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  

  //  digitalWrite(1, LOW);   // switch off GPS
    digitalWrite(10, HIGH); // Switch on RAK
    delay(1000);
  mySerial1.begin(115200, SERIAL_8N1, rxPin, txPin);
  delay(1000);  
  mySerial1.println("ATE");
  delay(100);
  
    while (mySerial1.available()){
      Serial.write(mySerial1.read()); // read it and send it out Serial (USB)
    }


  mySerial1.println("AT+NWM=1"); // Set LoRaWan mode
  delay(300); 
mySerial1.println("AT+NJM=0"); // Set ABP
  delay(300);

  mySerial1.println("AT+BAND=4");// Set EU868 frequency band
  delay(300);
  mySerial1.println("AT+DR=2");// Set EU868 frequency band
  delay(300);
  mySerial1.println("AT+NJM=0"); // Set ABP
  delay(300);
  //Serial.printf("Dev ADR = %s \n", devAddr);
  //mySerial1.printf("AT+DEVADDR=%s\n",devAddr);
  mySerial1.printf("AT+DEVADDR=");
  mySerial1.println(devAddr);
  //delay(300);
    mySerial1.printf("AT+NWKSKEY=");
  mySerial1.println(nwkkey);
 // delay(300);  
  mySerial1.printf("AT+APPSKEY=");
  mySerial1.println(appskey);
   if (mySerial1.available())
  { // If anything comes in Serial1 (pins 4 & 5)
    while (mySerial1.available())
      Serial.write(mySerial1.read()); // read it and send it out Serial (USB)
  }
    delay(4000);
    while (mySerial1.available()){
      Serial.write(mySerial1.read()); // read it and send it out Serial (USB)
    }

  delay(4000);


  mySerial1.println("ATC+ACC_READ");
  delay(300);  
 
}

void loop()
{
 
  
  
  if (Serial.available()) {      // If anything comes in Serial (USB),

    mySerial1.write(Serial.read());   // read it and send it out mySerial1 (pins 0 & 1)      

  }

  if (mySerial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)

    
        String ax_string=mySerial1.readStringUntil(' ');
        float ax= ax_string.toFloat();
         

            String ay_string=mySerial1.readStringUntil(' ');
        float ay= ay_string.toFloat();
        
            String az_string=mySerial1.readStringUntil('\n');
        float az= az_string.toFloat();

float tot = sqrt(ax*ax+ay*ay+az*az);

         Serial.println(tot);

if (tot <0.4)
{
Serial.println("Fall detect");
bool ft = true;

unsigned char mydata[4];
mydata[0] = 0x1; // CH1
mydata[1] = 0x1; // Temp
mydata[2] = ft;


char str[32] = "";
array_to_string(mydata, 3, str);

mySerial1.printf("AT+SEND=3:");
mySerial1.println(str);
   delay(5300);
  if (mySerial1.available())
  { // If anything comes in Serial1 (pins 4 & 5)
    while (mySerial1.available())
      Serial.write(mySerial1.read()); // read it and send it out Serial (USB)
  }
  Serial.println("AT set complete with downlink");
  delay(30000);
  Serial.println("Uplink");

}

  }
}

void i_button_isr(void) {
button++; 
}


void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
