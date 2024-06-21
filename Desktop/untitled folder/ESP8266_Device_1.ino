/*Water level monitoring system with the New Blynk app
   Home Page
*/
//Include the library files
#include <Wire.h>
#include <hd44780.h>                   // Main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h>  
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_TEMPLATE_ID "TMPL6H0vtxtHX"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring System"
#define BLYNK_AUTH_TOKEN "UUIYBCUCycG8tL93Mk7fblOgfpxYgOzV"

//Initialize the LCD display
hd44780_I2Cexp lcd(0x27);

 char auth[] = "UUIYBCUCycG8tL93Mk7fblOgfpxYgOzV";

 char ssid[] = "GUSTO University";//Enter your WIFI name
 char pass[] = "weloveGUSTO";//Enter your WIFI password

// char ssid [] = "ShanLay";
// char pass [] = "shanlayhtet232023";

BlynkTimer timer;
int manualTimeout = 5000; // Manual control timeout in milliseconds
unsigned long lastManualCommand = 0; // Last manual command timestamp

// Define the component pins
#define trig D7
#define echo D8
#define LED1 D0
#define LED2 D3
#define LED3 D4
#define LED4 D5
#define LED5 D6
#define relay 3 

//Enter your tank max value(CM)
int MaxLevel = 10;
int Level1 = (MaxLevel * 75) / 100;
int Level2 = (MaxLevel * 65) / 100;
int Level3 = (MaxLevel * 55) / 100;
int Level4 = (MaxLevel * 45) / 100;
int Level5 = (MaxLevel * 35) / 100;

bool manualControl = false;
bool motorState = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);

  lcd.setCursor(0, 0);
  lcd.print("Water level");
  lcd.setCursor(4, 1);
  lcd.print("Monitoring");
  delay(4000);
  lcd.clear();

  //Call the functions
  timer.setInterval(1000L, ultrasonic);
}

// Function to check if manual control should be disabled
void checkManualTimeout() {
  if (manualControl && (millis() - lastManualCommand > manualTimeout)) {
    manualControl = false;
    Serial.println("Switching back to auto mode.");
  }
}

//Get the ultrasonic sensor values
// Bypass automatic control if manual control is active
void ultrasonic() {
  if (manualControl) { 
    return;
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
    lcd.setCursor(0, 0);
    lcd.print("WLevel:");

    if (Level1 <= distance) {
      lcd.setCursor(8, 0);
      lcd.print("Very Low");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED5, LOW);
      if (!manualControl) {
      motorState = true;  // Turn on motor if Very Low
    }  // Turn on motor if Very Low
    } else if (Level2 <= distance && Level1 > distance) {
      lcd.setCursor(8, 0);
      lcd.print("Low");
      lcd.print("      ");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED5, LOW);
      if (!manualControl) {
      motorState = true;  // Turn on motor if Very Low
    }
    } else if (Level3 <= distance && Level2 > distance) {
      lcd.setCursor(8, 0);
      lcd.print("Medium");
      lcd.print("      ");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, LOW);
      digitalWrite(LED5, LOW);
      if (!manualControl) {
      motorState = true;  // Turn on motor if Very Low
    }
    } else if (Level4 <= distance && Level3 > distance) {
      lcd.setCursor(8, 0);
      lcd.print("High");
      lcd.print("      ");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, LOW);
      if (!manualControl) {
      motorState = false;  // Turn off motor if Very Low
      }
    } else if (Level5 >= distance && !manualControl) {
      lcd.setCursor(8, 0);
      lcd.print("Full");
      lcd.print("      ");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      if (!manualControl) {
      motorState = false;  // Turn off motor if Full
    }
    }

    // Ensure motor control respects automatic safety conditions
    if (!manualControl) {
      digitalWrite(relay, motorState ? LOW : HIGH); // LOW to turn on, HIGH to turn off
    Blynk.virtualWrite(V1, motorState ? 1 : 0); // Update Blynk button state
    }

    // Update the display
    lcd.setCursor(0, 1);
    lcd.print(motorState ? "Motor is ON " : "Motor is OFF");
  }

// Get the button value
BLYNK_WRITE(V1) {
  int buttonState = param.asInt();
   lastManualCommand = millis(); // Update the last manual command timestamp
   manualControl = true;
  if (buttonState == 1) {
    motorState = true;
    digitalWrite(relay,LOW);
    //Setlevel(); // Update Firebase with the motor state change
    // Ensure the LED display is updated when manual control is used
    lcd.setCursor(0, 0);
    lcd.print("Manual Mode ");
    lcd.setCursor(0, 1);
    lcd.print(motorState ? "Motor is ON " : "Motor is OFF");
    digitalWrite(LED1, motorState ? HIGH : LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
  } else {
     manualControl = false;
     motorState = false;
     digitalWrite(relay,HIGH);
    // Ensure the LED display is updated when manual control is used
    lcd.setCursor(0, 0);
    lcd.print("Manual Mode ");
    lcd.setCursor(0, 1);
    lcd.print(motorState ? "Motor is ON " : "Motor is OFF");
    digitalWrite(LED1, motorState ? HIGH : LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
  }
  lastManualCommand = millis();
  manualControl = true;
}

void loop() {
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer
  checkManualTimeout(); // Check for manual timeout
}
