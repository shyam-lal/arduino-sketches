/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 7th Aug 2021 with the current ESP8266 Arduino core release 3.0.2
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "abcd";
const char* password = "1234567890";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "a255rwh7bdl1ck-ats.iot.us-west-2.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key:

// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVALFyQ2lLZXuH5sHJwUCX9yIl99P0MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTExMTMxNTQw
MDNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDLA4IvgwLphth/lwI3
YUGFL8GVWmlU4cF8PjkjqAvT/Swm/nLkZXotzpHwEAPWgf6FE1jJ6JsNjwmOiyDt
Ve3+7Fvh/kJXLKalO3PfR/DW8q9BrzDgNRW0s9+yboxeeEKUH5M1Rd75yJ893mDl
vJV1nIe0fRQpRHY5N8xl3ACT3ZKdUMYm1eK8RcoyI29Mzml8X0RnSCJxKMzqxBYu
8ZOnx+cORw+oA7YUIAC54vX5afPQhpdDTBZTagTwIP0Ia2GMQHVf7k/FExDyw/wA
fiEySm45ezjDcLegplID0XeU3QXCrRjq7O2YjS5qamAZ7uPX1UiV21jUMf8XUm4L
Si/hAgMBAAGjYDBeMB8GA1UdIwQYMBaAFOhLoSiV2ttV5npCpItWCxFmK73sMB0G
A1UdDgQWBBQiw8aWp5QYWvnKBNLhiszQPcM5KDAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAu9z4Yhsev4LR4SJy6jgKlnxT
m/8ACOUlRXPkuFDmWWSuV3B6ooCa02HMdmE2WhG5wgkXcRDmpj+1ckc7/jgSikFD
1zdIMw8oDKCis1aS3hIF/ZKK+zQ7FhnJTM0kb6wd4310SzzjEAqhsC1lUCJCVenG
626iYYWcCqJVXGRhQWvLkeqD93qc3yA6uQwHMasKy92l2PmcWRvNl0HEVuC6cLn3
gGH33Id/uCwQmuZHDgQuToZ3DajEIAJ9c43re8EJXIWBdrRypFB3TLIaUysgWuOl
P1KJ8sfSd2Yi9ZaReOJRKSchDuLGbnO3HGXjre8g01xQa8dmRrF6N97fHSWKUQ==
-----END CERTIFICATE-----
)EOF";

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAywOCL4MC6YbYf5cCN2FBhS/BlVppVOHBfD45I6gL0/0sJv5y
5GV6Lc6R8BAD1oH+hRNYyeibDY8Jjosg7VXt/uxb4f5CVyympTtz30fw1vKvQa8w
4DUVtLPfsm6MXnhClB+TNUXe+cifPd5g5byVdZyHtH0UKUR2OTfMZdwAk92SnVDG
JtXivEXKMiNvTM5pfF9EZ0gicSjM6sQWLvGTp8fnDkcPqAO2FCAAueL1+Wnz0IaX
Q0wWU2oE8CD9CGthjEB1X+5PxRMQ8sP8AH4hMkpuOXs4w3C3oKZSA9F3lN0Fwq0Y
6uztmI0uampgGe7j19VIldtY1DH/F1JuC0ov4QIDAQABAoIBAB4CrsQHQe7zmB2E
XTeCkQT3Qk6pxthdZxTOvfOfa+AylMxO4P0SYzVSTBCIzYhwPwe9DGsMj585fdgs
iPmMsln05sBJrdTlLN1QJJjlMZ0U0wOKulbqggLsdC0UWnu8m1r6L/C4G3YhSikD
Lp/HePCh6w2rT8ep7zAWJ4kHNK6m1sjhO6afsjIcrE9CAfrQgDKxJiOVM/wFmI1f
wT54KEe6aIE1qdnSNfl1jBPkAHgSbjjuHQFgJYX37n+mDRaPlAf8FSwvCaLiXOcH
kR+ROYkytWoL1gPHDA97O7qNbKESg2SUamL5W4f3N81Jtaw1ooSl9ZDITu//QzPJ
RQy7D2ECgYEA+rYnIQYGskxqHZ/3zy0/9f12yRGo7yy/UWATFtqHDVdO68wmA+8d
YhEF3hsY4ZQefJ8WikTFsE4NkDJn0uksjCwKS8k7x5y+suW98weNFaCzOiLJY5rS
2dkzeRM4K9wXt9T8Wu95YHozZ1JOl3OZ8xVhdLQB0JytSnGHeMZvfscCgYEAz0vJ
VyCCblP72RJSERerVS6Kz8SEJy9xLvAnNDm+RTUhq0FU991dI0tOaj7QTmv7mmhJ
NWJhArcAK2XfuYmqXYauu6frqpBxH3DlQ8NlfrcbXMMpDuE+G8HCdW5ZQ7p8CybF
spU/lmKPDjcky6oMrUOaPj9Xvh1aV42TVsZv1BcCgYEA6L5BBP87KDqYKHhg7Qvs
3Mg4heDHG4JG+ZxfHFAyH8h1k6EpiW1psahr8gzlxERlKPvnSOUfQNS08rBYggvF
NzuLM1/VBzIUNq/g7sSLzRAqUkYI/1azd1mT10poUw83VYl73E+ENnQ7VCesnhzL
RkcK+fRB2xAYL4vFX/GXf/ECgYAu9vymwLbMSDrG81+kf7hRlDne2KcYOadq+v3m
1y5Ef8UhB6w4e+6eEHGzoIeRPOXEFdyAO8m+07c2Hd6SKp5ukhUEbwb1ncQDW5CV
ya81BkcsvQqQVHe90ig8csPGlHmGDUK9MxM0/x9qGRQ9EEnO94H6mF9Is13EeE7p
Wgv8AQKBgQDZ0Y+QHHPuIavnR77tTGUjaORPQX/6wGH5N4wDCFMEYkTu/W0wMniU
99Rg7Fr543P1wytsAd2kBvI2A/RYn36Bns1XcgCEFYVhdmxeuAELCe2aSh6aeEFY
iKH1/pLMVD2TIT5At0eats4vvHsbVhl7uD00ig+rXZpG2tmPYxFfvg==
-----END RSA PRIVATE KEY-----
)EOF";

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;
    pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published: "); Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
