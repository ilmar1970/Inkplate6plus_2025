#include "PubSub.h"

using PubSub::WithClient;

WithClient::WithClient(PubSubClientPtr client) {
    this->client = client;
}
