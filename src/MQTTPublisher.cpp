//
// Created by ppetermann on 24.09.22.
//

#include "MQTTPublisher.h"

MQTTPublisher::MQTTPublisher(MQTTClient &mqttClient) : mqttClient(mqttClient) {

}

void MQTTPublisher::publishDiscovery() {
    char localDoc[200];
    char topic[50];
    // @todo de-duplicate
    jsonDoc.clear();
    jsonDoc["name"] = MQTT_TEMPERATURE_NAME;
    jsonDoc["uniq_id"] = MQTT_TEMPERATURE_UID;
    jsonDoc["dev_cla"] = "temperature";
    sprintf(topic, "%s/%s/%s",MQTT_BASE_TOPIC, MQTT_SENSOR_NAME, "state" );
    jsonDoc["stat_t"] = topic;
    jsonDoc["unit_of_meas"] = "°C";
    jsonDoc["val_tpl"] = "{{ value_json.temperature }}";
    serializeJson(jsonDoc, localDoc);
    sprintf(topic, "%s/%s/%s", MQTT_BASE_TOPIC, MQTT_TEMPERATURE_UID, "config" );
    mqttClient.publish(topic, localDoc);

    jsonDoc.clear();
    jsonDoc["name"] = MQTT_HUMIDITY_NAME;
    jsonDoc["uniq_id"] = MQTT_HUMIDITY_UID;
    jsonDoc["dev_cla"] = "humidity";
    sprintf(topic, "%s/%s/%s",MQTT_BASE_TOPIC, MQTT_SENSOR_NAME, "state" );
    jsonDoc["stat_t"] = topic;
    jsonDoc["unit_of_meas"] = "%";
    jsonDoc["val_tpl"] = "{{ value_json.humidity }}";
    serializeJson(jsonDoc, localDoc);
    sprintf(topic, "%s/%s/%s", MQTT_BASE_TOPIC, MQTT_HUMIDITY_UID, "config" );
    mqttClient.publish(topic, localDoc);

}

void MQTTPublisher::publishTemp(float temp, float hum) {
    char localDoc[200];
    char topic[50];
    sprintf(topic, "%s/%s/%s",MQTT_BASE_TOPIC ,MQTT_SENSOR_NAME, "state" );

    jsonDoc.clear();
    jsonDoc["temperature"] = (int) (temp * 100 + 0.5) / 100.0; // NOLINT(bugprone-incorrect-roundings)
    jsonDoc["humidity"] = hum;
    serializeJson(jsonDoc, localDoc);

    mqttClient.publish(topic, localDoc);
}