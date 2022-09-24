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

// Json Doc
StaticJsonDocument<200> doc;

// global vars + init
float floatTempInC = -1;
float previousTemp = -1;

bool ensureWifi();

bool ensureMQTT();

// setup
void setup() {
    Wire.begin(SDA_PIN, SCL_PIN);

    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    dht.begin();

    // wifi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    ensureWifi();

    // mqtt
    mqttClient.begin(MQTT_BROKER_HOST, MQTT_BROKER_PORT, wifiClient);
    ensureMQTT();

    delay(500);
}

bool ensureMQTT() {
    // all good if connected
    if (mqttClient.connected()) {
        return true;
    }

    // connect
    mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);

    int i = 0;
    while (!mqttClient.connected() && i < 20) {
        blinkConnectionStatus(CONN_SERVICE_MQTT, CONN_STATE_CONNECTING);
        i++;
    }

    if (i < 20) {
        blinkConnectionStatus(CONN_SERVICE_MQTT, CONN_STATE_CONNECTED);
        delay(500);
        publishDiscovery();
        return true;
    }
    blinkConnectionStatus(CONN_SERVICE_MQTT, CONN_STATE_FAILED);
    delay(500);
    return false;
}

bool ensureWifi() {
    // all good if connected
    if (WiFi.isConnected()) {
        return true;
    }
    // initiate reconnect
    WiFi.reconnect();
    int i = 0;
    while (WiFiClass::status() != WL_CONNECTED && i < 30) {
        blinkConnectionStatus(CONN_SERVICE_WIFI, CONN_STATE_CONNECTING);
        i++;
    }
    if (i < 30) {
        blinkConnectionStatus(CONN_SERVICE_WIFI, CONN_STATE_CONNECTED);
        return true;
    }
    blinkConnectionStatus(CONN_SERVICE_WIFI, CONN_STATE_FAILED);
    return false;
}

void loop() {
    if (ensureWifi() && ensureMQTT()) {
        floatTempInC = readTemp();
        mqttClient.loop();
        publishTemp(floatTempInC);

        if (DISPLAY_SHOW_TEMP) {
            renderTemp(floatTempInC);
        }
    }
    delay(10000);
}

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


void renderTemp(float temp) {
    char strTempInC[7];
    dtostrf(temp, 5, 2, strTempInC);
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Temperature");
    u8g2.drawStr(0, 20, strTempInC);
    u8g2.drawStr(32, 20, "C");
    u8g2.sendBuffer();
}

float readTemp() {
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));

        // use previous reading (sometimes the sensor will give a nan response randomly, this ensures that
        // the measured temperature doesn't drop wildly each time that happens
        t = previousTemp;
    }
    previousTemp = t;
    return t;
}

void publishDiscovery() {
    char localDoc[200];
    doc.clear();

    doc["name"] = MQTT_NAME;
    doc["uniq_id"] = MQTT_CLIENT_ID;
    doc["dev_cla"] = "temperature";
    doc["stat_t"] = MQTT_TOPIC;
    doc["unit_of_meas"] = "°C";
    doc["val_tpl"] = "{{ value_json.temperature }}";
    serializeJson(doc, localDoc);

    mqttClient.publish(MQTT_DISCOVER, localDoc);
}

void publishTemp(float temp) {
    char localDoc[200];
    doc.clear();

    doc["temperature"] = (int) (temp * 100 + 0.5) / 100.0; // NOLINT(bugprone-incorrect-roundings)
    serializeJson(doc, localDoc);

    mqttClient.publish(MQTT_TOPIC, localDoc);
}
