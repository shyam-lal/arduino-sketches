#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#define LED D1

ESP8266WebServer server(80);
String routerSSID = "";
String routerPass = "";
String html_home;

char webpage[] PROGMEM = R"=====(
  <html>
  <head>
  </head>
  <body>
  <h1>HELLO WORLD</h1>
  </body>
  </html>
  )=====";

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  prepareFile();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP("node","");

  //delay(100);
  server.on("/socket.io.js", handleScript);
  server.on("/", onConnect);
  //server.on("/test.html", handleHTML);
    server.on("/test.html", []() {
        // send home.html
        Serial.println("page called");
        server.send(200, "text/html", html_home);
    });

  server.on("/ledOn", ledOn);
  server.on("/ledOff", ledOff);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}

String sendHtml() {
  String page = "<!DOCTYPE html>";
  page += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  page += "<body><h1>ANUNTE LIGHT<h1><body>";
  page += "<form action=\"/action_page\">";
  page += "<label for=\"fname\">First name:</label><br>";
  page += "<input type=\"text\"  name=\"fname\" value=\"\"><br>";
  page += "<label for=\"lname\">Last name:</label><br>";
  page += "<input type=\"text\" name=\"lname\" value=\"\"><br><br>";
  page += "<input type=\"submit\" value=\"Submit\"></form>";
  page += "<p>If you click the \"Submit\" button,the form-data will be sent to a page called.</p></body></html>";
  return page;
}

void prepareFile(){
  
  Serial.println("Prepare file system");
  SPIFFS.begin();
  
  File file = SPIFFS.open("/test.html", "r");
  if (!file) {
    Serial.println("file open failed");  
  } else{
    Serial.println("file open success");

    html_home = "";
    while (file.available()) {
      //Serial.write(file.read());
      String line = file.readStringUntil('\n');
      html_home += line + "\n";
    }
    file.close();
    Serial.print(html_home);
  }
}

void handleFile(char *filename) {
  File f = SPIFFS.open(filename, "r");
  if (f){
  String s;
  while (f.available()){
    s += char(f.read());
    }
    server.send(200, "text/html", s);
    }
  else {
    server.send(200, "text/html", "Error: File does not exist");
  }
}

void handleScript() {
  handleFile("/socket.io.js");
}

void handleHTML() {
  handleFile("./test.html");
}

void onConnect() {
  digitalWrite(LED, LOW);
  server.send(200, "text/html", webpage);
}

void ledOn() {
  digitalWrite(LED, HIGH);
}

void ledOff() {
  digitalWrite(LED, LOW);
}

void handleForm() {
 String firstName = server.arg("fname"); 
 routerSSID = firstName;
 String lastName = server.arg("lname"); 
 routerPass = lastName;

 Serial.print("First Name:");
 Serial.println(routerSSID);

 Serial.print("Last Name:");
 Serial.println(routerPass);
 
 String s = "<a href='/'> Go Back </a>";
 server.send(200, "text/html", s); //Send web page
 delay(2000);
 setUpSat();
}

void setUpSat() {
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  Serial.println("started");
  WiFi.mode(WIFI_STA);
  WiFi.begin(routerSSID, routerPass);
  //while (WiFi.status() != WL_CONNECTED)delay(500);
  Serial.println("Connected to router");
}
