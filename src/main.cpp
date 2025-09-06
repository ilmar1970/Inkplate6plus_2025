#include <PubSubClient.h>
#include <WiFi.h>

#include "Display.h"
#include "Synctime.h"
#include "air_ota.h"
#include "resourses.h"
#include "secrets.h"
#include <PubSub.h>

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

InkplatePtr display;
PagePtr togglePage;
PagePtr barPage;
PagePtr activePage;

PubSubClientPtr client;

// --- Backlight control ---
constexpr uint32_t BACKLIGHT_TIMEOUT_MS = 60000;  // 60s (adjust as you like)
constexpr uint8_t FRONTLIGHT_ON_LEVEL = 4;
constexpr uint8_t FRONTLIGHT_OFF_LEVEL = 0;

static uint32_t lastInteractionMs = 0;
static bool backlightOn = true;
static uint32_t wakeGuardUntilMs = 0;  // swallow touches right after wake
// bool nextPage = false; // page number
int fuelPort = 0;
int fuelStb = 0;
int waterPort = 0;
int waterStb = 0;

void setupTogglePage() {
    TextPtr title = TextPtr(new Display::Text(display, "SeaEsta", {400, 80}, 2, &FreeSans18pt7b));
    TogglePtr wifiToggle = TogglePtr(new Display::Toggle(display, "WiFi Booster", TOPIC_BOOSTER, {100, 200}, 1, &FreeSans18pt7b, false));
    TogglePtr cellToggle = TogglePtr(new Display::Toggle(display, "Cell", TOPIC_CELL, {100, 400}, 1, &FreeSans18pt7b, false));
    TogglePtr starlinkToggle = TogglePtr(new Display::Toggle(display, "Starlink", TOPIC_STARLINK, {100, 600}, 1, &FreeSans18pt7b, false));
    TogglePtr aux1Toggle = TogglePtr(new Display::Toggle(display, "AUX1", TOPIC_AUX1, {600, 200}, 1, &FreeSans18pt7b, false));
    TogglePtr aux2Toggle = TogglePtr(new Display::Toggle(display, "AUX2", TOPIC_AUX2, {600, 400}, 1, &FreeSans18pt7b, false));
    TogglePtr aux3Toggle = TogglePtr(new Display::Toggle(display, "AUX3", TOPIC_AUX3, {600, 600}, 1, &FreeSans18pt7b, false));

    TopicPtr wifiTopic = TopicPtr(new PubSub::Topic(client, TOPIC_BOOSTER));
    TopicPtr cellTopic = TopicPtr(new PubSub::Topic(client, TOPIC_CELL));
    TopicPtr starlinkTopic = TopicPtr(new PubSub::Topic(client, TOPIC_STARLINK));
    TopicPtr aux1Topic = TopicPtr(new PubSub::Topic(client, TOPIC_AUX1));
    TopicPtr aux2Topic = TopicPtr(new PubSub::Topic(client, TOPIC_AUX2));
    TopicPtr aux3Topic = TopicPtr(new PubSub::Topic(client, TOPIC_AUX3));

    setupToggleListener(wifi_toggle, wifi_toggle.name);
    setupToggleListener(cell_toggle, cell_toggle.name);
    setupToggleListener(starlink_toggle, starlink_toggle.name);
    setupToggleListener(aux1_toggle, aux1_toggle.name);
    setupToggleListener(aux2_toggle, aux2_toggle.name);
    setupToggleListener(aux3_toggle, aux3_toggle.name);

    togglePage->attachObject(title);
    togglePage->attachObject();
    togglePage->attachObject();
    togglePage->attachObject();
    togglePage->attachObject();
    togglePage->attachObject();
    togglePage->attachObject();
}

// void setupBarPage() {
//   Display::Icon waterIcon(waterIconBitmap, {700, 68}, {60, 60});
//   Display::Icon fuelIcon(waterIconBitmap, {214, 69}, {60, 60});
//   Display::Text titlePW("PORT", {127, 220}, 2);
//   Display::Text titleSW("STRB", {300, 220}, 2);
//   Display::Text titlePF("PORT", {613, 220}, 2);
//   Display::Text titleSF("STRB", {795, 220}, 2);
//   Display::Bar waterPort({270, 230}, {405, 490});
//   Display::Bar waterStrb({100, 230}, {236, 490});
//   Display::Bar fuelPort({579, 230}, {719, 490});
//   Display::Bar fuelStrb({772, 230}, {904, 490});

//   barPage.attachObject(&waterIcon);
//   barPage.attachObject(&fuelIcon);
//   barPage.attachObject(&titlePW);
//   barPage.attachObject(&titleSW);
//   barPage.attachObject(&titlePF);
//   barPage.attachObject(&titleSF);
//   barPage.attachObject(&waterPort);
//   barPage.attachObject(&waterStrb);
//   barPage.attachObject(&fuelPort);
//   barPage.attachObject(&fuelStrb);
// }

bool wakeOnAnyTap() {
    if (!backlightOn && display->touchInArea(0, 0, 1024, 758)) {
        display->setFrontlight(FRONTLIGHT_ON_LEVEL);
        backlightOn = true;
        lastInteractionMs = millis();
        wakeGuardUntilMs = lastInteractionMs + 300;
        return true;
    }
    return false;
}

void changePage() {
    lastInteractionMs = millis();
    activePage = (activePage == togglePage) ? barPage : togglePage;
    activePage->draw();
}

void log_mqtt(const String &msg, bool error = false) {
    if (error) {
        client.publish(TOPIC_ERROR, msg.c_str());
    }
    client.publish(TOPIC_LOG, msg.c_str());
}

void setup_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
    String msg;
    msg.reserve(length);
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }

    // normalize
    String t = topic;
    msg.trim();
    msg.toLowerCase();
    bool on = (msg == "1" || msg == "true" || msg == "on");

    // Route by exact /state topic
    if (t == String(TOPIC_BOOSTER) + "/state") {
        on ? wifi_toggle.enable() : wifi_toggle.disable();
    } else if (t == String(TOPIC_CELL) + "/state") {
        on ? cell_toggle.enable() : cell_toggle.disable();
    } else if (t == String(TOPIC_STARLINK) + "/state") {
        on ? starlink_toggle.enable() : starlink_toggle.disable();
    } else if (t == String(TOPIC_AUX1) + "/state") {
        on ? aux1_toggle.enable() : aux1_toggle.disable();
    } else if (t == String(TOPIC_AUX2) + "/state") {
        on ? aux2_toggle.enable() : aux2_toggle.disable();
    } else if (t == String(TOPIC_AUX3) + "/state") {
        on ? aux3_toggle.enable() : aux3_toggle.disable();
    } else if (t == "tanks/fuelPort") {
        fuelPort = msg.toInt();
        Serial.println(fuelPort);
    } else if (t == "tanks/fuelStb") {
        fuelStb = msg.toInt();
        Serial.println(fuelStb);
    } else if (t == "tanks/waterPort") {
        waterPort = msg.toInt();
        Serial.println(waterPort);
    } else if (t == "tanks/waterStb") {
        waterStb = msg.toInt();
        Serial.println(waterStb);
    }

    // Optional: small log
    char logEntry[128];
    snprintf(logEntry, sizeof(logEntry), "STATE [%s] <= %s", topic, msg.c_str());
    log_mqtt(logEntry);
}

void setupToggleListener(Display::Toggle &toggle, const char *topic) {
    toggle.onClickListener = [topic](Display::Toggle *t) {
        if (millis() < wakeGuardUntilMs) return;
        lastInteractionMs = millis();
        const char *payload = t->state ? "0" : "1";
        bool ok = client->publish(topic, payload, false);
        String s = String("CMD [") + topic + "] => " + payload + (ok ? "" : " (fail)");
        log_mqtt(s.c_str());
    };
}

void waitClick() {
    setupToggleListener(wifi_toggle, wifi_toggle.name);
    setupToggleListener(cell_toggle, cell_toggle.name);
    setupToggleListener(starlink_toggle, starlink_toggle.name);
    setupToggleListener(aux1_toggle, aux1_toggle.name);
    setupToggleListener(aux2_toggle, aux2_toggle.name);
    setupToggleListener(aux3_toggle, aux3_toggle.name);
}

void reconnect() {
    while (!client->connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client->connect(client_id, mqtt_user, mqtt_pass)) {
            Serial.println("connected");
            client->subscribe(TOPIC_SUB);
            client->subscribe(TOPIC_TANK);
            IPAddress ip = WiFi.localIP();
            String ipStr = ip.toString();
            client->publish(TOPIC_IP, ipStr.c_str());
            log_mqtt("Connected to MQTT my IP: " + ipStr);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client->state());
            Serial.println(". Trying again in 5s");
            delay(5000);
        }
    }
}

void networkSetup() {
    WiFiClient espClient;
    client = PubSubClientPtr(new PubSubClient(espClient));
}

void setup() {
    Serial.begin(115200);

    setup_wifi();
    client->setServer(mqtt_server, mqtt_port);
    client->setKeepAlive(60);
    client->setCallback(callback);
    if (WiFi.status() == WL_CONNECTED) {
        log_mqtt("WiFi connected. IP address: " + WiFi.localIP().toString());
    }
    waitClick();

    display = InkplatePtr(new Inkplate(INKPLATE_1BIT));
    togglePage = PagePtr(new Display::Page(display));
    // barPage = new Display::Page(display);

    setupTogglePage();
    // setupBarPage();

    Serial.println("Calling togglePage->draw()...");
    togglePage->draw();
    Serial.println("togglePage->draw() finished");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi reconnect ...");
        WiFi.reconnect();
        delay(500);
        return;
    }
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    if (wakeOnAnyTap()) {
        // Skip processing toggles this iteration to avoid accidental toggle on wake
        return;
    }
    // Auto turn off after inactivity
    if (backlightOn && (millis() - lastInteractionMs > BACKLIGHT_TIMEOUT_MS)) {
        display->setFrontlight(FRONTLIGHT_OFF_LEVEL);
        backlightOn = false;
    }

    std::pair<std::vector<DisplayCoordinates>, uint16_t> touchRecord = activePage->readTouchData();
    if (touchRecord.second > 1) {
        changePage();
    } else if (touchRecord.second == 1 && activePage == togglePage) {
        wifi_toggle.readCheckState(touchRecord.first[0]);
        cell_toggle.readCheckState(touchRecord.first[0]);
        starlink_toggle.readCheckState(touchRecord.first[0]);
        aux1_toggle.readCheckState(touchRecord.first[0]);
        aux2_toggle.readCheckState(touchRecord.first[0]);
        aux3_toggle.readCheckState(touchRecord.first[0]);
    }
}