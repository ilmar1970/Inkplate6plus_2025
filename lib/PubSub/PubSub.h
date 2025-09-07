#ifndef PUBSUB_H
#define PUBSUB_H

#include <vector>
#include <memory>

#include <Arduino.h>
#include <PubSubClient.h>
#include "Alias.h"
#include "secrets.h"

#define TOPIC_LOG "inkplate/log"
#define TOPIC_ERROR "inkplate/last_error"
#define TOPIC_IP "inkplate/ip"
#define TOPIC_SUB "inkplate/control/+/state"
#define TOPIC_BOOSTER "inkplate/control/booster"
#define TOPIC_CELL "inkplate/control/cell"
#define TOPIC_STARLINK "inkplate/control/starlink"
#define TOPIC_AUX1 "inkplate/control/batlight"
#define TOPIC_AUX2 "inkplate/control/batfan"
#define TOPIC_AUX3 "inkplate/control/aux3"
#define TOPIC_TANK "tanks/#"

using PubSubClientPtr = std::shared_ptr<PubSubClient>;
using OnReciveListener = OnEventListener<const char*>;

namespace PubSub {
    class WithClient {
        public:
            PubSubClientPtr client;
            WithClient(PubSubClientPtr client);
            virtual ~WithClient() = default;
    };

    class Topic : WithClient {
        public:
            const char* topicName;
            OnReciveListener onReciveListener;

            bool shouldPreccesEvent(const char* route);
            void onSubReciveEvent(const char* msg, const OnReciveListener& customOnReciveListener = nullptr);

            Topic(PubSubClientPtr client, const char* topicName);
            virtual ~Topic() = default;
    };

    class TopicManager : WithClient {
        public:
            PointerArray<Topic> topics;

            void attachTopic(std::shared_ptr<Topic> topic);
            void logMqtt(const char *topic, const String &msg, bool error = false);
            String parsePayload(uint8_t *payload, unsigned int length);

            TopicManager(PubSubClientPtr client);
            virtual ~TopicManager() = default;
    };
}

using TopicPtr = std::shared_ptr<PubSub::Topic>;
using TopicManagerPtr = std::shared_ptr<PubSub::TopicManager>;

#endif