#include "PubSub.h"

using PubSub::TopicManager;

TopicManager::TopicManager(PubSubClientPtr client) : WithClient(client) {}

void TopicManager::attachTopic(std::shared_ptr<Topic> topic) {
    topics.push_back(topic);
}
