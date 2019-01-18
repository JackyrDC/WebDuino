#include <SPI.h>
#include <Ethernet.h>
  String color1="danger";
  String color2="danger";
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Dirección MAC de la Ethernet Shield
IPAddress ip(192,168,9,150); //IP del Arduino
EthernetServer server(80); //Crea un servidor Web con el puerto 80 que es el puerto HTTP por defecto
String funcionalidad="";
#define AUTO 9
#define MANUAL 7


void setup() 
{
 pinMode(AUTO,INPUT);
 pinMode(MANUAL,INPUT); 
   Serial.begin(9600);
 
  // Inicializa la comunicación Ethernet y el servidor
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("IP del servidor:  ");
  Serial.println(Ethernet.localIP()); 
}

void loop() 
{
  
  if(digitalRead(AUTO)==0)
  {
    funcionalidad="AUTOMÁTICO";
  }
  if(digitalRead(MANUAL)==0)
  {
    funcionalidad="MANUAL";
  }
  a:;
    EthernetClient client = server.available(); 
  //Crear un cliente Web
  //Verifica si se detecte un cliente a través de una petición HTTP
  if (client)
  {
    Serial.println("new client");
    boolean currentLineIsBlank = true; //Una petición HTTP acaba con una línea en blanco
    String cadena="";
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();//Lee la petición HTTP carácter por carácter
        Serial.write(c);//Visualiza la petición HTTP por el Monitor Serial
         if (c == 'n' && currentLineIsBlank) 
         {
           // Enviamos al cliente una respuesta HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();

            //HTML 
            client.println("<!DOCTYPE html><html lang='es-HN>'");
            client.println("<head><title>Monitor 1</title><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'><link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.2.1/css/bootstrap.min.css' integrity='sha384-GJzZqFGwb1QTTN6wy59ffF1BuGJpLSa9DkKMp0DgiMDm4iYMj70gZWKYbI706tWS' crossorigin='anonymous'></head>");
            client.println("<body>");
            client.println("<div class='container-fluid'><div class='row bg-info'><h1 class='badge badge-pill-dark text-black mt-2 font-weight-bolder' style='font-size:2em'>RADINSA</h1></div></div>");
            client.println("<div class='container-fluid'><div class='row'>");
            client.println("<span class='mt-5 badge badge-info'>Funcionando en modo &nbsp"+funcionalidad+"</span>");    
           break;
         }
              if (c == 'n') {
          currentLineIsBlank = true;
        }
        else if (c != 'r') {
          currentLineIsBlank = false;
        }
      }
    }
       //Dar tiempo al navegador para recibir los datos
    delay(1);
    client.stop();// Cierra la conexión
  }
  funcionalidad="";
}
