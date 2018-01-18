#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!

void setup() {
    Serial.begin(9600);
    analogReference(EXTERNAL);
}
 
void loop(){
    float pressure = readPressure(A0);
    float millibars = pressure/100;
 
    Serial.println();
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" pascals");
    Serial.print("Pressure = ");
    Serial.print(millibars);
    Serial.println(" millibars");
    delay(500);
}
 
/* Reads pressure from the given pin.
* Returns a value in Pascals
*/
float readPressure(int pin){
    int pressureValue = analogRead(pin);
    float pressure=((pressureValue/1024.0)+0.095)/0.000009;
    return pressure;
}
