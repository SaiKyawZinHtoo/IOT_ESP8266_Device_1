#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>

#define BLYNK_TEMPLATE_ID "TMPL65ErV4sIj"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring System"
#define BLYNK_AUTH_TOKEN "ml00pvaUCY4Ys9SFO9frLKOWc98pEnCc"

// // Replace with your network credentials
// char ssid[] = "ShanLay";
// char pass[] = "shanlayhtet232023";

 char ssid[] = "GUSTO University";//Enter your WIFI name
 char pass[] = "weloveGUSTO";//Enter your WIFI password

// Replace with your Blynk Auth Token
char auth[] = "ml00pvaUCY4Ys9SFO9frLKOWc98pEnCc";

// Define GPIO pins for relay control
#define RELAY_PIN1 D0  // D0  - controls solenoid valve
#define RELAY_PIN2 D2  // D2 - controls motor
#define RELAY_PIN3 D3  // D3 - controls solenoid valve
#define RELAY_PIN4 D4  // D4 - controls motor
#define trig D5
#define echo D6
#define trig1 D7
#define echo1 D8

//Enter your tank max value(CM)
int MaxLevel = 7;

int Level1 = (MaxLevel * 75) / 100;
int Level2 = (MaxLevel * 65) / 100;
int Level3 = (MaxLevel * 55) / 100;
int Level4 = (MaxLevel * 45) / 100;
int Level5 = (MaxLevel * 35) / 100;

bool relayState = false; // Track the state of the relays
bool manualControl = false; // Track if manual control is active
bool motorState1 = false;
bool solenoldState1 = false;


SimpleTimer timer; // Create a Timer object
int manualTimeout = 5000; // Manual control timeout in milliseconds
unsigned long lastManualCommand = 0; // Last manual command timestamp

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  // Blynk setup
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Set relay pins as outputs
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
  pinMode(RELAY_PIN4, OUTPUT);

  // Ensure relays are off initially
  digitalWrite(RELAY_PIN1, HIGH); // Assuming HIGH means relay off
  digitalWrite(RELAY_PIN2, HIGH); // Assuming HIGH means relay off
  digitalWrite(RELAY_PIN3, HIGH); // Assuming HIGH means relay off
  digitalWrite(RELAY_PIN4, HIGH); // Assuming HIGH means relay off

  Serial.println("Setup complete. Waiting for Blynk commands...");

  // Set up a timer to check for auto mode timeout
  timer.setInterval(1000L, checkManualTimeout); // Check every second
}

// Function to check if manual control should be disabled
void checkManualTimeout() {
  if (manualControl && (millis() - lastManualCommand > manualTimeout)) {
    manualControl = false;
    Serial.println("Switching back to auto mode.");
  }
}

// Get the ultrasonic sensor values
void ultrasonic() {
  if (manualControl) {
    return; // Skip ultrasonic check if in manual control mode
  }
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  int distance = t / 29 / 2;

  int blynkDistance = (distance - MaxLevel) * -1;
  if (distance <= MaxLevel) {
    Blynk.virtualWrite(V0, blynkDistance);
  } else {
    Blynk.virtualWrite(V0, 0);
  }

  if (distance >= Level1 && !manualControl) {

    Serial.println("Water level: Very Low");
    digitalWrite(RELAY_PIN1, LOW); // Turn on solenoid valve
    digitalWrite(RELAY_PIN2, LOW); // Turn on motor
    motorState1 = true;
    solenoldState1 = true;
    Serial.println("Solenoid valve and Motor ON");

  } else if (distance <= Level5 && !manualControl) {

    Serial.println("Water level: Full");
    digitalWrite(RELAY_PIN1, HIGH); // Turn off solenoid valve
    digitalWrite(RELAY_PIN2, HIGH); // Turn off motor
    motorState1 = false;
    solenoldState1 = false;
    Serial.println("Solenoid valve and Motor OFF");
    
  }
}

// Get the ultrasonic sensor values
void ultrasonic1() {
  if (manualControl) {
    return; // Skip ultrasonic check if in manual control mode
  }
  digitalWrite(trig1, LOW);
  delayMicroseconds(4);
  digitalWrite(trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  long t = pulseIn(echo1, HIGH);
  int distance = t / 29 / 2;

  int blynkDistance = (distance - MaxLevel) * -1;
  if (distance <= MaxLevel) {
    Blynk.virtualWrite(V1, blynkDistance);
  } else {
    Blynk.virtualWrite(V1, 0);
  }

  if (distance >= Level1 && !manualControl) {
    Serial.println("Water level: Very Low");
    digitalWrite(RELAY_PIN3, LOW);
    digitalWrite(RELAY_PIN4, LOW);
    motorState1 = true;
    solenoldState1 = true;
    Serial.println("Solenoid valve and Motor ON");
  } else if (distance <= Level5 && !manualControl) {
    Serial.println("Water level: Full");
    digitalWrite(RELAY_PIN3, HIGH);
    digitalWrite(RELAY_PIN4, HIGH);
    motorState1 = false;
    solenoldState1 = false;
    Serial.println("Solenoid valve and Motor OFF");
  }
}

// Blynk function to control solenoid valve 1
BLYNK_WRITE(V2){
  int pinValue = param.asInt(); // Get value from Blynk app
  Serial.print("V2 value: ");
  Serial.println(pinValue);
  manualControl = true;
  lastManualCommand = millis(); // Update the last manual command timestamp
  if (pinValue == 1) {
    digitalWrite(RELAY_PIN1, LOW); // Turn on solenoid valve 1
    Serial.println("Solenoid valve 1 ON");
    solenoldState1 = true;
  } else {
    digitalWrite(RELAY_PIN1,HIGH); // Turn off solenoid valve 1
    Serial.println("Solenoid valve 1 OFF");
    solenoldState1 = false;
  }
}

// Blynk function to control motor 1
BLYNK_WRITE(V3){
  int pinValue = param.asInt(); // Get value from Blynk app
  Serial.print("V3 value: ");
  Serial.println(pinValue);
  manualControl = true;
  lastManualCommand = millis(); // Update the last manual command timestamp
  if (pinValue == 1) {
    digitalWrite(RELAY_PIN2, LOW); // Turn on motor 1
    Serial.println("Motor 1 ON");
    motorState1 = true;
  } else {
    digitalWrite(RELAY_PIN2, HIGH); // Turn off motor 1
    Serial.println("Motor 1 OFF");
    motorState1 = false;
  }
}

// Blynk function to control solenoid valve 1
BLYNK_WRITE(V4){
  int pinValue = param.asInt(); // Get value from Blynk app
  Serial.print("V2 value: ");
  Serial.println(pinValue);
  manualControl = true;
  lastManualCommand = millis(); // Update the last manual command timestamp
  if (pinValue == 1) {
    digitalWrite(RELAY_PIN3, LOW); // Turn on solenoid valve 1
    Serial.println("Solenoid valve 1 ON");
    solenoldState1 = true;
  } else {
    digitalWrite(RELAY_PIN3,HIGH); // Turn off solenoid valve 1
    Serial.println("Solenoid valve 1 OFF");
    solenoldState1 = false;
  }
}

// Blynk function to control motor 1
BLYNK_WRITE(V5){
  int pinValue = param.asInt(); // Get value from Blynk app
  Serial.print("V3 value: ");
  Serial.println(pinValue);
  manualControl = true;
  lastManualCommand = millis(); // Update the last manual command timestamp
  if (pinValue == 1) {
    digitalWrite(RELAY_PIN4, LOW); // Turn on motor 1
    Serial.println("Motor 1 ON");
    motorState1 = true;
  } else {
    digitalWrite(RELAY_PIN4, HIGH); // Turn off motor 1
    Serial.println("Motor 1 OFF");
    motorState1 = false;
  }
}

void loop(){
  Blynk.run();
  timer.run(); // Run the timer
  ultrasonic();
  ultrasonic1();
}

