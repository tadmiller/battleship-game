#include <Wire.h>
byte a = 0;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  while (1)
  {
    while (1 < Wire.available())
    { // loop through all but the last
      char c = Wire.read(); // receive byte as a character
      Serial.print(c);         // print the character
    }
    int x = Wire.read();    // receive byte as an integer
    Serial.println(x);         // print the integer
    delay(random(1, 100));

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write("x is ");        // sends five bytes
    Wire.write(a);              // sends one byte
    Wire.endTransmission();    // stop transmitting

    a++;
  }

}



// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

}
