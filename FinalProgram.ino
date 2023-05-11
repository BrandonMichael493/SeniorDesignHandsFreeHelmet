#include <SoftwareSerial.h>
#include <ArduinoBlue.h>
#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


#define LED_PIN     7
#define NUM_LEDS    112
double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
double xO = -1000000, yO = -1000000 , zO = -1000000; //dumb values, easy to spot problem
double xA = -1000000, yA = -1000000 , zA = -1000000; //dumb values, easy to spot problem

int batteryV = A0;
int solarV = A1;

int brightness = 50;
float levelBatt;
float levelSolar;
float voltsBatt;
float voltsSolar;
int perc;
CRGB leds[NUM_LEDS];
// The bluetooth tx and rx pins must be supported by software serial.
// Visit https://www.arduino.cc/en/Reference/SoftwareSerial for unsupported pins.
// Bluetooth TX -> Arduino D8
const int BLUETOOTH_TX = 10;
// Bluetooth RX -> Arduino D7
const int BLUETOOTH_RX = 9;

int prevThrottle = 49;
int prevSteering = 49;
int throttle, steering, sliderVal, button, sliderId;

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);
ArduinoBlue phone(bluetooth); // pass reference of bluetooth object to ArduinoBlue constructor.


/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);


int HS = 0;
int HazS = 0;
int BS = 0;
// Setup code runs once after program starts.
void setup() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);
    FastLED.show(); 
  
    // Start serial monitor at 9600 bps.
    Serial.begin(9600);

    // Start bluetooth serial at 9600 bps.
    bluetooth.begin(9600);

    // delay just in case bluetooth module needs time to "get ready".
    delay(200);

    Serial.println("setup complete");

    //while (!Serial) delay(10);  // wait for serial port to open!

    Serial.println("Orientation Sensor Test"); Serial.println("");

    /* Initialise the sensor */
    if (!bno.begin())
    {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      while (1);
    }

      delay(1000);
}

// Put your main code here, to run repeatedly:
void loop(void) {

    sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    //bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    //bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
    //bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    //bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

    printEvent(&orientationData);
    printEvent(&linearAccelData);

    int8_t boardTemp = bno.getTemp();
    Serial.println();
    Serial.print(F("temperature: "));
    Serial.println(boardTemp);
    Serial.print(voltsSolar);

  uint8_t system, gyro, accel, mag = 0;
 bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.println();
  Serial.print("Calibration: Sys=");
  Serial.print(system);
  Serial.print(" Gyro=");
  Serial.print(gyro);
  Serial.print(" Accel=");
  Serial.print(accel);
  Serial.print(" Mag=");
  Serial.println(mag);

  Serial.println("--");
  delay(BNO055_SAMPLERATE_DELAY_MS);

    if(zO >= 50){   
        for (int i=0; i<10; i++){           
         for (int i = 27; i <= 41; i++) {
            leds[i] = CRGB ( 255, 255, 0);
            FastLED.show();
            delay(10);
  }
         for (int i = 27; i <= 41; i++) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);
  }
        }
  }

   if(zO <=-50){   
        for (int i=0; i<10; i++){           
         for (int i = 83; i >= 68; i--) {
            leds[i] = CRGB ( 255, 255, 0);
            FastLED.show();
            delay(10);
  }
         for (int i = 83; i >= 68; i--) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);
  }
        }
  }  


  if(xA <= -10){
        for (int i = 0; i <= 26; i++) {
            leds[i] = CRGB ( 255, 0, 0);
            FastLED.show();
            delay(10);        
      }
        for (int i = 110; i >= 84; i--) {
            leds[i] = CRGB ( 255, 0, 0);
            FastLED.show();
            delay(10);
        }
        delay(1000);
      
         for (int i = 0; i <= 26; i++) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);        
        }
        for (int i = 110; i >= 84; i--) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);
        }
        

         
       }             

    // ID of the button pressed pressed.
    button = phone.getButton();

    // Returns the text data sent from the phone.
    // After it returns the latest data, empty string "" is sent in subsequent.
    // calls until text data is sent again.
    String str = phone.getText();

    // Throttle and steering values go from 0 to 99.
    // When throttle and steering values are at 99/2 = 49, the joystick is at center.

    // ID of the slider moved.
    sliderId = phone.getSliderId();

    // Slider value goes from 0 to 200.
    sliderVal = phone.getSliderVal();

    // Display button data whenever its pressed.
    if (button != -1) {
        Serial.print("Button: ");
        Serial.println(button);
    } 
  
    // Display slider data when slider moves
    if (sliderId != -1) {
        Serial.print("Slider ID: ");
        Serial.print(sliderId);
        Serial.print("\tValue: ");
        Serial.println(sliderVal);
    }

  

    // If a text from the phone was sent print it to the serial monitor
    if (str != "") {
        Serial.println(str);
    }

    // Send string from serial command line to the phone. This will alert the user.
    if (Serial.available()) {
        Serial.write("send: ");
        String str = Serial.readString();
        phone.sendMessage(str); // phone.sendMessage(str) sends the text to the phone.
        Serial.print(str);
        Serial.write('\n');
    }
    if (str.equalsIgnoreCase("Info")){
      phone.sendMessage("Command Directory: \n Battery - See battery level \n Solar - See Solar Voltage \n Compass - See compass direction");
    }
    if (str.equalsIgnoreCase("Battery")){
      levelBatt = analogRead(batteryV);
      voltsBatt = (levelBatt/204.8);      
      String voltsStr = String(voltsBatt);  
      perc = map(levelBatt, 735, 860.16, 0, 100);   
      String percStr = String(perc); 
      phone.sendMessage(voltsStr + " Volts \n" + percStr + "%");
    }

    if (str.equalsIgnoreCase("Solar")){      
      String voltsSTR = String(voltsSolar); 
      phone.sendMessage(voltsSTR + " Volts");
    }

    levelSolar = analogRead(solarV);
    voltsSolar = ((levelSolar/204.8)*2); 


  if (voltsSolar <= 0.25){
        for (int i = 42; i <= 67; i++) {
          leds[i] = CRGB ( 255, 255, 255);
          FastLED.show();
          delay(40);  
        }
  }
  else{
            for (int i = 67; i >= 42; i--) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(40);
            HS = 0;           
            }
  }  



//Start of App Functionality for All Controls
      
    if(sliderId == 0){  
      if(sliderVal <= 200 && sliderVal >=1){
        brightness = (sliderVal/2);
        FastLED.setBrightness(brightness);
        delay(100);      
        FastLED.show();      
      }       
    }
     if(button == 6){      
       if (HS == 0){
        HS = 1;
        for (int i = 42; i <= 67; i++) {
          leds[i] = CRGB ( 255, 255, 255);
          FastLED.show();
          delay(40);  
        }
       }
        else{
            for (int i = 67; i >= 42; i--) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(40);
            HS = 0;           
        }  
          
    }
     }
       if(button == 2 ){   
        for (int i=0; i<10; i++){           
         for (int i = 27; i <= 41; i++) {
            leds[i] = CRGB ( 255, 255, 0);
            FastLED.show();
            delay(10);
  }
         for (int i = 27; i <= 41; i++) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);
  }
        }
  }

       if(button == 3){   
        for (int i=0; i<10; i++){           
         for (int i = 83; i >= 68; i--) {
            leds[i] = CRGB ( 255, 255, 0);
            FastLED.show();
            delay(10);
  }
         for (int i = 83; i >= 68; i--) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);
  }
        }
  }  

          
      if(button == 4){
        if (BS == 0){
        for (int i = 0; i <= 26; i++) {
            leds[i] = CRGB ( 255, 0, 0);
            FastLED.show();
            delay(10);        
      }
        for (int i = 110; i >= 84; i--) {
            leds[i] = CRGB ( 255, 0, 0);
            FastLED.show();
            delay(10);
        }
        BS = 1;
        }        
       else{
         for (int i = 0; i <= 26; i++) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);        
        }
        for (int i = 110; i >= 84; i--) {
            leds[i] = CRGB ( 0, 0, 0);
            FastLED.show();
            delay(10);
        }
        BS = 0;

         
       }             
}
      if(button == 5){
        if (HazS == 0){        
          for (int i = 0; i <= 111; i++) {
            leds[i] = CRGB (55, 55, 0);
            delay(1);
            if(i >= 50){
            leds[i-50] = CRGB (0, 0, 0);            
            FastLED.show();
            i++;
            
                
          }
                    
        for (int i = 61; i <= 111; i++) {   
            leds[i] = CRGB (0, 0, 0);
            delay(1);
            FastLED.show();
        }                          
          }
          HazS = 1;
        } 
          else{
              for (int i = 0; i <= 111; i++) {   
                leds[i] = CRGB (0, 0, 0);
                delay(1);
                FastLED.show();
          }
          HazS = 0;
}
  //End of App Functionality for All Controls.
}
}
void printEvent(sensors_event_t* event) {
 

  if (event->type == SENSOR_TYPE_ORIENTATION) {
    Serial.print("Orient:");
    xO = event->orientation.x;
    yO = event->orientation.y;
    zO = event->orientation.z;
  }

  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    Serial.print("Linear:");
    xA = event->acceleration.x;
    yA = event->acceleration.y;
    zA = event->acceleration.z;
  }

  else {
    Serial.print("Unk:");
  }

  Serial.print("\tx= ");
  Serial.print(x);
  Serial.print(" |\ty= ");
  Serial.print(y);
  Serial.print(" |\tz= ");
  Serial.println(z);
}


      



