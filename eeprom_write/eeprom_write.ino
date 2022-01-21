#include <PubSubClient.h>
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>


String routerPass = "setu";

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Setup");
  setEEPtopic();
  readTopic();
}

void setEEPtopic(){
  Serial.println("store");
  String topic = "hello";
  for(int i=0; i < topic.length() ; i++)
  {
    Serial.println(i);
    Serial.println(String(topic.charAt(i)));
    EEPROM.write(250 + i, topic[i]);
  }
  EEPROM.write(0, '0');
  EEPROM.commit();
}

void readTopic() {
  Serial.print("Read");
  String routerPass;

  for(int i = 0; i < 5; i++){
    routerPass = routerPass + char(EEPROM.read(250+i));
    Serial.println("+++++++++++++++++++++++");
    Serial.println(i);
    Serial.println(String(char(EEPROM.read(250+i))));
  }
  Serial.print("---------------------------");
  Serial.println(routerPass);
}

void loop() {
  
}
