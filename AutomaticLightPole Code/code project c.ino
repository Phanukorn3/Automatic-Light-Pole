#include <WiFiS3.h>
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;
uint32_t frame_off[3] = { 0, 0, 0 };
uint32_t frame_on[3] = { 0x0E011011, 0x01F01001, 0x800F0000 };

const char* ssid = "pw";  // Your SSID
const char* password = "qwert245";         // No password for open networks

const int trigPin = 9;
const int echoPin = 11;

const int RED_PIN = 8;    
const int GREEN_PIN = 7;  
const int BLUE_PIN = 6;   

long duration;
int distanceCm, distanceInch;
int status = WL_IDLE_STATUS;

String outputbegin = "on";
String outputR = "off";
String outputG= "off";
String outputB = "off";


String header = "";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  connectToWiFi();
  server.begin();
  matrix.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

}

void loop() {
    // Clear the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    Serial.print("Distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");
    Serial.print("WHITE : ");
    Serial.println(outputbegin);
    Serial.print("RED : ");    
    Serial.println(outputR);
    Serial.print("GREEN : ");
    Serial.println(outputG);
    Serial.print("BLUE : ");
    Serial.println(outputB);   
    delay(1000);
    // Calculate the distance in cm and inches
    distanceCm = duration * 0.034 / 2;
    if (distanceCm <= 10) {
        if (outputR == "on") {
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, HIGH);        
        }
        else if (outputG == "on") {
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, HIGH);
        }
        else if (outputB == "on") {
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, LOW);
        }
        else if (outputbegin == "on"){
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, LOW);
        }
    }
    else {   
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(BLUE_PIN, HIGH);
    }



  WiFiClient client = server.available();  

  if (client) {                    
    Serial.println("new client");  
    String currentLine = "";
    header = "";        
    while (client.connected()) {   
      if (client.available()) {    
        char c = client.read();   
        Serial.write(c);   
        header += c;    
        if (c == '\n') {          
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

           
            if (header.indexOf("GET /red") >= 0) {
                Serial.println("Setting color to RED");
                outputbegin = "off";
                outputR = "on";
                outputG = "off";
                outputB = "off";
            } else if (header.indexOf("GET /green") >= 0) {
                Serial.println("Setting color to GREEN");
                outputbegin = "off";
                outputR = "off";
                outputG = "on";
                outputB = "off";
            } else if (header.indexOf("GET /blue") >= 0) {
                Serial.println("Setting color to BLUE");
                outputbegin = "off";
                outputR = "off"; 
                outputG = "off"; 
                outputB = "on";
            } else if (header.indexOf("GET /white") >= 0) {
                Serial.println("Setting color to BLUE");
                outputbegin = "on"; 
                outputR = "off"; 
                outputG = "off"; 
                outputB = "off";
            }

              digitalWrite(RED_PIN, HIGH);
              digitalWrite(GREEN_PIN, HIGH);
              digitalWrite(BLUE_PIN, HIGH);
                          
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
      
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".red { background-color: #ff0000;border: 5px; color: white; padding: 16px 60px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px;}");
            client.println(".green {background-color: #00FF00;border: 5px; color: white; padding: 16px 40px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px;}");
            client.println(".blue {background-color: #0000FF;border: 5px; color: white; padding: 16px 50px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px;}");      
            client.println(".white {background-color: #666;border: 5px; color: white; padding: 16px 40px; border-radius: 20px;");                  
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            

            client.println("<body><h1>LIGHT</h1>");
            client.println("<p><a href=\"/red\"><button class=\"red\">RED</button></a></p>");
            client.println("<p><a href=\"/green\"><button class=\"green\">GREEN</button></a></p>");
            client.println("<p><a href=\"/blue\"><button class=\"blue\">BLUE</button></a></p>");
            client.println("<p><a href=\"/white\"><button class=\"white\">WHITE</button></a></p>");

            client.println();

            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    client.stop();
    Serial.println("client disconnected");
  }
}
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Since there’s no password, pass only the SSID
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi network");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

