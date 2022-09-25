#include "main.h"
#include "config.h"

// display
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// dht sensor
DHT dht(DHTPIN, DHTTYPE);

// wifi
WiFiClient wifiClient;

// mqtt
MQTTClient mqttClient(256);
MQTTPublisher mqttPublisher(mqttClient);

// Json Doc
StaticJsonDocument<200> doc;

// global vars + init
float floatTempInC = -1;
float previousTemp = -1;
float floatHumidity = -1;
float previousHumidity = -1;

WifiStatus wifiStatus;

/**
 * success callback for mqtt connection success
 */
void mqttSuccess() {mqttPublisher.publishDiscovery();}
MQTTStatus mqttStatus(mqttClient, mqttSuccess);


/**
 * Setup
 */
void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);

    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    dht.begin();

    Serial.println("Setup Wifi");
    // wifi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    ensureStatus(wifiStatus);

    Serial.println("Setup MQTT");
    // mqtt
    mqttClient.begin(MQTT_BROKER_HOST, MQTT_BROKER_PORT, wifiClient);
    ensureStatus(mqttStatus);

    Serial.println("Setup done");
    delay(500);
}

/**
 * Main Loop
 */
void loop() {
    if (ensureStatus(wifiStatus) && ensureStatus(mqttStatus)) {
        floatTempInC = readTemperature();
        floatHumidity = readHumidity();
        mqttClient.loop();
        mqttPublisher.publishTemp(floatTempInC, floatHumidity);

        if (DISPLAY_RENDER_VALUES) {
            renderValues(floatTempInC, floatHumidity);
        }
    }
    delay(10000);
}

/**
 * blink a connection status (states.h)
 * @param service
 * @param status
 */
void blinkConnectionStatus(const char *service, const char *status) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, service);
    u8g2.drawStr(0, 20, status);
    u8g2.sendBuffer();
    delay(500);

    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, service);
    u8g2.sendBuffer();
    delay(500);

    u8g2.clearDisplay();
}

/**
 * will run connection check + reconnect for StatusWrapper
 * @return bool
 */
bool ensureStatus(StatusWrapper &status) {
    if (status.isConnected()) {
        return true;
    }

    status.connect();

    int i = 0;
    while (status.wait() && i < 20) {
        blinkConnectionStatus(status.getServiceName(), CONN_STATE_CONNECTING);
        i++;
    }

    if (i < 20) {
        blinkConnectionStatus(status.getServiceName(), CONN_STATE_CONNECTED);
        status.success();
        delay(500);
        return true;
    }
    blinkConnectionStatus(status.getServiceName(), CONN_STATE_FAILED);
    delay(500);
    return false;
}

/**
 * read temp from dht sensor
 * @return
 */
float readTemperature() {
    float t = dht.readTemperature();

    if (isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));

        // use previous reading (sometimes the sensor will give a nan response randomly, this ensures that
        // the measured temperature doesn't drop wildly each time that happens
        t = previousTemp;
    }
    previousTemp = t;
    return t;
}

/**
 * read temp from dht sensor
 * @return
 */
float readHumidity() {
    float h = dht.readHumidity();

    if (isnan(h)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        h = previousHumidity;
    }
    previousHumidity = h;
    return h;
}

/**
 * render temp on display
 * @param temp
 */
void renderValues(float temp, float hum) {
    char tmpString[7];
    u8g2.clearBuffer();
    dtostrf(temp, 5, 2, tmpString);
    u8g2.drawStr(0, 10, "Temperature");
    u8g2.drawStr(0, 20, tmpString);
    u8g2.drawStr(32, 20, "C");

    dtostrf(hum, 5, 2, tmpString);
    u8g2.drawStr(0, 30, "Humidity");
    u8g2.drawStr(0, 40, tmpString);
    u8g2.drawStr(32, 40, "%");
    u8g2.sendBuffer();
}

