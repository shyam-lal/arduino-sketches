#include <PubSubClient.h>
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

void setup() {
//  setEEPtopic("setu");
  EEPROM.begin(512);
  Serial.begin(115200);
  setValue('a');
}

void setEEPtopic(char topic[]){
  for(int i=0; i < 4 ; i++)
  {
    EEPROM.write(0x0F+i, topic[i]);
  }
   EEPROM.commit();
}

void setValue(char value){
  EEPROM.write(0, value);
  EEPROM.commit();
  Serial.println(char(EEPROM.read(0)));
}

void loop() {
  // put your main code here, to run repeatedly:

}
