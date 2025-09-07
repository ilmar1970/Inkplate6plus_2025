#include "PubSub.h"

using PubSub::Topic;

Topic::Topic(PubSubClientPtr client, const char* topicName) : WithClient(client) {
    this->topicName = topicName;
}

bool Topic::shouldPreccesEvent(const char* route) {
    return String(route) == String(topicName) + "/state";
}

void Topic::onSubReciveEvent(const char* msg, const OnReciveListener& customOnReciveListener) {
    customOnReciveListener ? customOnReciveListener(msg) : onReciveListener(msg);
}