#ifndef PUBSUB_H
#define PUBSUB_H

#include <vector>
#include <memory>

#include <Arduino.h>
#include <PubSubClient.h>
#include <Alias.h>

using PubSubClientPtr = std::shared_ptr<PubSubClient>;
using TopicPtr = std::shared_ptr<PubSub::Topic>;
using TopicManagerPtr = std::shared_ptr<PubSub::TopicManager>;

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

            void callback();

            Topic(PubSubClientPtr client, const char* topicName);
            virtual ~Topic() = default;
    };

    class TopicManager : WithClient {
        public:
            PointerArray<Topic> topics;

            void attachTopic(std::shared_ptr<Topic> topic);

            TopicManager(PubSubClientPtr client);
            virtual ~TopicManager() = default;
    };
}

#endif