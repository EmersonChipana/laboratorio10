#include <WiFi.h>  
#include "config.h"
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <stdlib.h>

boolean mod = true;
String estado="0";
int valor;

/*
Sensor LMR35
*/
#define ADC_VREF_mV    3300.0 // 3.3v en millivoltios
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       33 // ESP32 pin GIOP36 (ADC0) conectado al LM35
#define factor 0.0805860805860

int datoVal;
float milliVolt,tempC,tempF;


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
int rele2 =23;
int rele3= 14;
int ledRojo = 22;
int ledVerde = 21;
int ledAzul = 19;

int color=1;


AsyncWebServer server(80);


                 //Definimos el puerto de comunicaciones
// Variable to store the HTTP request
String header;
 

JSONVar info;
JSONVar temp;
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
  datoVal = analogRead(PIN_LM35);
  temp["datoVal"]   = String(datoVal);
   // Convirtiendo los datos del ADC a    milivoltios
  temp["mil"] =  String(datoVal * (ADC_VREF_mV / ADC_RESOLUTION));
  // Convirtiendo el voltaje al temperatura en °C
  temp["tempC"] =  datoVal * factor ; 
  temp["estado"]=estado;
  String jsonString = JSON.stringify(temp);
  return jsonString;
}


void setup() {


  Serial.begin(115200); // inicializando el pouerto serial


  
  pinMode(rele,OUTPUT);
  pinMode(rele2,OUTPUT);
  pinMode(rele3,OUTPUT);

  pinMode(33,INPUT);

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
  server.on("/CONTROL", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/control.html",String(), false);
  });   
  server.on("/HORARIO", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/horario.html",String(), false);
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

   server.on("/INFO", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getinfo();
    request->send(200, "application/json", json);
    json = String();
  });


  server.on("/TEMP", HTTP_GET, [](AsyncWebServerRequest *request){
    datoVal=analogRead(33);
    String json = gettemp();
    request->send(200, "application/json", json);
    json = String();
  });
  


server.on("/ON", HTTP_GET, [](AsyncWebServerRequest *request){
             digitalWrite(rele, HIGH); 
              digitalWrite(ledRojo,255);
digitalWrite(ledVerde,0);
digitalWrite(ledAzul,0);
             //String json = getserv();
             Serial.print("Encendido");
           request->send(0);
   // json = String();
            });
server.on("/OFF", HTTP_GET, [](AsyncWebServerRequest *request){
             digitalWrite(rele, LOW); 
             digitalWrite(ledRojo,0);
digitalWrite(ledVerde,255);
digitalWrite(ledAzul,0);

             //String json = getserv();
            Serial.print("Apagado");
           request->send(0);
   // json = String();
            });
//Ventilador 
server.on("/VON", HTTP_GET, [](AsyncWebServerRequest *request){
             digitalWrite(rele2, HIGH); 
             digitalWrite(rele3, HIGH);
             //String json = getserv();
             Serial.print("Encendido");
             digitalWrite(ledRojo,0);
digitalWrite(ledVerde,0);
digitalWrite(ledAzul,255);
           request->send(0);
   // json = String();
            });
server.on("/VOFF", HTTP_GET, [](AsyncWebServerRequest *request){
             digitalWrite(rele2, LOW);
            digitalWrite(rele3, LOW); 

             //String json = getserv();
             digitalWrite(ledRojo,255);
digitalWrite(ledVerde,0);
digitalWrite(ledAzul,255);
            Serial.print("Apagado");
           request->send(0);
   // json = String();
            });

server.on("/SET_POINT", HTTP_POST, [](AsyncWebServerRequest *request){
            pwmValue = request->arg("set_point");
            valor=pwmValue.toInt();
            Spoint(valor);
            Serial.println(pwmValue);
            ledcWrite(PWM1_Ch, pwmValue.toInt()); 
            request->redirect("/control.html");
                  
            });  
server.on("/TRUE", HTTP_GET, [](AsyncWebServerRequest *request){ 
 mod=true;
request->send(0);
   // json = String();
            });
server.on("/FALSE", HTTP_GET, [](AsyncWebServerRequest *request){ 
mod=false;
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

        
  
  server.begin();

  
  
}  
void loop() {
  datoVal =analogRead(33);
  float datoC=datoVal*factor;
if(mod==true){
  if (datoC>(valor*1.05)){
    digitalWrite(rele, LOW);    
    digitalWrite(rele2, HIGH);
    digitalWrite(rele3, HIGH);
    estado="1";
  }else if (datoC<(valor*0.95)){
    digitalWrite(rele, HIGH);    
    digitalWrite(rele2,LOW);
    digitalWrite(rele3,LOW);
    estado="2";
  }
 
}
  
}
