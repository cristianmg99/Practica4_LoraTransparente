#include <Wire.h>
#include <SPI.h>
#include "Adafruit_VEML6070.h"
#include <SoftwareSerial.h>

//SoftwareSerial loraSerial(7, 6); // RX, TX
Adafruit_VEML6070 uv = Adafruit_VEML6070();

#define marca 126
#define VEML6070_ADDR_L     (0x38) ///< Low address
#define MQ135     A0                        //Pin MQ135
#define co2Zero     55                        //calibrated CO2 0 level
int proceso[9];
int co2ppm = 0;
int luzUV =0;
String MensajeFinal;

//*****************************************************Structura******************************************
struct Sensores{
  String nombre;
  float valor;
  int valormin;
  int valormax;
  int ls ;
  int li;
  String serial;
  bool inverso;
  char cadena[50];
  
void begin (String NOMBRE , int LI,int LS ,int VALORMIN, int VALORMAX, bool Inverso)
{
nombre = NOMBRE;
ls= LS;
li=LI;
valormax= VALORMAX;
valormin= VALORMIN;
inverso = Inverso;

} 

void control(float Valor)
{
 valor = Valor ; 
 mensaje();
}

void mensaje()
{ String Mensaje = "";
  Mensaje += nombre;
  Mensaje +="/";
  Mensaje += valor;
  Mensaje +="/";

  serial = Mensaje;
}


};

//****************************************************Fin De la Structura****************************************

//**********************************************Proceso de recepcion de datos************************************
void recepcion(String CADENA)
{
  String cadena;
  String cad1 ="";
  int posicion_inicial = 0;
  int posicion_final= 0;
  int cont=0;

  if (Serial.available())
   { if(Serial.read()== marca ) // si enviamos desde la pc
  {
   int tam = Serial.read();
    tam -= 32 ; 
    char datos[tam+1];
   
    Serial.readBytes(datos,tam);
    for(int i=0 ; i<tam; i++)
    {  cadena += String(datos[i]);}
    
    }
  } 
  else{cadena = CADENA ;}   
        
   do{
    
    posicion_final = cadena.indexOf("/",posicion_inicial);
    cad1 = cadena.substring(posicion_inicial,posicion_final);
    proceso[cont] = cad1.toInt();
   
    posicion_inicial = posicion_final + 1;
    cont++;
    
    
     } while(posicion_final > 0);
  }
  



//****************************************************Funcion de los sensores************************************

void SensorVML()
{
//SCL A5
//SDA A4
//Serial.print("Luz ultravioleta= ");
luzUV=uv.readUV();
//Serial.println(luzUV); 
}

void SensorAire()
{
 
  int co2now[10];                               //int array for co2 readings
  int co2raw = 0;                               //int for raw value of co2
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

 /* 
  Serial.print("Calidad del aire= ");
  Serial.print(co2ppm);  // prints the value read
  Serial.println(" PPM");
  */
 
  
}
//****************************************************Setup***************************************************************

Sensores aire;
Sensores UV;


void setup() {
 Serial.begin(9600);
  //loraSerial.begin(9600);
  uv.begin(VEML6070_1_T);
  pinMode(MQ135,INPUT);                     //MQ135 analog feed set for input
  //aire.begin(String NOMBRE , int LI,int LS ,int VALORMIN, int VALORMAX, bool Inverso
 aire.begin("CalidadAire",0,0,0,0,false);
 UV.begin("LuzUltraVioleta",0,0,0,0,false);

}
  

void loop() {
  SensorVML();
  SensorAire();
  Serial.println("");
  aire.control(co2ppm);
  UV.control(luzUV);
  MensajeFinal=aire.serial+UV.serial;
  Serial.println(MensajeFinal);
  delay(2000);

}
