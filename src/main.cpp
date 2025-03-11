#include <beeper.h>



void setup() 
{
  #ifdef _DEBUG
  Serial.begin(9600);
  delay(300);
  Serial.println("Inicializando. Arrancando WiFi");
  #endif
  
  WiFiStart();  // inicializa wifi
  pinMode(LED_ROJO, OUTPUT);  // pines de salida
  pinMode(LED_VERDE,OUTPUT);
  pinMode(BOTON, INPUT_PULLUP); // pin de entrada
  //attachInterrupt(BOTON, pushISR, FALLING);    // pulsador
  digitalWrite(LED_VERDE,HIGH);     // de entrada ponemos el verde
  //enviarMensaje();
  Serial.println("Activando interrupcion. Esperando pulsacion.");
  attachInterrupt(BOTON, pushISR, FALLING);    // pulsador
}

void loop() 
{
  if (enviar)
    enviarMensaje();
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
void enviarMensaje()
{
  digitalWrite(LED_VERDE,LOW); //apagamos el verde
  digitalWrite(LED_ROJO,HIGH); //encendemos el rojo
  String postData = replaceSpaces("id_usuario_o=2&id_grupo=21&mensaje=Hay un paciente en triaje.");
  #ifdef _DEBUG
   Serial.println("Enviando mensaje.");
  #endif
  String id_mensaje, parms[1];
  parms[0] = postData;
  Api("mcrearg",parms,1);
  Serial.println("Enviado");
  id_mensaje =  doc["id_mensaje"].as<String>();
  // repetir este bucle hasta que alguien lea el mensaje
  String response="null";

  while(response == "null") 
  {
    parms[0] = "id_mensaje="+id_mensaje;
    response = Api("mv",parms,1);  // llamada que devuelve la cadena "null" hasta que el mensaje haya sido leido
    Serial.println(response);
    delay(100);
  }
  enviar = false;
  digitalWrite(LED_VERDE,HIGH); //apagamos el verde
  digitalWrite(LED_ROJO,LOW); //encendemos el rojo
  attachInterrupt(BOTON, pushISR, FALLING);    // pulsador habilitado de nuevo
}
String replaceSpaces(String str) //Reemplaza los espacios en str por %20
{
  String result = "";
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == ' ') {
      result += "%20";
    } else {
      result += str[i];
    }
  }
  return result;
}
void IRAM_ATTR pushISR() // ISR push
{
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime[4] > debounceDelay) 
  {
    detachInterrupt(BOTON);
    lastDebounceTime[2] = currentTime;
    enviar = true;
  }

}
String Api(char metodo[], String parametros[],int numparam) //Hace una llamada al metodo del Api indicado con los parametros que van en el array Los parametros van en formato 'paramname=paramvalue'
{
  
   int f, responsecode;
  String postData, url, payload;
  DeserializationError error;   // por si esta mal formada la respuesta
  postData = parametros[0];
  if(numparam >1)
  {
    for (f = 1; f < numparam; f++) // concatenamos los elementos pasados en parametros en el formato parm1=valor1&param2=valor2....
      {
        postData += "&";
        postData += parametros[f];
      }
  }
  //borrame = postData;
  url = _URL;
  url += metodo; // con esto queda formada la url del API con su metodo al final
  
  Serial.print("Comunicando con: ");
  
  Serial.println(url);
  
  Serial.println(postData);
  
  delay(10000);
  apicall.begin(url);    // iniciamos la llamada al api
  // Especificamos el header content-type
  
  apicall.addHeader("Content-Type", "application/x-www-form-urlencoded");
  responsecode = apicall.POST(postData);  
  if(responsecode != 200)
    {
      Serial.printf("Ha habido un error %d al comunicar con el api.\n",responsecode);
      Serial.println(url);
      Serial.println(postData);
    }
  payload = apicall.getString();
  error = deserializeJson(doc,payload);   // deserializamos la respuesta y la metemos en el objeto doc
  return(payload);
}