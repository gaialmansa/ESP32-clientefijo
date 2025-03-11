#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <HTTPClient.h>
//#include <NTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include <esp_pm.h>
#include "driver/gpio.h"

#include <local.h>


#define LED_ROJO 22           
#define LED_VERDE 23
#define BOTON 15

void WiFiStart();
void IRAM_ATTR pushISR();
void enviarMensaje();
String Api(char metodo[], String parametros[],int);
String replaceSpaces(String str);


// Variables para manejar el debounce
volatile unsigned long lastDebounceTime[3] = { 0, 0, 0};
const unsigned long debounceDelay = 200;  // Ajusta este valor según necesites
HTTPClient apicall;             // cliente HTTP para llamar al API   
bool enviar = false;            // flag para enviar mensajes inicializado a falso
bool leido = false;             // flag para detectar que el mensaje ha sido leido
DynamicJsonDocument doc(1024);  // calculamos un k para la respuesta del api

