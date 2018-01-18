#include <SparkFun_HIH4030.h>
#include <Wire.h>
#define aref_voltage 5
int tempSensor = 1;
int tempReading; 
#define HIH4030_OUT A0
#define HIH4030_SUPPLY 5
HIH4030 sensorSpecs(HIH4030_OUT, HIH4030_SUPPLY);

void setup(void)
{
  Serial.begin(9600);                    
}
void printData(HIH4030 sensor, float temperature){
  Serial.print("Temperature = ");
  Serial.print(temperature);
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

void loop(void)
{  
  float temp = getTemperature();                  
  printData(sensorSpecs, temp);    
  Serial.println("");             
  delay(1000);                   
  
}


