#include <beeper.h>


String mac;

void setup() 
{
  
  #ifdef _DEBUG
  Serial.begin(9600);
  delay(300);
  Serial.println("Inicializando. Arrancando WiFi");
  #endif
  WiFiStart();               // inicializa wifi
  pinMode(BUILTIN, OUTPUT);  // pines de salida
  //pinMode(BUZZER, OUTPUT);
  ledcSetup(0, 1000, 8);
  ledcAttachPin(BUZZER, 0);
  pinMode(VIB, OUTPUT);
  pinMode(LEDOUT, OUTPUT);
  digitalWrite(BUILTIN,LOW); // de entrada encendemos el pin hasta que conecte
  mac = WiFi.macAddress();   // obtenemos la mac
  equipo = registro();       // y el equipo al que esta asignado el dispositivo
  
}
void loop() 
{
  if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Error: Conexión WiFi perdida.");
      WiFiStart();
    }
  
  if (haymsg())               // hay algun mensaje pendiente. Encendemos el led y los trastos
    {
      tone(BUZZER,350);
      digitalWrite(LEDOUT,HIGH);
      digitalWrite(VIB,HIGH);
      delay(100);
      noTone(BUZZER);
      digitalWrite(VIB,LOW);
      delay(900);
    }
    else
      digitalWrite(LEDOUT,LOW);
    delay(1000);
}
void WiFiStart() // Inicializa la conexion
{
    String mensaje;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    delay(200);
    mensaje = "Intentando conectar a "+String(ssid);  // los parametros de la conexion se leen en local.h
    delay(5000);
    #ifdef _DEBUG
    Serial.println(mensaje);
    #endif
    
    while (WiFi.status() != WL_CONNECTED) 
    {
     delay(500);
     #ifdef _DEBUG
    Serial.print(".");
    #endif
     }
    mensaje = "Conectado a "+String(ssid);

    #ifdef _DEBUG
    Serial.println(mensaje);
    #endif
}
int registro()   // Busca en BD el dispositivo por su mac, y devuelve el equipo al que está asignado
{
  char metodo[] = "esp32init";  // metodo para la api
  String url, payload;
  int respuesta;

  url = _URL;
  url += metodo;              // con esto queda formada la url del API con su metodo al final
  #ifdef _DEBUG
   Serial.print("Comunicando con: ");
   Serial.println(url);
   Serial.println(mac);
  #endif
  //delay(10000);
  apicall.begin(url);    // iniciamos la llamada al api
                         // Especificamos el header content-type
  apicall.addHeader("Content-Type", "application/x-www-form-urlencoded");
  respuesta = apicall.POST("mac="+String(mac));  
  #ifdef _DEBUG
  if(respuesta != 200)
    {
      Serial.printf("Ha habido un error %d al comunicar con el api.\n",respuesta);
      Serial.println(url);
      Serial.println(mac);
    }
  #endif
  payload = apicall.getString();

  #ifdef _DEBUG
    Serial.print("Respuesta de la API:");
    Serial.println(payload);
  #endif
  return payload.toInt();
}
bool haymsg()    // Devuelve true cuando hay mensajes pendientes.
{
  char metodo[] = "haymsg";  // metodo para la api
  String url, payload;
  int respuesta;

  url = _URL;
  url += metodo;              // con esto queda formada la url del API con su metodo al final
  #ifdef _DEBUG
   Serial.print("Comunicando con: ");
   Serial.println(url);
  #endif
  //delay(10000);
  apicall.begin(url);    // iniciamos la llamada al api
                         // Especificamos el header content-type
  apicall.addHeader("Content-Type", "application/x-www-form-urlencoded");
  respuesta = apicall.POST("equipo="+String(equipo)); 
  
  #ifdef _DEBUG 
    Serial.print("equipo = ");
    Serial.println(equipo);
    Serial.println(respuesta); 
 
  if(respuesta != 200)
    {
      Serial.printf("Ha habido un error %d al comunicar con el api.\n",respuesta);
      Serial.println(url);
      Serial.println(mac);
    }
  #endif
  payload = apicall.getString();

  #ifdef _DEBUG
    Serial.print("Respuesta de la API:");
    Serial.println(payload);
  #endif
  return bool (payload == "1") ;
}