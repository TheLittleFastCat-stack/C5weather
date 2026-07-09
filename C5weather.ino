#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
#include <WiFi.h>
#include <HTTPClient.h>

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

BH1750 bh;

#define led 15

//#define SDA_PIN 9
//#define SCL_PIN 10

#define uS_TO_S_FACTOR 1000000  /* Konverzní faktor pro mikrosekundy (1 sekunda = 1 000 000 mikrosekund) */
#define log_delay 1800

const char* ssid = "O2-Internet-110";
const char* password = "BRW8QXcGdP";

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("power on");

  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  delay(500);

  Wire.begin(9, 10);

  // Initialize AHT20
  if (!aht.begin()) {
    Serial.println("Could not find AHT20? Check wiring");
    
  }
  
  // Initialize BMP280 (Address 0x77)
  if (!bmp.begin(0x77)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    
  }

  if (!bh.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C)) {
    Serial.println("Could not find a valid BH1750 sensor, check wiring!");
    
  }

  digitalWrite(led, LOW);

  sensors_event_t hum, temp;
  aht.getEvent(&hum, &temp); // populate temp and humidity objects

  float temperature = temp.temperature;
  float humidity = hum.relative_humidity;
  
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");

  float pressure = bmp.readPressure() / 100.0;
  
  Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("Approx Altitude: "); Serial.print(bmp.readAltitude(1013.25)); Serial.println(" m");

  float light = bh.readLightLevel();
  Serial.print("Light: ");
  Serial.print(light);
  Serial.println(" lx");


  HTTPClient http;

  http.begin("http://10.0.0.94:5000/weather");
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"temperature\":" + String(temperature, 2) + ",";
  json += "\"humidity\":" + String(humidity, 2) + ",";
  json += "\"pressure\":" + String(pressure, 2) + ",";
  json += "\"light\":" + String(light, 0);
  json += "}";

  int code = http.POST(json);

  Serial.println(code);

  http.end();


  delay(1500);
  esp_sleep_enable_timer_wakeup(log_delay * uS_TO_S_FACTOR);

  Serial.println("deep sleep");

  esp_deep_sleep_start();
}

void loop() {
  
  delay(2000);
}