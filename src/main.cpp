#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "meca";
const char* password = "meca2023";

// Replace with the IP address of your server and the port
const char* serverName = "http://62.55.20.57:3000/upload";

// DHT Sensor setup
#define DHTPIN 2      // GPIO2 (D4 on NodeMCU)
#define DHTTYPE DHT11 // Define the sensor type

DHT dht(DHTPIN, DHTTYPE);

// Function declarations
void connectToWiFi();
void readDHTSensor(int &temperature, int &humidity);
void sendDataToServer(int temperature, int humidity);

void setup() {
    Serial.begin(115200);
    delay(10);

    // Connect to Wi-Fi
    connectToWiFi();

    // Initialize DHT sensor
    dht.begin();
}

void loop() {
    delay(2000);

    int temperature, humidity;

    // Read temperature and humidity from DHT sensor
    readDHTSensor(temperature, humidity);

    // If readings are valid, send data to server
    if (temperature != -1 && humidity != -1) {
        sendDataToServer(temperature, humidity);
    }
}

void connectToWiFi() {
    // Connect to Wi-Fi network
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);

    // Wait until connected
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
}

void readDHTSensor(int &temperature, int &humidity) {
    // Read temperature and humidity from DHT sensor
    int t = int(dht.readTemperature() * 10); // Multiply by 10 to retain one decimal place
    int h = int(dht.readHumidity() * 10);    // Multiply by 10 to retain one decimal place

    // Check if sensor reading is valid
    if (t == -127 || h == -127) {
        Serial.println("Failed to read from DHT sensor!");
        temperature = -1;
        humidity = -1;
    } else {
        // Store temperature and humidity values
        temperature = t;
        humidity = h;
        Serial.print("Humidity: ");
        Serial.print(h / 10); // Divide by 10 to display one decimal place
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t / 10); // Divide by 10 to display one decimal place
        Serial.println(" *C");
    }
}

void sendDataToServer(int temperature, int humidity) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        // Construct URL with temperature and humidity values
        String serverPath = String(serverName) + "?temperature=" + String(temperature / 10) + "&humidity=" + String(humidity / 10);

        // Send HTTP GET request to server
        http.begin(client, serverPath);
        int httpResponseCode = http.GET();

        // Check HTTP response code
        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}
