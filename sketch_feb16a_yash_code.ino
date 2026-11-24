// Include necessary libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define the I2C address for the LCD (common address is 0x27, but verify with your module)
#define LCD_ADDRESS 0x27

// Define LCD dimensions
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// Initialize the LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Define the analog pin for MQ-7 sensor
#define MQ7_PIN A0

// Define the digital pin for DHT11 sensor
#define DHT_PIN 2
#define DHT_TYPE DHT11  // DHT11 sensor type

// Initialize DHT object
DHT dht(DHT_PIN, DHT_TYPE);

// Variables for sensor readings
int sensorValue = 0;
float voltage = 0.0;
float coLevel = 0.0;  // Rough estimate in ppm (needs calibration for accuracy)
float temperature = 0.0;
float humidity = 0.0;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize DHT sensor
  dht.begin();
  
  // Print initial message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Sensors Init");
  delay(2000);  // Wait for sensors to stabilize (MQ-7 needs preheating, DHT11 needs a moment)
  
  // Clear LCD and prepare for readings
  lcd.clear();
}

void loop() {
  // Read the analog value from MQ-7 sensor
  sensorValue = analogRead(MQ7_PIN);
  
  // Convert to voltage (assuming 5V reference)
  voltage = sensorValue * (5.0 / 1023.0);
  
  // Rough CO level calculation (this is approximate and requires calibration)
  // MQ-7 sensitivity curve: Rs/R0 = (Vcc - Vout)/Vout
  // For CO, a common formula is CO_ppm = 10^((log10(Rs/R0) - b)/m) where m and b are from datasheet
  // Simplified rough estimate (not accurate without calibration):
  coLevel = (voltage - 0.1) * 100;  // Example: adjust based on your calibration
  
  // Read temperature and humidity from DHT11
  temperature = dht.readTemperature();  // In Celsius
  humidity = dht.readHumidity();
  
  // Check for DHT read errors (optional, but good practice)
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0.0;
    humidity = 0.0;
  }
  
  // Display on LCD (formatted for 16x2)
  lcd.setCursor(0, 0);
  lcd.print("CO:");
  lcd.print(coLevel, 1);
  lcd.print("ppm T:");
  lcd.print(temperature, 1);
  lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humidity, 1);
  lcd.print("% V:");
  lcd.print(voltage, 2);
  lcd.print("V");
  
  // Print to serial for debugging
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.print("V | CO Level: ");
  Serial.print(coLevel);
  Serial.print(" ppm | Temp: ");
  Serial.print(temperature);
  Serial.print(" C | Hum: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  // Delay before next reading
  delay(1000);  // Update every second
}