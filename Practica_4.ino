#include <Wire.h>
#include <SPI.h>
#include "Adafruit_VEML6070.h"
#include <MechaQMC5883.h>
#include <Adafruit_GFX.h>    // Librería graficos 
#include <Adafruit_ST7735.h> // Librería Hardware

//SoftwareSerial loraSerial(7, 6); // RX, TX
Adafruit_VEML6070 uv = Adafruit_VEML6070();
MechaQMC5883 qmc;

#define TFT_CS     10
#define TFT_RST    8  // Reset
#define TFT_DC     9
#define TFT_SCLK 13   // SCLK
#define TFT_MOSI 11   // MOST

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Comandos utilizados

//Colores
#define WHITE 0xFFFF
#define BLACK 0x0000
#define red   0xf800
#define GREEN 0x07e0
#define BLUE 0x001f
#define YELLOW 0xffe0
#define CYAN 0x07FF
#define ORANGE 0xFCA0
#define BROWN 0x8200
#define PURPLE 0x780F

#define marca 126
#define VEML6070_ADDR_L     (0x38) ///< Low address
#define MQ135     A0                        //Pin MQ135
#define co2Zero     55                        //calibrated CO2 0 level


int proceso[9];
int co2ppm = 0;
int luzUV =0;
String MensajeFinal;
unsigned long tiempo=0;
int periodo=0;
int angulo=0;
int t=0;
int coordX=15; //Son los puntos 0 de la grafica, X Aumenta con el tiempo
int coordY=110;//Son los puntos 0 de la grafica, Y Obtiene el valor de la variable



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

Sensores aire;
Sensores UV;
Sensores brujula;
//****************************************************Fin De la Structura****************************************

//**********************************************Proceso de recepcion de datos************************************
void recepcion()
{
  String cadena;
  String cad1 ="";
  int posicion_inicial = 0;
  int posicion_final= 0;
  int cont=0;

  if (Serial.available())
   { if(Serial.find(marca)) // si enviamos desde la pc
    {
      /*   !!!!!!DESCOMENTAR PARA EL VIDEO
       *    
        int tam = Serial.read();
        tam -= 32 ; 
        char datos[tam+1];
        Serial.readBytes(datos,tam);
        for(int i=0 ; i<tam; i++)
        {  cadena += String(datos[i]);}

       */
       char datos[58];
       
        Serial.readBytes(datos,58);
        for(int i=0 ; i<strlen(datos); i++)
        {  cadena += String(datos[i]);}
        
        Serial.println(cadena);
        
       
    }
   } 
    
        
   do{
    
    posicion_final = cadena.indexOf("/",posicion_inicial);
    cad1 = cadena.substring(posicion_inicial,posicion_final);
    proceso[cont] = cad1.toInt();
   
    posicion_inicial = posicion_final + 1;
    cont++;
    
    
     } while(posicion_final > 0);

    Peticion();
  }
  
void Peticion()
{
  
  switch(proceso[1])
  {
    case 20://Peticion de datos por muestreo
    datos();
    break;

    case 26://Peticion de datos por muestreo
    periodo=int(proceso[2]);
    Serial.println("tiempo de muestreo= "+String(periodo));
    Serial.println(String(periodo));
    break;
    
    
    case 25://Descubrimiento de la red
    String MensajeDescubrimiento="Unidad1/"+aire.nombre+"/"+UV.nombre;
    Serial.println(MensajeDescubrimiento);
    break;
    
    
    
  }
}


//****************************************************Funcion de los sensores************************************
void Angulo()
{
  int x,y,z;   // variables para los 3 ejes y acimut
  qmc.read(&x,&y,&z,&angulo); // funcion para leer valores y asignar a variables
  
  
}
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

void datos()
{
    SensorVML();
    SensorAire();
    Angulo();
    //Serial.println("");
    aire.control(co2ppm);
    UV.control(luzUV);
    brujula.control(angulo);
    MensajeFinal=aire.serial+UV.serial+brujula.serial;
    Serial.println(MensajeFinal);
    
    //TFT
    if (coordX<150)
     {
       coordX=coordX+1;
       valores_tft((angulo/5),red,coordX,coordY-(angulo/5));
       valores_tft((co2ppm/5),BLUE,coordX,coordY-(co2ppm/5));
       valores_tft((luzUV/5),PURPLE,coordX,coordY-(luzUV/5));
     }
     else
     {
      iniciar_tft();
      Grafica_tft();
      coordX=10;
     }
    
}
//****************************************************Setup***************************************************************




void setup() {
 Serial.begin(9600);
  Wire.begin();     // inicializa bus I2C
  uv.begin(VEML6070_1_T);
  pinMode(MQ135,INPUT);                     //MQ135 analog feed set for input
  qmc.init();
  iniciar_tft();
  
  
  //aire.begin(String NOMBRE , int LI,int LS ,int VALORMIN, int VALORMAX, bool Inverso
 aire.begin("CalidadAire",0,0,0,0,false);
 UV.begin("LuzUltraVioleta",0,0,0,0,false);
 brujula.begin("AnguloMagnetico",0,0,0,0,false); 
 
 Serial.println("Esperando Señal maestra");
 datos_tft("Esperando la señal De la unidad maestra");
 datos_tft("Direccion= 1");
 int x=0;
 while(x==0)
 {
    recepcion();
    if(proceso[1]==25)
    {
      datos_tft("Comando recibido del maestro");
      delay(2000);
      x=1;
    }
 }
 
 iniciar_tft();
 delay(500);
 Grafica_tft();
}
  

void loop() {

  if(Serial.available())
  {
    recepcion();
  }
  if(periodo>0)
  {
    if(millis()-tiempo>=periodo)
    {
      datos();
      tiempo=millis();
    }
  }

}


//*************************************Funciones del TFT******************
int x=10;
void iniciar_tft()// Borra todo de la pantalla
{
   tft.initR(INITR_BLACKTAB);
  tft.fillScreen(BLACK); /*   0,   0,   0 */
  tft.setTextSize(1);
  tft.setRotation(1);
}

void datos_tft(String dato)
{ 
  
    tft.setCursor(20,x); //x,y
    tft.print(dato);
    x=x+15;
  
}

void Grafica_tft()
{
  //Linea roja horizontal
  tft.setRotation(1);
  int y1=110;
  tft.setTextColor(WHITE);
  for( int x1 = 10;x1< 150;x1++)
  {
    tft.setCursor(x1,y1);
    tft.print(".");
  }
  tft.setCursor(5,120);
  tft.print("C=17"); 
  tft.setCursor(55,120);
  tft.print("Tiempo(s)");
 

  //Linea roja Vertical
   int x2=15;
  tft.setTextColor(WHITE);
  for( int y2 = 10;y2< 111;y2++)
  {
    tft.setCursor(x2,y2);
    tft.print(".");
  } 

  //Numeros Verticales
  int EscalaY=10;
  int valory=500;
  while(EscalaY<111)
   { 
  
  tft.setCursor(0,EscalaY);
  tft.print(String(valory));
  EscalaY=EscalaY+10;
  valory=valory-50;
  }

  //Indicacion de colores
  cuadro_de_color(25,0,red,"Angulo");
  cuadro_de_color(70,0,BLUE,"CO2");
  cuadro_de_color(100,0,PURPLE,"UV");
  cuadro_de_color(130,0,BLACK,"D=1");
}

void cuadro_de_color(int x,int y,int color,String texto)
{
  tft.setTextColor(color);
  for(int i=x;i<x+4;i++)
  {
    for(int j=y;j<y+4;j++)
    {
      
     tft.setCursor(i,j);
     tft.print("."); 
    }
  }

  tft.setTextColor(WHITE);
  tft.setCursor(x+8,y+4);
  tft.print(texto);
  
}
void valores_tft(int valor,int color,int posicionX,int posicionY)
{
  tft.setTextColor(color);
  tft.setCursor(posicionX,posicionY);
  tft.print(".");

  
  
}
