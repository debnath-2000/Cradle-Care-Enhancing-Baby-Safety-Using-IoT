#define BLYNK_TEMPLATE_ID "TMPL3k0FVPWo8"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "cxRNk_5MdFYjBXtDTsC-kcOdyVnsqLTf"

//#define PULSE_SENSOR_PIN A0  // Analog pin connected to the pulse sensor
#define rainThreshold  200  // Rain sensor threshold
#define cryThreshold  950  // Sound sensor threshold
#define threshold_LPG  10000 // LPG sensor threshold
#define threshold_pulse 120 // Pulse sensor
#define threshold_temp 101 // Temp sensor


/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<SoftwareSerial.h>
SoftwareSerial my(D1,D2);//RX,TX
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "xxxxxxxx";
char pass[] = "xxxxxxxxxx";

void setup() {
  Serial.begin(115200);
  my.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}


void loop() {
  Blynk.run();
  
  // Process data from SoftwareSerial
  while (my.available() > 0) {
    char data = my.read();

    // Check if the incoming data is for BPM
    if (data == 'b') {
      String bpmData = my.readStringUntil('\n');
      int bpm = bpmData.toInt();
      Serial.println(bpm);
      if(bpm>threshold_pulse)
      {
        Blynk.logEvent("pulse");
      }
      Blynk.virtualWrite(V0, bpm);
    }
    // Check if the incoming data is for water
    else if (data == 'w') {
      String waterData = my.readStringUntil('\n');
      int water = waterData.toInt();
      Serial.println(water);
      if(water>rainThreshold)
      {
        Blynk.logEvent("water_leakage");
      }
      Blynk.virtualWrite(V1, water);
    }
    // Check if the incoming data is for gas
    else if (data == 'g') {
      String gasData = my.readStringUntil('\n');
      double gas = gasData.toDouble();
      Serial.println(gas);
      if(gas>threshold_LPG)
      {
        Blynk.logEvent("gas_alert");
      }
      Blynk.virtualWrite(V2, gas);
      
    }
    // Check if the incoming data is for sound
    else if (data == 's') {
      String soundData = my.readStringUntil('\n');
      int sound = soundData.toInt();
      Serial.println(sound);
      if(sound>cryThreshold)
      {
        Blynk.logEvent("cry_alert");
      }
      Blynk.virtualWrite(V5, sound);
    }
    // Check if the incoming data is for temperature
    else if (data == 't') {
      String tempData = my.readStringUntil('\n');
      double temp = tempData.toDouble();
      Serial.println(temp);
      if(temp>threshold_temp)
      {
        Blynk.logEvent("temperature");
      }
      Blynk.virtualWrite(V4, temp);
    }
  }
}
