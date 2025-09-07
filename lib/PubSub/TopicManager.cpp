#include "PubSub.h"

using PubSub::TopicManager;

TopicManager::TopicManager(PubSubClientPtr client) : WithClient(client) {
    client->setServer(mqtt_server, mqtt_port);
    client->setKeepAlive(60);
    client->setCallback([this](char *route, uint8_t *payload, unsigned int length) {
        String msg = parsePayload(payload, length);
        for (TopicPtr topic : topics) {
            if (topic->shouldPreccesEvent(route)){
                topic->onSubReciveEvent(msg.c_str());
            }
        }
        logMqtt(route, msg);
    });
}

void TopicManager::attachTopic(std::shared_ptr<Topic> topic) {
    topics.push_back(topic);
}

void TopicManager::logMqtt(const char *route, const String &msg, bool error) {
    Serial.printf("STATE [%s] <= %s", route, msg.c_str());
    if (error) {
        client->publish(TOPIC_ERROR, msg.c_str());
    }
    client->publish(TOPIC_LOG, msg.c_str());
}

String TopicManager::parsePayload(uint8_t *payload, unsigned int length) {
    String msg;
    msg.reserve(length);
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    msg.trim();
    msg.toLowerCase();
    return msg;
}
