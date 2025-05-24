#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Wi-Fi and Firebase Credentials ---
#define WIFI_SSID "Sakib"
#define WIFI_PASSWORD "Alhamdulillah@2024"
#define API_KEY "AIzaSyAdWXBhJyBMxMqon98JsP9_KcvyGEKTIl0"
#define DATABASE_URL "https://mars-rover-297aa-default-rtdb.firebaseio.com/"
#define USER_EMAIL "parvezsakib121@gmail.com"
#define USER_PASSWORD "ParvezSakib@25078"

// --- OLED Display ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Sensor Pins ---
#define DHTPIN D2
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN A0
#define TRIG_PIN D5
#define ECHO_PIN D6

// --- Sensor Setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Firebase Setup ---
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- Frequency Measurement ---
unsigned long lastFrequencyUpdate = 0;
int objectDetectionCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

// --- Initialize OLED ---
if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println(F("SSD1306 allocation failed"));
  for (;;);
}
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);  // Line 1
display.println("Hello,I am Pioneer-X");
display.setCursor(0,10); // Line 2
display.println("An exploration rover");

display.display();

  // --- Connect to Wi-Fi ---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  // --- Firebase Configuration ---
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // --- Initialize Sensors ---
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Setup completed.");
}

void loop() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    temperature = 0;
  }

  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  // Ultrasonic Distance
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  bool objectDetected = distance < 20;

  if (Firebase.ready()) {
    Firebase.RTDB.setFloat(&fbdo, "/sensorData/temperature", temperature);
    Firebase.RTDB.setInt(&fbdo, "/sensorData/soilMoisture", soilMoisture);
    Firebase.RTDB.setFloat(&fbdo, "/sensorData/ultrasonicDistance", distance);
    Firebase.RTDB.setBool(&fbdo, "/sensorData/objectDetected", objectDetected);
  } else {
    Serial.println("Firebase not ready!");
  }

  if (objectDetected) {
    objectDetectionCount++;
  }

  if (millis() - lastFrequencyUpdate >= 60000) {
    int detectionFrequency = objectDetectionCount;

    if (Firebase.ready()) {
      Firebase.RTDB.setInt(&fbdo, "/sensorData/objectDetectionFrequency", detectionFrequency);
    }

    objectDetectionCount = 0;
    lastFrequencyUpdate = millis();
  }

  Serial.println("----- Sensor Readings -----");
  Serial.print("Temperature (C): ");
  Serial.println(temperature);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  Serial.print("Ultrasonic Distance (cm): ");
  Serial.println(distance);
  Serial.print("Object Detected: ");
  Serial.println(objectDetected);
  Serial.println("---------------------------");

  delay(2000);
}
