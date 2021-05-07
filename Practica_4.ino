#include <Wire.h>
#include <SPI.h>
#include "Adafruit_VEML6070.h"
#include <SoftwareSerial.h>

SoftwareSerial loraSerial(7, 6); // RX, TX
Adafruit_VEML6070 uv = Adafruit_VEML6070();

#define VEML6070_ADDR_L     (0x38) ///< Low address
#define MQ135     A0                        //Pin MQ135
#define co2Zero     55                        //calibrated CO2 0 level




void SensorVML()
{
//SCL A5
//SDA A4
Serial.print("Luz ultravioleta= ");
Serial.println(uv.readUV()); 
}

void SensorAire()
{
 
  int co2now[10];                               //int array for co2 readings
  int co2raw = 0;                               //int for raw value of co2
  int co2ppm = 0;                               //int for calculated ppm
  int zzz = 0;                                  //int for averaging


  for (int x = 0;x<10;x++)  //10 Muestas de oxigeno
  {                   
    co2now[x]=analogRead(A0);
    delay(10);
  }

  for (int x = 0;x<10;x++)  //Los sumamos todos
  {                     
    zzz=zzz + co2now[x];  
  }
  
  co2raw = zzz/10;                            //los dividimos sobre 10
  co2ppm = co2raw - co2Zero;                 //al promedio le restamos el valor de calibracion

  Serial.print("Calidad del aire= ");
  Serial.print(co2ppm);  // prints the value read
  Serial.println(" PPM");
 
  
}

void setup() {
 Serial.begin(9600);
  loraSerial.begin(9600);
  uv.begin(VEML6070_1_T);
  pinMode(MQ135,INPUT);                     //MQ135 analog feed set for input

}

void loop() {
  SensorVML();
  SensorAire();
  Serial.println("");
  delay(2000);

}
