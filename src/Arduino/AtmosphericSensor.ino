//bme280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
#define BME280_ADDRESS (0x76)

//ccs811
#include <Adafruit_CCS811.h>
Adafruit_CCS811 ccs;

//radiation
#include<SPI.h>
#define MEAS_PERIOD 1000
#define ARRAY_SIZE 60
#define BAT_BUF_SIZE 10
unsigned int countsBuffer[ARRAY_SIZE];
float radiationBuffer[ARRAY_SIZE];
unsigned int counts = 0;
unsigned int cpm = 0;
unsigned long int previousMillis = 0;
float mkSvHr = 0.0;
float factor = 0.00081; //convertion factor for M4011 tube
unsigned int backgroundTube = 25; // own background pulse/minute
int index = 0;

//bluetooth
#include <SoftwareSerial.h>
int gRxPin = 4; 
int gTxPin = 3; 
SoftwareSerial BTSerial(gRxPin, gTxPin);

//battery
float batteryBuffer[ARRAY_SIZE];
float maxBatVoltage = 4.2; // battery max voltage
float minBatVoltage = 2.4; //battery min voltage
float maxAnalogVoltage = 1.1; //max analog voltage
float maxVoltage = 5.0;
float maxAnalogValue;
float splitMaxBatVolt;
float splitMinBatVolt;
float coefVolt = 0.985; //based on real battery voltage
//Voltage Splitter: R2 / (R1 + R2) R1 ~ 1000, R2 ~ 220

/* Pin map
 *  Device Rx pin on D3 - Tx Arduino
 *  Device Tx pin on D4 - Rx Arduino
 *  battery + on pin A1
 *  SDA pin on A4
 *  SCL pin on A5
 *  D9 for connection state check
 *  D2 for Radiation sensor read
 */

void tubeImpulse()
{
    counts++;
}

void setup()
{
    Wire.begin();
    analogReference(INTERNAL);
    analogRead(1);
    BTSerial.begin(9600);
    //Serial.begin(9600);
    pinMode(A1,INPUT);
    pinMode(9,INPUT);
    pinMode(2,INPUT);
    bme.begin(0x76);
    ccs.begin(0x5A);
    attachInterrupt(0,tubeImpulse,FALLING);

    for(int k = 0; k < ARRAY_SIZE; k++)
    {
        radiationBuffer[k] = 0.0;
        countsBuffer[k] = 0;
        batteryBuffer[k] = 0.0;
    }

    splitMaxBatVolt = maxAnalogVoltage * maxBatVoltage / maxVoltage * coefVolt;
    splitMinBatVolt = maxAnalogVoltage * minBatVoltage / maxVoltage * coefVolt;
    maxAnalogValue = splitMaxBatVolt * 1023.0 / maxAnalogVoltage * coefVolt;
} 

float getMapVal(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
 
void loop()
{   
    //get data from radiation
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis < MEAS_PERIOD)
        return;

    previousMillis = currentMillis;
    countsBuffer[index] = counts;
    counts = 0; 
  
    for(int k = 0; k < ARRAY_SIZE; k++)
        cpm += countsBuffer[k];

    if(cpm > backgroundTube)
      cpm -= backgroundTube;
    
    mkSvHr = cpm * (float)ARRAY_SIZE * factor;
    radiationBuffer[index] = mkSvHr;
    mkSvHr = 0.0;



    for(int k = 0; k < ARRAY_SIZE; k++)
        mkSvHr += radiationBuffer[k];

    mkSvHr = (float)(mkSvHr / ARRAY_SIZE);
    batteryBuffer[index] = splitMaxBatVolt * (float)analogRead(1) / maxAnalogValue;
    float batteryVoltage = 0.0;

    for(int k = 0; k < BAT_BUF_SIZE; k++)
        batteryVoltage += batteryBuffer[k];
        
    batteryVoltage = (float)(batteryVoltage / BAT_BUF_SIZE);
    
    cpm = 0;
    index++;

    if(index >= ARRAY_SIZE)
        index = 0;
  
    //get data from bme sensor
    double temp = bme.readTemperature();
    double pressure = bme.readPressure() / 100.0 * 0.75;
    double hum = bme.readHumidity();
    float chargeLevel = getMapVal(batteryVoltage, splitMinBatVolt, 
        splitMaxBatVolt, 0.0, 100.0);

    if(chargeLevel < 0.0)
      chargeLevel = 0.0;
   else if(chargeLevel > 100.0)
      chargeLevel = 100.0;

    //get data from CCS811
    float co = 0.0;
    float tvoc = 0.0;
    
    if(ccs.available() && !ccs.readData())
    {
        co = ccs.geteCO2();
        tvoc = ccs.getTVOC();
        ccs.setTempOffset(temp);
    }

    //send data
    String output = String(temp,1) + " " + String(hum,1) + " "
    + String(pressure,0) + "/" + String(chargeLevel,0)
    + "/" + String(co,1) + " " + String(tvoc,1)
    + "/" + String(mkSvHr,2);

    BTSerial.println(output);
    //Serial.println(output);
}
