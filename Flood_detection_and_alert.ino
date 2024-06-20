#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Initialize the LCD, I2C address 0x27, 16 columns and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the pins for the ultrasonic sensor
#define TRIG_PIN 11
#define ECHO_PIN 12

// Define the pin for the buzzer
#define BUZZER_PIN 7

// Define the pins for the LEDs
#define LED_LOW_PIN 6
#define LED_MED_PIN 5
#define LED_HIGH_PIN 4

// Define the pins for the SIM800L GSM module
#define SIM800L_TX 2
#define SIM800L_RX 3

// Create a software serial port for the SIM800L
SoftwareSerial sim800l(SIM800L_TX, SIM800L_RX);

// Threshold for flood level in centimeters
#define FLOOD_THRESHOLD 100
#define MEDIUM_WATER_LEVEL 500
#define HIGH_WATER_LEVEL 700

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  sim800l.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Initialize the pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(LED_LOW_PIN, OUTPUT);
  pinMode(LED_MED_PIN, OUTPUT);
  pinMode(LED_HIGH_PIN, OUTPUT);

  // Test the LCD display
  lcd.setCursor(0, 0);
  lcd.print("Flood Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");
  delay(2000);
  lcd.clear();

  // Send initialization message via SMS
  sendSMS("Flood monitoring system initialized and running.");
}

void loop() {
  long duration, distance;

  // Send a pulse to trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the duration of the echo pulse
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in centimeters
  distance = duration * 0.034 / 2;

  // Display the distance on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Water Level:");
  lcd.setCursor(0, 1);
  lcd.print(distance);
  lcd.print(" cm  ");

  // Control LEDs based on water level
  if (distance < FLOOD_THRESHOLD) {
    digitalWrite(LED_LOW_PIN, HIGH);
    digitalWrite(LED_MED_PIN, LOW);
    digitalWrite(LED_HIGH_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("!! FLOOD ALERT !!");
    sendSMS("Flood alert! Water level is too high.");
  } else if (distance < MEDIUM_WATER_LEVEL) {
    digitalWrite(LED_LOW_PIN, LOW);
    digitalWrite(LED_MED_PIN, HIGH);
    digitalWrite(LED_HIGH_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } else if (distance < HIGH_WATER_LEVEL) {
    digitalWrite(LED_LOW_PIN, LOW);
    digitalWrite(LED_MED_PIN, LOW);
    digitalWrite(LED_HIGH_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    // No alert condition
    digitalWrite(LED_LOW_PIN, LOW);
    digitalWrite(LED_MED_PIN, LOW);
    digitalWrite(LED_HIGH_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Wait for a short time before taking another reading
  delay(1000);
}

void sendSMS(String message) {
  // Send the SMS message using the SIM800L GSM module
  sim800l.println("AT+CMGF=1");    // Set SMS mode to text
  delay(1000);
  sim800l.println("AT+CMGS=\"+1234567890\""); // Replace with your phone number
  delay(1000);
  sim800l.print(message);
  delay(1000);
  sim800l.write(26);  // ASCII code of CTRL+Z to send the SMS
  delay(1000);
}
