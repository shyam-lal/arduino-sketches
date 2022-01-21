#include <PubSubClient.h>
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid = "abcd";
const char* password = "1234567890";
const char* mqtt_server = "broker.mqtt-dashboard.com";

//const char *topic = "setup";
char topic[5] = "setu";
char payloadType;
char payloadChar[10], payloadId[4];

char pub_str[100];
String message, payloadMsg;


WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi()
{

delay(10);

Serial.print("connecting to");
 Serial.println(ssid);
 WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED)
 {
 delay(500);
 Serial.print("-");
 }

Serial.println();
 Serial.println("WiFi Connected");
 Serial.println("WiFi got IP");
 Serial.println();
 Serial.println(WiFi.localIP());
}

void callback(char topic[], byte payload[], unsigned int length)
{

Serial.print("Message arrived : ");
 Serial.print(length);
 Serial.print(" : ");
 
 for (int i = 0; i < length; i++){
  Serial.print(char(payload[i]));
  payloadChar[i] = char(payload[i]);
 }

// for (int i = 5; i < length; i++){
//  Serial.println(char(payload[i]));
//  payloadMsg[i] = char(payload[i]);
// }

 payloadMsg = (String(payloadChar)).substring(5,9);
 
 payloadType = char(payload[4]);

 if(payloadType == '0'){
  Serial.print("********************");
//  for (int i = 0; i < sizeof(payloadMsg); i++){
//    topic[i] = char(payloadMsg[i]);
//    } 
//  strcpy(topic, payloadMsg);
//  client.unsubscribe("setu");
//  Serial.println("000000000000000000000");
//  Serial.println(topic);
//
//  for (int i = 0; i < sizeof(topic); i++){
//  Serial.println(char(topic[i]));
//  } 
  
  setEEPtopic(String(payloadMsg));
  Serial.println("5555555555555555555555");
  Serial.println(String(payloadMsg));
  client.unsubscribe("setu");
  setTopicFromEEP();
//  client.subscribe(topic);

//  changeTopic(payloadMsg);
 }
}

void reconnect()
{

while(!client.connected()){
Serial.println("Attempting MQTT connection");
if(client.connect("unnikuttan"))
{
Serial.println("Connected");
//client.publish("lamp001","Connected!");
//client.subscribe(topic);
setTopicFromEEP();

Serial.print("subscribed!");
}
else
{
Serial.print("Failed, rc = ");
Serial.print(client.state());
Serial.println("Waiting for 5 seconds to try again");
delay(5000);
 }
 }
}

void setup()
{
 
 pinMode(2, OUTPUT);
 Serial.begin(115200);
 EEPROM.begin(512);
 
// setEEPtopic("setup");
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 reconnect();
 setEEPtopic("setu"); 
} 

//Reading from EEPROM
void setTopicFromEEP(){
  Serial.println("++++++++++++++");
  String strText;
  for(int i=0; i<4 ;i++)
  {
    strText = strText + char(EEPROM.read(0x0F+i));
  }
  char buff[5];
  strText.toCharArray(buff, 5);
  Serial.println(strText);
  client.subscribe(buff);
}

//Storing topic too EEPROM
void setEEPtopic(String topic){
  for(int i=0; i < 4 ; i++)
  {
    EEPROM.write(0x0F+i, topic[i]);
    Serial.println("write");
    Serial.println(topic[i]);
  }
   EEPROM.commit();
}

void loop()
{

if(!client.connected())
{
  Serial.print("disconnected");
  reconnect();
}
//float tmp = gettemp();
////sprintf(pub_str,"%f", tmp);
//dtostrf(tmp,2,6,pub_str);
//Serial.print(pub_str);
//Serial.println(tmp);
//client.publish("...Your_topic_to_read_temp...",pub_str);

//String buf;
//Serial.readString().toCharArray(buf, pub_str.length);
message = Serial.readString();
//client.publish("lamp001","shyam");
delay(1000);

client.loop();
}

void changeTopic(char payloadMsg[]) {
  Serial.print("--------------------");
  String topicString = String(payloadMsg);
  Serial.println(topicString);
  client.subscribe("");
}
