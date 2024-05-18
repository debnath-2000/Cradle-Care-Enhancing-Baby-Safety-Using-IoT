#include <DFRobot_MLX90614.h>
#include <Wire.h>
#include <Servo.h>

DFRobot_MLX90614_I2C sensor;   // instantiate an object to drive our sensor
Servo myServo;

const int rainSensorPin = A1;   // Rain sensor pin
const int analogSoundPin = A0;  // Sound sensor pin
const int mq135Pin = A2;         // LPG sensor pin
const int heaterPin = 2;         // Heater pin for LPG sensor
const int buzzerPin = 3;         // Buzzer pin for LPG alarm
const int servoPin = 9;          // Servo pin for crying detection
const int PULSE_SENSOR_PIN = A4; // Analog pin connected to the pulse sensor

const int rainThreshold = 500;  // Rain sensor threshold
const int cryThreshold = 500;   // Sound sensor threshold
const int threshold_LPG = 10000; // LPG sensor threshold

void setup() {
  Serial.begin(9600);
  while (NO_ERR != sensor.begin()) {
   // Serial.println("Communication with MLX90614 failed, please check connection");
    delay(3000);
  }
 // Serial.println("Begin MLX90614 ok!");

  sensor.enterSleepMode();
  delay(50);
  sensor.enterSleepMode(false);
  delay(200);
  sensor.setEmissivityCorrectionCoefficient(0.98);

  pinMode(rainSensorPin, INPUT);
  pinMode(analogSoundPin, INPUT);
  pinMode(mq135Pin, INPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(heaterPin, HIGH);

  myServo.attach(servoPin);

  delay(2000); // Allow sensors to warm up
}

void loop() {
  // MLX90614 Temperature Reading
  float ambientTemp = sensor.getAmbientTempCelsius();
  float objectTemp = sensor.getObjectTempCelsius();
  Serial.println();
  Serial.print("t");
  //Serial.print("Ambient celsius : ");
  //Serial.print(ambientTemp); 
  //Serial.println(" C");
  //Serial.print("Object celsius : ");  
 // Serial.print(objectTemp);  
  //Serial.println(" C");

  //Serial.print("Ambient fahrenheit : ");
  //Serial.print(ambientTemp*9/5 + 32); 
  //Serial.println(" F");
  //Serial.print("Object fahrenheit : ");  
  Serial.println(objectTemp*9/5 + 32);  
  //Serial.println(" F");

  // Rain Sensor Reading
  int rainSensorReading = analogRead(rainSensorPin);
  Serial.print("w");
 // Serial.print("Rain Sensor Reading: ");
 
  if (rainSensorReading < rainThreshold) {
     Serial.println(rainSensorReading);
    //Serial.println("Water Detected");
    // Take appropriate action for rain detection
    // Example: activate an alarm, close a window, etc.
  } else {
    //Serial.println("No Water Detected");
    // Perform other actions or continue normal operation
  }

  // Sound Sensor Reading
  int soundSensorReading = analogRead(analogSoundPin);
  Serial.print("s");
  //Serial.print("Sound Sensor Reading: ");
  Serial.println(soundSensorReading);
  if (soundSensorReading > cryThreshold) {
    Serial.print("s");
    Serial.println(soundSensorReading);
    //Serial.println("Baby is Crying!");
    // If baby crying detected, move the servo
    myServo.write(270);
    delay(1000);
    myServo.write(0);
    delay(1000);
   myServo.write(180);
  }

  // LPG Sensor Reading
  int lpgSensorReading = analogRead(mq135Pin);
  float sensorVoltage = lpgSensorReading * (5.0 / 1023.0);
  float Rs = ((5.0 * 10.0) / sensorVoltage) - 10.0;
  float ppm_LPG = pow(10, ((log10(Rs / 90) - 0.8) / -0.25)) * 0.05;
  Serial.print("g");
  //Serial.print("LPG Concentration (ppm): ");
  Serial.println(ppm_LPG);
  if (ppm_LPG > threshold_LPG) {
   // Serial.println("LPG Gas Detected");
    // If LPG gas concentration exceeds threshold, activate buzzer
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
  }

  // Pulse Sensor Reading
  int sensorValue = analogRead(PULSE_SENSOR_PIN);
  int heartRate = map(sensorValue, 0, 1023, 40, 150);// Map the sensor value to heart rate range
  Serial.print("b");
 // if (heartRate >= 80 && heartRate <= 100) {
  //  Serial.print("Heartbeat Detected! Heart Rate: ");
    Serial.println(heartRate);
    delay(300); // Adjust delay as needed
  //}

  delay(2000); // Adjust delay as needed
}
