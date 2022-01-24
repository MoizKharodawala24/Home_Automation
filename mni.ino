#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

WiFiClient client; 
ESP8266WebServer server1(80);
WiFiServer server(80);

/* WIFI settings */
char* ssid = "Muffadal villa"; //WIFI SSID
const char* password = "7d08c10c17974bc";  //WIFI PASSWORD
/* data received from application*/ 
String data ="";
/* define L298N or L293D motor control pins and ir pins*/ 
int Relay1 = 12; //D6 
int Relay2 = 13; //D7
int irPin1=4; //D2 
int irPin2=5; //D1
/* variables will help in counting*/ 
int count=0;
boolean state1 = true;
boolean state2 = true;
boolean insideState = false;
boolean outsideIr=false;
boolean isPeopleExiting=false;
int i=1;
String page="";

void setup()
{
/* initialize motor control pins as output*/
pinMode(Relay1, OUTPUT); 
pinMode(Relay2, OUTPUT);
pinMode(irPin1, INPUT); 
pinMode(irPin2, INPUT);

/* start server communication*/
Serial.begin(9600);
connectWiFi();
server1.on("/",[](){
  page="<h1>Nice</h1><h1>Data:</h1><h1>"+String(data)+"</h1>";
  server1.send(200,"text/html",page);
  });
server1.begin();
server.begin();
 
digitalWrite(Relay1,HIGH); 
digitalWrite(Relay2,HIGH);

Serial.println("Started");
}

void loop()
{
/*Counting of people in room*/
if(!digitalRead(irPin1) && i==1 && state1)
{ outsideIr=true; 
delay(100); i++;
state1 = false;
}

if(!digitalRead(irPin2) && i==2 && state2)
{Serial.println("Entering into room"); 
outsideIr=true;
delay(100); i = 1 ;
count++;
Serial.print("No of persons inside the room: "); 
Serial.println(count);
state2 = false;
}
if(!digitalRead(irPin2) && i==1 && state2)
{outsideIr=true;
delay(100);
i = 2 ;
state2 = false;
}
if(!digitalRead(irPin1) && i==2 && state1)
{ Serial.println("Exiting from room"); 
outsideIr=true;
delay(100); 
count--; 
if(count<=0)
{count=0;
}
Serial.print("No of persons inside the room: ");
Serial.println(count);
i = 1;
state1 = false;
}
if (digitalRead(irPin1)){
state1 = true;
}

if (digitalRead(irPin2)){state2 = true;
}
if(count<=0)
{ count=0; 
digitalWrite(Relay1,HIGH); 
digitalWrite(Relay2,HIGH);
}

else if(count<=5 && count>0){
  digitalWrite(Relay1,LOW); 
  digitalWrite(Relay2,HIGH);
}

else if(count>5 && count<=10){ 
  digitalWrite(Relay2,LOW); 
  digitalWrite(Relay1,HIGH);
}
/* If the server available, run the "checkClient" function*/
client = server.available(); 
if (!client) return;
data = checkClient();
Serial.println(data);

/* Conditions switching on and off*/ 
if (data == "Relay1ON")
{
digitalWrite(Relay1,LOW);
}
else if(data == "Relay1OFF")
{
digitalWrite(Relay1,HIGH);
}
if (data == "Relay2ON")
{
digitalWrite(Relay2,LOW); 
digitalWrite(Relay1,HIGH);
}
else if(data == "Relay2OFF")
{
digitalWrite(Relay2,HIGH);
}

}
void connectWiFi()
{
Serial.println("Connecting to WIFI");
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  }
/************ RECEIVE DATA FROM the APP **************/ 
String checkClient (void)
{
while(!client.available()) delay(1);
String request = client.readStringUntil('\ r'); 
request.remove(0, 5); 
request.remove(request.length()-9,9); 
return request;
}
