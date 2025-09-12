//Default up position is set to 90, meaning that 15 degrees to the right will end up being position 75.
//Gimbal has to be manually defaulted each time code is ran so input 90 to default it.

#include <Servo.h>
#include <Wire.h>
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
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
  if (result >= 360) {
    result = result - 360;
  }
  
  return result;
}

//Input handling of single integer numbers as positions.
void userInput(){
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
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  
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
  myServo.attach(9); // Servo is on pin D9
  myServo.write(90); // Make servo go up on start

  //Setup stuff for MPU
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
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
