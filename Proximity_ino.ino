// Proximity Warning
// adapted and revised from Adafruit's github

================
#include <Wire.h>

// the i2c address
#define VCNL4000_ADDRESS 0x13

// commands and constants
#define VCNL4000_COMMAND 0x80
#define VCNL4000_PRODUCTID 0x81
#define VCNL4000_IRLED 0x83
#define VCNL4000_AMBIENTPARAMETER 0x84
#define VCNL4000_AMBIENTDATA 0x85
#define VCNL4000_PROXIMITYDATA 0x87
#define VCNL4000_SIGNALFREQ 0x89
#define VCNL4000_PROXINITYADJUST 0x8A

#define VCNL4000_3M125 0
#define VCNL4000_1M5625 1
#define VCNL4000_781K25 2
#define VCNL4000_390K625 3

#define VCNL4000_MEASUREAMBIENT 0x10
#define VCNL4000_MEASUREPROXIMITY 0x08
#define VCNL4000_AMBIENTREADY 0x40
#define VCNL4000_PROXIMITYREADY 0x20
  
int ledPin = 9;
int output = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 255);

  Wire.begin();

  uint8_t rev = read8(VCNL4000_PRODUCTID);
  
  if ((rev & 0xF0) != 0x10) {
    // Serial.println("Sensor not found :(");
    while (1);
  }
    
  
  write8(VCNL4000_IRLED, 20);        // set to 20 * 10mA = 200mA
  //  Serial.print("IR LED current = ");
  //  Serial.print(read8(VCNL4000_IRLED) * 10, DEC);
  //  Serial.println(" mA");
  
  //  write8(VCNL4000_SIGNALFREQ, 3);
  //  Serial.print("Proximity measurement frequency = ");
  uint8_t freq = read8(VCNL4000_SIGNALFREQ);
  //  if (freq == VCNL4000_3M125) Serial.println("3.125 MHz");
  //  if (freq == VCNL4000_1M5625) Serial.println("1.5625 MHz");
  //  if (freq == VCNL4000_781K25) Serial.println("781.25 KHz");
  //  if (freq == VCNL4000_390K625) Serial.println("390.625 KHz");
  
  write8(VCNL4000_PROXINITYADJUST, 0x81);
  //  Serial.print("Proximity adjustment register = ");
  //  Serial.println(read8(VCNL4000_PROXINITYADJUST), HEX);
  
  // arrange for continuous conversion
  //   write8(VCNL4000_AMBIENTPARAMETER, 0x89);

}

uint16_t readProximity() {
  write8(VCNL4000_COMMAND, VCNL4000_MEASUREPROXIMITY);
  while (1) {
    uint8_t result = read8(VCNL4000_COMMAND);
    //Serial.print("Ready = 0x"); Serial.println(result, HEX);
    if (result & VCNL4000_PROXIMITYREADY) {
      return read16(VCNL4000_PROXIMITYDATA);
    }
    delay(1);
  }
}



void loop() {

  // read ambient light!
  write8(VCNL4000_COMMAND, VCNL4000_MEASUREAMBIENT | VCNL4000_MEASUREPROXIMITY);
  
  while (1) {
    uint8_t result = read8(VCNL4000_COMMAND);
    //Serial.print("Ready = 0x"); Serial.println(result, HEX);
    if ((result & VCNL4000_AMBIENTREADY)&&(result & VCNL4000_PROXIMITYREADY)) {
      //  Serial.print("Ambient = ");
      //  Serial.print(read16(VCNL4000_AMBIENTDATA));
      //  Serial.print("\t\tProximity = ");
      //  Serial.println(read16(VCNL4000_PROXIMITYDATA));
      
      int proximity = read16(VCNL4000_PROXIMITYDATA);
      // Serial.println(proximity);

      if (proximity > 2400) { // approx. 50mm from the sensor 
        digitalWrite(ledPin, HIGH);
        Serial.println("1"); // send serial prints to Processing for different situations 
        // This print will allow Processing to know when something happened, like the range of values for the sensor.
        delay (300);
        digitalWrite(ledPin, LOW);
        Serial.println("2");
        delay (100);
      }
      else if (proximity < 2400) {
        output = map(proximity, 2160, 2400, 0, 200);
        analogWrite(ledPin, output);
        Serial.println("0");
        delay (50);
      }
      break;
    }
    delay(10);
  }
  
   delay(100);
 }

// Read 1 byte from the VCNL4000 at 'address'
uint8_t read8(uint8_t address)
{
  uint8_t data;

  Wire.beginTransmission(VCNL4000_ADDRESS);
#if ARDUINO >= 100
  Wire.write(address);
#else
  Wire.send(address);
#endif
  Wire.endTransmission();

  delayMicroseconds(170);  // delay required

  Wire.requestFrom(VCNL4000_ADDRESS, 1);
  while(!Wire.available());

#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}


// Read 2 byte from the VCNL4000 at 'address'
uint16_t read16(uint8_t address)
{
  uint16_t data;

  Wire.beginTransmission(VCNL4000_ADDRESS);
#if ARDUINO >= 100
  Wire.write(address);
#else
  Wire.send(address);
#endif
  Wire.endTransmission();

  Wire.requestFrom(VCNL4000_ADDRESS, 2);
  while(!Wire.available());
#if ARDUINO >= 100
  data = Wire.read();
  data <<= 8;
  while(!Wire.available());
  data |= Wire.read();
#else
  data = Wire.receive();
  data <<= 8;
  while(!Wire.available());
  data |= Wire.receive();
#endif
  
  return data;
}

// write 1 byte
void write8(uint8_t address, uint8_t data)
{
  Wire.beginTransmission(VCNL4000_ADDRESS);
#if ARDUINO >= 100
  Wire.write(address);
  Wire.write(data);  
#else
  Wire.send(address);
  Wire.send(data);  
#endif
  Wire.endTransmission();
}
