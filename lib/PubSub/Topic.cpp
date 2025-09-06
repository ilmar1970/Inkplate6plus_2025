#include "PubSub.h"

using PubSub::Topic;

Topic::Topic(PubSubClientPtr client, const char* topicName) : WithClient(client) {
    this->topicName = topicName;
}

void Topic::callback() {

}