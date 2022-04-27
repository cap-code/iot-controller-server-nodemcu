#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#define LED 2
#define LIGHT1 15
#define LIGHT2 5
#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

void hexdump(const void *mem, uint32_t len, uint8_t cols) 
{
	const uint8_t* src = (const uint8_t*) mem;
	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	USE_SERIAL.printf("\n");
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) 
{
	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			webSocket.sendTXT("MSG201");//esp8266
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
			if(strncmp((char *)payload,"1231",4)==0){
			  USE_SERIAL.printf("turning on\n");
			  digitalWrite(LIGHT1,LOW);
			}else if(strncmp((char *)payload,"123-1",5)==0){
			  USE_SERIAL.printf("turning off\n");
			  digitalWrite(LIGHT1,HIGH);
			}else if(strncmp((char *)payload,"4561",4)==0){
				USE_SERIAL.printf("turning on\n");
			    digitalWrite(LIGHT2,LOW);
			}else if(strncmp((char *)payload,"456-1",5)==0){
				USE_SERIAL.printf("turning on\n");
			    digitalWrite(LIGHT2,HIGH);
			}
			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			// hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
		case WStype_ERROR:	
			USE_SERIAL.printf("[WSc] error\n");
			break;
	
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}


void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LIGHT1,OUTPUT);
  pinMode(LIGHT2,OUTPUT);
  delay(1000);
  // WiFi.begin("","");
  Serial.println("connecting");
  for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

  WiFiMulti.addAP("POCO M2", "hotbag12345");
  while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	webSocket.begin("192.168.206.130", 4000, "/");
	webSocket.onEvent(webSocketEvent);
	webSocket.setReconnectInterval(3000);
  if(WiFiMulti.run() == WL_CONNECTED){
       digitalWrite(LED, LOW);
  }
  Serial.println();
  Serial.println("wifi connected, ip Adddress:");
  Serial.println(WiFi.localIP());
  webSocket.enableHeartbeat(15000, 3000, 2);

}

void loop() {
  	webSocket.loop();
}