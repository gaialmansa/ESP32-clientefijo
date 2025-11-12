#include <Arduino.h>
#include <HTTPClient.h>
//#include <ArduinoJson.h>

#include <local.h>



#define BUILTIN 8
#define BUZZER  2
#define VIB     0
#define LEDOUT  1
int equipo;
void WiFiStart();
int registro();
String Api(char metodo[], String parametros[],int);
String replaceSpaces(String str);
bool haymsg();

HTTPClient apicall;             // cliente HTTP para llamar al API   

