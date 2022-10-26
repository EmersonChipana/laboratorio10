#include <WiFi.h>  
#include "config.h"
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <stdlib.h>
/*
 * Sensor de Luz
 */
 #define ADC_VREF_mV    3300.0 // 3.3v en millivoltios
#define ADC_RESOLUTION 4096.0
#define LIGHT_SENSOR_PIN    33
int datoADC;
float porcentaje=0.0;
float percent=0.0;
float factor=100.0/ADC_RESOLUTION;

boolean var1;

/*
Sensor LMR35
*/
#define ADC_VREF_mV    3300.0 // 3.3v en millivoltios
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       36 // ESP32 pin GIOP36 (ADC0) conectado al LM35
#define factor 0.0805860805860

int datoVal;
float milliVolt,tempC,tempF;

/*
 * Sensor de presencia
 */
 const int PIN_TO_SENSOR = 14; // GIOP19 pin conectado al sensor
int pinStateCurrent   = LOW;  // estado actual
int pinStatePrevious  = LOW;  // estado previo

 #define COUNT_LOW 1638
 #define COUNT_HIGH 7864
 #define TIMER_WIDTH 16
 int i;

 /*
  * PWMVALUE
  */
#define PWM1_Ch    0    //canales
#define PWM1_Res   8    //resolucion    
#define PWM1_Freq  1000  //frecuencia
#define PWM2_Ch    1
#define PWM2_Res   8
#define PWM2_Freq  1000
#define PWM3_Ch    2
#define PWM3_Res   8
#define PWM3_Freq  1000
int pwm1,pwm2,pwm3;
int PWM1_DutyCycle = 0;


 int ahora1 = 1638;

int ahora2 =1638;
String pwmValue;
 

int rele = 18;
int rele2 =23
int ledRojo = 22;
int ledVerde = 21;
int ledAzul = 19;

int color=1;


AsyncWebServer server(80);


                 //Definimos el puerto de comunicaciones
// Variable to store the HTTP request
String header;
 

JSONVar info;

JSONVar luz;





void initWiFi() {
// conectamos al Wi-Fi
  WiFi.begin(ssid, password);
  // Mientras no se conecte, mantenemos un bucle con reintentos sucesivos
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      // Esperamos un segundo
      Serial.println("Conectando a la red WiFi..");
    }
  Serial.println();
  Serial.println(WiFi.SSID());
  Serial.print("Direccion IP:\t");
  // Imprimimos la ip que le ha dado nuestro router
  Serial.println(WiFi.localIP());
}

void elegirColor(int color){
  if(color == 1) { //Hacer color rojo
digitalWrite(ledRojo,255);
digitalWrite(ledVerde,0);
digitalWrite(ledAzul,0);
  }else if (color==2){
 
//Hacer color verde
digitalWrite(ledRojo,0);
digitalWrite(ledVerde,255);
digitalWrite(ledAzul,0);
  }else if (color ==3){
 
//Hacer color azul
digitalWrite(ledRojo,0);
digitalWrite(ledVerde,0);
digitalWrite(ledAzul,255);
  }else if (color==4){
//Hacer color amarillo
digitalWrite(ledRojo,255);
digitalWrite(ledVerde,255);
digitalWrite(ledAzul,0);
  
 }else if (color==5){
//Hacer color magenta
digitalWrite(ledRojo,255);
digitalWrite(ledVerde,0);
digitalWrite(ledAzul,255);
 }
}

static int funcluz(long datoADC){
 int value=0;
 if (datoADC == 0) {
  value=0;
 }else if (datoADC > 0 && datoADC<51) {
    value=1;
  } else if (datoADC >= 51 && datoADC<102) {
    value=2;
  } else if (datoADC>=102 && datoADC<153) {
    value=3;
  } else if (datoADC>=153 && datoADC<204) {  
    value=4;
  } else {
    value=5;
  }
  return value;
}
 String getinfo(){

  info["dip"] = String(WiFi.localIP());
  info["status"] =  String(WiFi.status());
  info["ssid"] =  String(WiFi.SSID());
  info["rsid"] =  WiFi.RSSI();
  String jsonString = JSON.stringify(info);
  return jsonString;
}

 String gettemp(){
// Lectura de los datos del sensor
  temp["datoVal"]   = String(analogRead(PIN_LM35));
   // Convirtiendo los datos del ADC a 
   milivoltios
  temp["mil"] =  String(datoVal * (ADC_VREF_mV / ADC_RESOLUTION));
  // Convirtiendo el voltaje al temperatura en °C
  temp["tempC"] =  datoVal * factor ; 
  String jsonString = JSON.stringify(temp);
  return jsonString;
}


void setup() {


  Serial.begin(115200); // inicializando el pouerto serial

  ledcAttachPin(rele, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);


  pinMode(PIN_TO_SENSOR, INPUT); // Configurando el pin como entrada//sensor de mov
  
  pinMode(rele,OUTPUT);
  pinMode(rele2,OUTPUT);

  pinMode(ledRojo,OUTPUT);
pinMode(ledVerde,OUTPUT);
pinMode(ledAzul,OUTPUT);

//  DS18B20.begin();      // initializando el sensor DS18B20
  initWiFi();

 // digitalWrite(PinLedR, LOW); digitalWrite(PinLedG, LOW); digitalWrite(PinLedB, LOW);  
  if(!SPIFFS.begin())
     { Serial.println("ha ocurrido un error al montar SPIFFS");
       return; }
  
  //Serial.println(WiFi.localIP());
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html",String(), false);
  });    
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/style.css", "text/css");
            });      
            /*  
server.on("/ADC", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getSensorReadings();
    request->send(200, "application/json", json);
    json = String();
  });
  */
  
 /*
  * 
  */
 server.on("/LUZ", HTTP_POST, [](AsyncWebServerRequest *request){
             pwmValue = request->arg("bombilla");
            Serial.print("PWM:\t");
            Serial.println(pwmValue);
            ledcWrite(PWM1_Ch, pwmValue.toInt());
            
            request->redirect("/");
            });
   
  server.on("/MODO", HTTP_POST, [](AsyncWebServerRequest *request){
     
            AsyncWebParameter* p = request->getParam(0);
            //int color= p->value().toInt();
            Serial.println("color");
            Serial.println(p->value().c_str());
           // Serial.println(color);
       //  elegirColor(color) ;  
             
               
            });  

  server.on("/MODO1", HTTP_GET, [](AsyncWebServerRequest *request){
    color =1;
    elegirColor(color) ;
    
    
  });
  server.on("/MODO2", HTTP_GET, [](AsyncWebServerRequest *request){
    color =2;
    elegirColor(color) ;  
   
  });
  server.on("/MODO3", HTTP_GET, [](AsyncWebServerRequest *request){
    color =3;
    elegirColor(color) ; 
    
  });
  server.on("/MODO4", HTTP_GET, [](AsyncWebServerRequest *request){
     color =4;
    elegirColor(color) ;  
    
  });
  server.on("/MODO5", HTTP_GET, [](AsyncWebServerRequest *request){
     color =5;
    elegirColor(color) ;  
    
  });
   server.on("/INFO", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getinfo();
    request->send(200, "application/json", json);
    json = String();
  });

server.on("/ON", HTTP_GET, [](AsyncWebServerRequest *request){
             ledcWrite(rele, HIGH); 
              
             //String json = getserv();
             Serial.print("Encendido");
           request->send(0);
   // json = String();
            });
server.on("/OFF", HTTP_GET, [](AsyncWebServerRequest *request){
             ledcWrite(rele, OFF); 

             //String json = getserv();
            Serial.print("Apagado");
           request->send(0);
   // json = String();
            });
//Ventilador 
server.on("/VON", HTTP_GET, [](AsyncWebServerRequest *request){
             ledcWrite(rele2, HIGH); 
              
             //String json = getserv();
             Serial.print("Encendido");
           request->send(0);
   // json = String();
            });
server.on("/OFF", HTTP_GET, [](AsyncWebServerRequest *request){
             ledcWrite(rele2, OFF); 

             //String json = getserv();
            Serial.print("Apagado");
           request->send(0);
   // json = String();
            });

server.on("/SLIDER", HTTP_POST, [](AsyncWebServerRequest *request){
            pwmValue = request->arg("bomb");
            Serial.print("PWM:\t");
            Serial.println(pwmValue);
            ledcWrite(PWM1_Ch, pwmValue.toInt()); 
            request->redirect("/");
                  
            });  

server.on("/per", HTTP_POST, [](AsyncWebServerRequest *request){
            percent= (request->arg("p")).toInt();        
            request->redirect("/");
            }); 

server.on("/LUZPWD", HTTP_POST, [](AsyncWebServerRequest *request){
  pwmValue = request->arg("pwd");
            Serial.print("PWM:\t");
            Serial.println(pwmValue);
            ledcWrite(PWM1_Ch, pwmValue.toInt()); 
            request->redirect("/");
  }); 
server.on("/valorluz",HTTP_GET, [](AsyncWebServerRequest *request){
  JSONVar val;
  long valor1=pwmValue.toInt();
  int value=funcluz(valor1);
  val["status"] = String(value);
  request->send(200, "application/json", JSON.stringify(val));
 val=String();
                  
            });  

server.on("/LUZSensor", HTTP_GET, [](AsyncWebServerRequest *request){
         JSONVar val;
     int value=0;
            digitalWrite(rele,HIGH);
            datoADC = analogRead(LIGHT_SENSOR_PIN);
              //porcentaje=factor*datoADC;
              Serial.print("Valor Analogico = ");
              Serial.print(datoADC);   
              Serial.print("  Porcentaje = ");
              Serial.print(porcentaje);  
              if (datoADC < 40) {
                ledcWrite(PWM1_Ch, 50); 
                Serial.println("%  => Oscuro");
                value=1;
              } else if (datoADC < 800) {
                ledcWrite(PWM1_Ch, 100); 
                Serial.println("% => Tenue");
                value=2;
              } else if (datoADC < 2000) {
                ledcWrite(PWM1_Ch, 150); 
                Serial.println("% => Claro");
                value=3;
              } else if (datoADC < 3200) {
                ledcWrite(PWM1_Ch, 200); 
                Serial.println("% => Luminoso");
                value=4;
              } else {
                ledcWrite(PWM1_Ch, 255); 
                Serial.println("% => Muy Luminoso");
                value=5;
              }
            Serial.print("Apagado");
              val["status"] = String(value);
           request->send(200, "application/json", JSON.stringify(val));
   // json = String();
            });
        
  
  server.begin();

  
  
}  
void loop() {
   datoADC = analogRead(LIGHT_SENSOR_PIN);
   if(color==3){
  // lectura del dato analogico (valor entre 0 y 4095)
 
  porcentaje=factor*datoADC;
  Serial.print("Valor Analogico = ");
  Serial.print(datoADC);   
  Serial.print("  Porcentaje = ");
  Serial.print(porcentaje);  
  if (datoADC < 40) {
      ledcWrite(PWM1_Ch, 255); 
      pwmValue = "255";
      Serial.println("%  => Oscuro");
    } else if (datoADC < 800) {
      ledcWrite(PWM1_Ch, 200); 
      Serial.println("% => Tenue");
      pwmValue = "200";
    } else if (datoADC < 2000) {
      ledcWrite(PWM1_Ch, 150); 
      Serial.println("% => Claro");
      pwmValue = "150";
    } else if (datoADC < 3200) {
      ledcWrite(PWM1_Ch, 100); 
      Serial.println("% => Luminoso");
      pwmValue = "100";
    } else if (datoADC < 3800) {
      ledcWrite(PWM1_Ch, 50); 
      Serial.println("% => Muy Luminoso");
      pwmValue = "50";
    } else{
      ledcWrite(PWM1_Ch, 0); 
      pwmValue = "0";
    }
  delay(500);
  }

  if(color==5){
     porcentaje=factor*datoADC;
     if(porcentaje<percent){
      ledcWrite(PWM1_Ch, 0); 
      pwmValue ="0";
     }else{
      ledcWrite(PWM1_Ch, 255); 
      pwmValue ="255";
     }
  }
}
