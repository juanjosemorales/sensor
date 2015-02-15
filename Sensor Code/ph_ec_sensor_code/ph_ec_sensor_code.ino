/*
 # This sample code is used to test the pH meter V1.0.
 # Editor : localsprout
 # Ver    : 1.0
 # Product: analog pH meter
 # SKU    : SEN0161
 # Product name: Analog EC Meter
 # Product SKU : DFR0300
 */
#include <OneWire.h> //required for the temperature sensor that comes with the EC sensor

//PH Sensor Constants------------------------------------------------------------------
#define PHsensorPin A2            //pH meter Analog output to Arduino Analog Input 0
#define PHoffset 0.56           //deviation compensate

//EC Sensor Constants-------------------------------------------------------------------
#define StartConvert 0
#define ReadTemperature 1
byte ECsensorPin = A0;  //EC Meter analog output,pin on analog 1
byte DS18B20_Pin = 2; //DS18B20 signal, pin on digital 2
float temperature,ECcurrent; 
int delayFactor = 20000;

//Temperature chip i/o
OneWire ds(DS18B20_Pin);  // on digital pin 2

void setup(void)
{
  Serial.begin(9600);  
  TempProcess(StartConvert);   //let the DS18B20 start the convert
}
void loop(void)
{
  static float pHValue,pHvoltage,ECValue,ECVoltage;
  pHValue=analogRead(PHsensorPin);
  pHvoltage = pHValue*5.0/1024;
  pHValue = 3.5*pHvoltage+PHoffset;
  Serial.print("pH Voltage: "); Serial.print(pHvoltage);
  Serial.print(" pH: "); Serial.println(pHValue,2);
  delay(delayFactor);
  
  //EC Intervals
   ECValue = analogRead(ECsensorPin);
   ECVoltage=ECValue*(float)5000/1024;
   delay(delayFactor);
   
   /*
   Every once in a while,MCU read the temperature from the DS18B20 and then let the DS18B20 start the convert.
   Attention:The interval between start the convert and read the temperature should be greater than 750 millisecond,or the temperature is not accurate!
   */
   temperature = TempProcess(ReadTemperature);  // read the current temperature from the  DS18B20
   TempProcess(StartConvert);                   //after the reading,start the convert for next reading
   Serial.print("EC Voltage: ");
   Serial.print(ECVoltage);  //millivolt average,from 0mv to 4995mV
   Serial.print(" mV    ");
   Serial.print("temp: ");
   Serial.print(temperature);    //current temperature
   Serial.print(" ^C     EC: ");
   
   float TempCoefficient=1.0+0.0185*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.0185*(fTP-25.0));
   float CoefficientVolatge=(float)ECVoltage/TempCoefficient;   
   if(CoefficientVolatge<150)Serial.println("No solution!");   //25^C 1413us/cm<-->about 216mv  if the voltage(compensate)<150,that is <1ms/cm,out of the range
   else if(CoefficientVolatge>3300)Serial.println("Out of the range!");  //>20ms/cm,out of the range
   else
   { 
     if(CoefficientVolatge<=448)ECcurrent=6.84*CoefficientVolatge-64.32;   //1ms/cm<EC<=3ms/cm
     else if(CoefficientVolatge<=1457)ECcurrent=6.98*CoefficientVolatge-127;  //3ms/cm<EC<=10ms/cm
     else ECcurrent=5.3*CoefficientVolatge+2278;                           //10ms/cm<EC<20ms/cm
     ECcurrent/=1000;    //convert us/cm to ms/cm
     Serial.print(ECcurrent,2);  //two decimal
     Serial.println(" ms/cm");
   }
   delay(delayFactor);
}

/*
ch=0,let the DS18B20 start the convert;ch=1,MCU read the current temperature from the DS18B20.
 */
float TempProcess(bool ch)
{
  //returns the temperature from one DS18B20 in DEG Celsius
  static byte data[12];
  static byte addr[8];
  static float TemperatureSum;
  if(!ch){
    if ( !ds.search(addr)) {
      Serial.println("no more sensors on chain, reset search!");
      ds.reset_search();
      return 0;
    }      
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 0;
    }        
    if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized!");
      return 0;
    }      
    ds.reset();
    ds.select(addr);
    ds.write(0x44,1); // start conversion, with parasite power on at the end
  }
  else{  
    byte present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE); // Read Scratchpad            
    for (int i = 0; i < 9; i++) { // we need 9 bytes
      data[i] = ds.read();
    }         
    ds.reset_search();           
    byte MSB = data[1];
    byte LSB = data[0];        
    float tempRead = ((MSB << 8) | LSB); //using two's compliment
    TemperatureSum = tempRead / 16;
  }
  return TemperatureSum;  
}

