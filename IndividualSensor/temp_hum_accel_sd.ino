#include <SD.h>
#include <SparkFun_HIH4030.h>
#include <Wire.h>


#define aref_voltage 5

\\ sensor pins
int tempSensor = 1;
int xPin = 13;
int yPin = 14;
int zPin = 15;
int SDPin = 53;
int tempReading; 


#define HIH4030_OUT A0
#define HIH4030_SUPPLY 5
HIH4030 sensorSpecs(HIH4030_OUT, HIH4030_SUPPLY);

 
File myFile;
String data = "";
 
void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(SDPin, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("log.csv", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("Beginning new log at " + String(hour) + ":" + String(minutes) + ":" + String(second));
  // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

String printHumData(HIH4030 sensor, float temperature){
  
  Serial.print("Temperature = ");
  Serial.print(temperature);
  float tempF = (temperature * 9/5) + 32;
  Serial.print(" C ("); Serial.print((temperature * 9/5) + 32); Serial.println(" F)");
  Serial.print("Sensor Voltage = ");
  Serial.print(sensor.vout());
  Serial.println(" V");
  Serial.print("Relative Humidity = ");
  Serial.print(sensor.getSensorRH());
  Serial.println(" %");
  Serial.print("True Relative Humidity = ");
  Serial.print(sensor.getTrueRH(temperature));
  Serial.println(" %");


  //print to sd
  //temp, relative humidity, absolute humidity
  String data = "";
  data += String(temperature) + ",";
  data += String(sensor.getSensorRH()) + ",";
  data += String(sensor.getTrueRH()) + ",";
  
}

String printAccelData() {
  int x,y,z;

  x = analogRead(13);       // read analog input pin 0
  y = analogRead(14);       // read analog input pin 1
  z = analogRead(15);       // read analog input pin 1
  Serial.print("accelerations are x, y, z: ");
  Serial.print(x-300, DEC);    // print the acceleration in the X axis
  Serial.print(" ");       // prints a space between the numbers
  Serial.print(y-300, DEC);    // print the acceleration in the Y axis
  Serial.print(" ");       // prints a space between the numbers
  Serial.println(z-300, DEC);  // print the acceleration in the Z axis

  String data;

  data += x + ",";
  data += y + ",";
  data += z + ",";
}

float getTemperature()
{
 tempReading = analogRead(tempSensor); 
 float voltage = tempReading * aref_voltage;
 voltage /= 1024.0; 
 float temperatureC = (voltage - 0.5) * 100 ; 
 Serial.print(temperatureC); Serial.println(" degrees C");
return temperatureC; 
}

void loop() {
  data = "";

  data = data + String(hour)                    + ':';
  data = data + String(minute)                  + ':';
  data = data + String(second)                  + ',';

  float temp = getTemperature();                  
  data += printHumData(sensorSpecs, temp);
  data += printAccelData();    
  Serial.println("");

  //print data=hour:minute:second,tempC,relHum,absHum,xAccel,yAccel,zAccel

  if(SD.exists("data.csv")) {
    if(myFile = SD.open("data.csv", FILE_WRITE)){
        myFile.println(data);
        myFile.close();
    }
    else {
      Serial.println("ERROR: UNABLE TO OPEN DATA FILE");
    }
  }
  else {
    Serial.println("ERROR: DATA.CSV NON EXISTENT");
  }
               
  delay(1000);   

  

}
