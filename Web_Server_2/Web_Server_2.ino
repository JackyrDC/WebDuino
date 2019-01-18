#include <SPI.h>
#include <Ethernet.h>

  int rel1=7;
  int rel2=6;

  String color1="danger";
  String color2="danger";
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Dirección MAC de la Ethernet Shield
IPAddress ip(192,168,9,150); //IP del Arduino
EthernetServer server(80); //Crea un servidor Web con el puerto 80 que es el puerto HTTP por defecto
 

String estado1="OFF"; //Estado del Led 1 inicialmente "OFF"
String estado2="OFF"; //Estado del Led 2 inicialmente "OFF" 

void setup()
{
  Serial.begin(9600);
 
  // Inicializa la comunicación Ethernet y el servidor
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  pinMode(rel1,OUTPUT);
  pinMode(rel2,OUTPUT);
  digitalWrite(rel1,HIGH);
  digitalWrite(rel2,HIGH);
}
 
void loop()
{
  EthernetClient client = server.available(); 
  //Crear un cliente Web
  //Verifica si se detecte un cliente a través de una petición HTTP
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true; //Una petición HTTP acaba con una línea en blanco
    String cadena="";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();//Leemos la petición HTTP carácter por carácter
        Serial.write(c);//Visualiza la petición HTTP por el Monitor Serial
        if(cadena.length()<50)
        {
                    cadena.concat(c);//Concatena el String 'cadena' con la petición HTTP (c).Convierte la petición HTTP a un String
           
                   int posicion=cadena.indexOf("Data="); //Guarda la posición de la Palabra "Data=" a la variable 'posicion'
           
                    if(cadena.substring(posicion)=="Data=1")//Si en la posición hay "Data=1"
                    {
                      digitalWrite(rel2,HIGH);
                      delay(10000);
                      digitalWrite(rel1,LOW);
                      estado1="ON";
                      estado2="OFF";
                      color1="success";
                      color2="danger";
                    }
                    else if(cadena.substring(posicion)=="Data=2")//Si en posición hay "Data=2"
                    {
                      digitalWrite(rel1,HIGH);
                      estado1="OFF";
                      color1="danger";
                    }
                    else if(cadena.substring(posicion)=="Data=3")//Si en la posición hay "Data=3"
                    {
                       digitalWrite(rel1,HIGH);
                       delay(10000);
                       digitalWrite(rel2,LOW);
                       estado2="ON";
                       estado1="OFF";
                       color1="danger";
                      color2="success";
                    }
                    else if(cadena.substring(posicion)=="Data=4")//Si en la posición hay "Data=4"
                    {
                      digitalWrite(rel2,HIGH);
                      estado2="OFF";
                      color2="danger";
                    } 
                          
        }
        //Cuando reciba una línea en blanco, quiere decir que la petición HTTP ha acabado y el servidor Web está listo para enviar una respuesta
        if (c == 'n' && currentLineIsBlank) {
 
            // Enviamos al cliente una respuesta HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
 
            //Página web en formato HTML
            client.println("<!DOCTYPE html>");
            client.println("<html lang='es-hn'>");                 
            client.println("<head><title>Monitor 1</title><meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'><link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.2.1/css/bootstrap.min.css' integrity='sha384-GJzZqFGwb1QTTN6wy59ffF1BuGJpLSa9DkKMp0DgiMDm4iYMj70gZWKYbI706tWS' crossorigin='anonymous'></head>");
            client.println("<body>");
            client.println("<div class='container-fluid'><div class='row bg-info' height='15%'></div></div>");
            client.println("<div class='container-fluid'><div class='row'>");    
            client.println("<div class='col-3 col-md-0'></div><span class='col-12 col-md-5 text-white bg-"+color1+"'>Estado del Relay 2 = ");client.print(estado1);client.println("</span></div>");             
            client.println("<br />");            
            client.print("<button class='btn btn-info' onClick=location.href='./?Data=1'>ON</button>");           
            client.print("<button class='btn btn-info' onClick=location.href='./?Data=2'>OFF</button>");
            client.println("<br /><br />");
            client.println("<div class='row'>");    
            client.println("<div class='col-3 col-md-0'></div><span class='col-12 col-md-5 text-white bg-"+color2+"'>Estado del Relay 3 = ");client.print(estado2);client.println("</span></div>");
            client.println("<br />");        
            client.print("<button class='btn btn-info'  onClick=location.href='./?Data=3'>ON</button>");           
            client.print("<button class='btn btn-info' onClick=location.href='./?Data=4'>OFF</button>");
            client.println("<br /><br />");
            client.println("<br /><br />");             
            client.println("</div></b></body>");
            client.println("</html>");
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
  
}
  
