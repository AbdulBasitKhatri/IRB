#include <IRremote.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Index.h"

const char* ssid = "IRB";
const char* password = "IRHACKED";

ESP8266WebServer server(80);

uint8_t STATUS_PIN = LED_BUILTIN;
uint8_t IR_RECEIVE_PIN = D5;
uint8_t IR_SEND_PIN = D6;
uint8_t DELAY_BETWEEN_REPEAT = 50;
bool blasting = false;
String recvCode = "{}";

struct storedIRDataStruct {
    decode_type_t protocol;
    uint16_t address;
    uint16_t command;
    uint8_t flags;
    uint8_t numOfBits;
    uint8_t rawCode[RAW_BUFFER_LENGTH];
    uint8_t rawCodeLength;
} sStoredIRData;

int lastButtonState;

void storeCode(IRData *aIRReceivedData);
void sendCode();

void setup() {
  Serial.begin(115200);
  
  IPAddress ip(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Initialized");
  delay(2000);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Initialized");
  pinMode(STATUS_PIN, OUTPUT);
  IrReceiver.start();
  Serial.println("Receiver Initialized");

  server.on("/", handleRoot);
  server.on("/blast", handleBlast);
  server.on("/loadCode", handleLoadCode);
  server.on("/update", handleUpdate);
  server.begin();
  Serial.println("Server started...");
}

void loop() {
  server.handleClient();
  if(blasting){
    digitalWrite(STATUS_PIN, HIGH);
    sendCode();
    digitalWrite(STATUS_PIN, LOW);
    delay(DELAY_BETWEEN_REPEAT);
  }
  else{
    if (IrReceiver.available()) {
      storeCode(IrReceiver.read());
      IrReceiver.resume();  
    }
  }
}

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleBlast(){
  if(blasting){
    blasting = false;
    IrReceiver.start();
    server.send(200, "text/plain", "Stopping...");
  }
  else{
    IrReceiver.stop();
    blasting = true;
    server.send(200, "text/plain", "Blasting...");
  }
}

void handleLoadCode(){
  server.send(200, "text/plain", toIR(server.arg("code")));
}
void handleUpdate(){
  server.send(200, "application/json", recvCode);
  recvCode = "{}";
}

void storeCode(IRData *aIRReceivedData) {
  if (aIRReceivedData->flags & IRDATA_FLAGS_IS_REPEAT) {
    Serial.println(F("Ignore repeat"));
    return;
  }
  if (aIRReceivedData->flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
    Serial.println(F("Ignore autorepeat"));
    return;
  }
  if (aIRReceivedData->flags & IRDATA_FLAGS_PARITY_FAILED) {
    Serial.println(F("Ignore parity error"));
    return;
  }
  sStoredIRData.protocol = aIRReceivedData->protocol;
  sStoredIRData.address = aIRReceivedData->address;
  sStoredIRData.command = aIRReceivedData->command;
  sStoredIRData.flags = aIRReceivedData->flags;
  sStoredIRData.numOfBits = aIRReceivedData->numberOfBits;
  if (sStoredIRData.protocol == UNKNOWN) {
    // IrReceiver.printIRResultRawFormatted(&Serial, true);
    sStoredIRData.rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
    IrReceiver.compensateAndStoreIRResultInArray(sStoredIRData.rawCode);
  } else {
    // IrReceiver.printIRResultShort(&Serial);
    sStoredIRData.flags = 0;
  }  
  recvCode = fromIR(sStoredIRData);  
}

size_t IRSendD(decode_type_t protocol, uint16_t address, uint16_t command, uint8_t flags, uint8_t numOfBits, uint_fast8_t numOfRepeats) {
  bool sendRepeat = (flags & IRDATA_FLAGS_IS_REPEAT);
  if (protocol == NEC) {
      IrSender.sendNEC(address, command, numOfRepeats, sendRepeat);
  } else if (protocol == SAMSUNG) {
      IrSender.sendSamsung(address, command, numOfRepeats, sendRepeat);
  } else if (protocol == SONY) {
      IrSender.sendSony(address, command, numOfRepeats, numOfBits);
  } else if (protocol == PANASONIC) {
      IrSender.sendPanasonic(address, command, numOfRepeats);
  } else if (protocol == DENON) {
      IrSender.sendDenon(address, command, numOfRepeats);
  } else if (protocol == SHARP) {
      IrSender.sendSharp(address, command, numOfRepeats);
  } else if (protocol == LG) {
      IrSender.sendLG(address, command, numOfRepeats, sendRepeat);
  } else if (protocol == JVC) {
      IrSender.sendJVC((uint8_t) address, (uint8_t) command, numOfRepeats); // casts are required to specify the right function
  } else if (protocol == RC5) {
      IrSender.sendRC5(address, command, numOfRepeats, !sendRepeat); // No toggle for repeats
  } else if (protocol == RC6) {
      IrSender.sendRC6(address, command, numOfRepeats, !sendRepeat); // No toggle for repeats
  } else if (protocol == ONKYO) {
      IrSender.sendOnkyo(address, command, numOfRepeats, sendRepeat);
  } else if (protocol == APPLE) {
      IrSender.sendApple(address, command, numOfRepeats, sendRepeat);
  #if !defined(EXCLUDE_EXOTIC_PROTOCOLS)
  } else if (protocol == BOSEWAVE) {
      IrSender.sendBoseWave(command, numOfRepeats);
  } else if (protocol == LEGO_PF) {
      IrSender.sendLegoPowerFunctions(address, command, command >> 4, sendRepeat); // send 5 autorepeats
  #endif
  }
  return 1;
}

void sendCode() {
  if (sStoredIRData.protocol == UNKNOWN) {
    IrSender.sendRaw(sStoredIRData.rawCode, sStoredIRData.rawCodeLength, 38);
    Serial.println(F("Sent raw"));
  } else {
    IRSendD(sStoredIRData.protocol, sStoredIRData.address, sStoredIRData.command, sStoredIRData.flags, sStoredIRData.numOfBits, NO_REPEATS);
    Serial.print(F("Sent"));
  }
}

String toIR(const String& json) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    return "Failed to deserialize JSON: " + String(error.f_str());
  }
  sStoredIRData.protocol = doc["protocol"].as<decode_type_t>();
  sStoredIRData.address = doc["address"].as<uint16_t>();
  sStoredIRData.command = doc["command"].as<uint16_t>();
  sStoredIRData.flags = doc["flags"].as<uint8_t>();
  sStoredIRData.numOfBits = doc["numOfBits"].as<uint8_t>();

  sStoredIRData.rawCodeLength = doc["rawCodeLength"];
  JsonArray rawCodeArray = doc["rawCode"].as<JsonArray>();
  for (int i = 0; i < sStoredIRData.rawCodeLength && i < rawCodeArray.size(); i++) {
    sStoredIRData.rawCode[i] = rawCodeArray[i].as<int>();
  }
  return "Successfully deserialized JSON into struct";
}

String fromIR(const storedIRDataStruct& data) {
  DynamicJsonDocument doc(1024);
  doc["protocol"] = data.protocol;
  doc["address"] = data.address;
  doc["command"] = data.command;
  doc["flags"] = data.flags;
  doc["numOfBits"] = data.numOfBits;
  JsonArray rawCodeArray = doc.createNestedArray("rawCode");
  for (int i = 0; i < data.rawCodeLength; i++) {
    rawCodeArray.add(data.rawCode[i]);
  }
  doc["rawCodeLength"] = data.rawCodeLength;

  String output;
  serializeJson(doc, output);
  return output;
}