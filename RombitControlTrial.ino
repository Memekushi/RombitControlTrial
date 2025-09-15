//Default up position is set to 90, meaning that 15 degrees to the right will end up being position 75.
//Gimbal has to be manually defaulted each time code is ran so input 90 to default it.

#include <Servo.h>
#include <Wire.h>
const int MPU_addr=0x68; //Signal path reset register
int16_t AcX,AcY,AcZ; // Accelerometer values set to Int but locked to 16 bits
 
int minVal=265;
int maxVal=402;
 
int x; // pitch
int y; // yaw
int z; // z is roll which is the imporatnt one here
int newUp; 

Servo myServo;

int goinUpUpUp(int sensor, int up = 90)
{
  sensor = z;
  
  // Add the target up to the sensor and handle 360 wrap-around
  int result = sensor + up;
  
  // Handle wrap-around if result exceeds 360
  if (result >= 360) 
  {
    result = result - 360;
  }
  
  return result;
}

//Input handling of single integer numbers as positions.
void userInput()
{
  if(Serial.available())
  {
    int inputPosition = Serial.parseInt();
      if(inputPosition > 0 && inputPosition < 180)
      {
        newUp = inputPosition;
      }
  }
}
void sensorSensing()
{
  //This function begins a transmission to the I2C peripheral device with the given address. 
  //Subsequently, queue bytes for transmission with the write() function and transmit them by calling endTransmission()
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); //writes data from 3B which is the accelerometer's X OUT, which I think grabs all X Y and Z data.
  Wire.endTransmission(false); //Doesnt end the transmission but rather sends the last set of bytes queued by write(). True = stop, False = restart
  Wire.requestFrom(MPU_addr,14,true); // requests bytes from first argument, how many from the second, and the third is t/f same as ^^^^^^^^^^^^^^^. True also means I2C bus is released, and false prevents bus from being released
  
  //The AcX/Y/Z variables were set to hold strictly 16 bits, so these functions grab the first 8 bits, shifts them to the left, 
  // and ORs them with the rest of the reading to combine into a 16-bit int
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();

  //Syntax: map(value, fromLow, fromHigh, toLow, toHigh)
  // So here the data of XYZ ranging from 265-402, gets mapped to -90-90 which I think encompasses the 180 degrees of the servo.
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  
  //This function just takes the arctan using the other values to calculate the value of XYZ and convert them from radians to degrees
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  
  Serial.print("AngleX= ");
  Serial.println(x);
 
  Serial.print("AngleY= ");
  Serial.println(y);
  
  Serial.print("AngleZ= ");
  Serial.println(z);
  Serial.println("-----------------------------------------");
}

void setup(){
  myServo.attach(9); // Servo is phsically on pin D9
  myServo.write(90); // Make servo go up on start

  Wire.begin(); // This initializes the wire library and joins the I2C bus as a controller
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); //Writing to 6B is the power managment register which serves to power on the MPU
  Wire.write(0); // Not exactly sure why you should write 0 first, maybe its to get it ready to recieve?
  Wire.endTransmission(true);
  Serial.begin(9600); //Not exactly sure what baud is but this is needed for serial to work
}

void loop(){
  userInput();
  sensorSensing();

  int adjustedZ = goinUpUpUp(z, newUp);
  int newZ = z + adjustedZ;

  Serial.print("Servo going to:");
  Serial.println(adjustedZ);
  Serial.println("-----------------------------------------");
  myServo.write(adjustedZ);
  delay(50);
}

/*
#include <Servo.h>
#include <Wire.h>
#define MPU6050 MPU_ADDR 0x68
#define MPU
Servo myServo; // Create servo object

void setup(){
//Servo setup
Serial.begin(9600);
myServo.attach(9); // Servo signal line to Arduino pin 9
myServo.write(90); // Calibrate Servo to up position
delay(100);

}

void loop(){
  if(Serial.available())
  {
    int inputPosition = Serial.parseInt();
    
    if(inputPosition > 0 && inputPosition < 180)
    {
      myServo.write(inputPosition);
      //Serial.println("Servo is at:");
      //Serial.println(inputPosition);
      delay(1000);
    }
  }
}
*/
