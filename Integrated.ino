#include <SD.h>
#include <SparkFun_HIH4030.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define aref_voltage 5

// sensor pins
int tempSensor = 1;
int xPin = 13;
int yPin = 14;
int zPin = 15;
int SDPin = 53;
int tempReading; 
#define HIH4030_OUT A0
#define THERMISTORPIN A7   

TinyGPS gps;
SoftwareSerial ss(3,2);

#define HIH4030_SUPPLY 5
HIH4030 sensorSpecs(HIH4030_OUT, HIH4030_SUPPLY);

// for thermistor
#define THERMISTORNOMINAL 10000 
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    
uint16_t samples[NUMSAMPLES];
 
File myFile;
String data = "";
 
void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
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
    //myFile.println("Beginning new log at " + String(hour) + ":" + String(minutes) + ":" + String(second));
    myFile.println("Beginning new log at " + String(millis()));
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
  String hum_data = "";
  hum_data += String(temperature) + ",";
  hum_data += String(sensor.getSensorRH()) + ",";
  hum_data += String(sensor.getTrueRH(temperature)) + ",";

  return hum_data;
  
}

String printAccelData() {
  float x,y,z;
  
  x = analogRead(13);     
  y = analogRead(14);     
  z = analogRead(15);  

  x = x-376;
  x /= 20.0;
  
  y = y-345;
  y /= 20.0;

  z -= 369;
  z /= 20.0;
  
  Serial.print("accelerations are x, y, z: ");
  Serial.print(x , DEC);    // print the acceleration in the X axis
  Serial.print(" ");       // prints a space between the numbers
  Serial.print(y , DEC);    // print the acceleration in the Y axis
  Serial.print(" ");       // prints a space between the numbers
  Serial.println(z , DEC);  // print the acceleration in the Z axis

  String accel_data = "";

  accel_data += String(x) + ',';
  accel_data += String(y) + ',';
  accel_data += String(z) + ',';

  return accel_data;
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

float printThermistorData() {
  uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  Serial.print("Average analog reading "); 
  Serial.println(average);
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
 
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *C");

  return steinhart;
}

String GPS() {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  String GPS_data = "";

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
       //6Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    
    float falt = gps.f_altitude();
    unsigned long age;
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long fix_age;
 
    gps.crack_datetime(&year, &month, &day,&hour, &minute, &second, &hundredths, &fix_age);
    Serial.print(month);
    Serial.print(",");
    Serial.print(day);
    Serial.print(",");
    Serial.println(year);
    
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
    gps.f_get_position(&flat, &flon, &age);

   GPS_data += String(year) + "/" + String(month) + "/" + String(day) + "_" + String(hour) + ":" + String(minute) + ":" + String(second) + ",";
    
//  Serial.print("LAT=");
//    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
//  Serial.print(" LON=");
 //   Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
//   Serial.print(" SAT=");
//   Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
//   Serial.print(" Altitude=");
    Serial.println(flat,6);
    Serial.println(flon,6);
    Serial.println(falt);

    GPS_data += String(flat) + ",";
    GPS_data += String(flon) +",";
    GPS_data += String(falt) + ",";
    
    
  }
 else{
  Serial.println("no fix");
  GPS_data += "no_fix,no_fix,no_fix,no_fix," ;
   }

   return GPS_data;
}

void loop() {
  data = "";
  
  //data = data + String(hour())                    + ':';
  //data = data + String(minute())                  + ':';
  //data = data + String(second())                  + ',';
  //data = data + String(millis()) + ',';

  float temp = getTemperature();                  
  data += printHumData(sensorSpecs, temp);
  data += String(printThermistorData()) + ',';
  data += printAccelData();   
  data += GPS(); 
  Serial.println("");

  //print data=hour:minute:second,tempC_tmp,relHum,absHum,tempC_thermistor,xAccel,yAccel,zAccel

  if(SD.exists("log.csv")) {
    if(myFile = SD.open("log.csv", FILE_WRITE)){
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
               
  delay(100);   

}
