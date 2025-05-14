#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>       // MQTT client
#include "secrets.h"
#include "Display.h"
#include "air_ota.h"
#include "Synctime.h"
#include "resourses.h"


#define TOPIC_LOG "inkplate/log"
#define TOPIC_ERROR "inkplate/last_error"
#define TOPIC_IP "inkplate/ip"
#define TOPIC_SUB "inkplate/control/+/state"
#define TOPIC_BOOSTER "inkplate/control/booster"
#define TOPIC_CELL "inkplate/control/cell"
#define TOPIC_STARLINK "inkplate/control/starlink"
#define TOPIC_AUX1 "inkplate/control/aux1"
#define TOPIC_AUX2 "inkplate/control/aux2"
#define TOPIC_AUX3 "inkplate/control/aux3"


WiFiClient espClient;
PubSubClient client(espClient);
Synctime synctime;

Display::Page togglePage{};
Display::Page barPage{};
Display::Page* activePage;

void log_mqtt(const String& msg, bool error=false) {
  if (error) {
    client.publish(TOPIC_ERROR, msg.c_str());
  }
  client.publish(TOPIC_LOG, msg.c_str());
}

void setup_wifi() {
  log_mqtt("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  log_mqtt("WiFi connected. IP address: " + WiFi.localIP().toString());
}

// Callback when a message is received
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  char logEntry[128]; // Adjust size as needed
  snprintf(logEntry, sizeof(logEntry), "MQTT message on [%s]: %s", topic, msg.c_str());
  log_mqtt(logEntry);

  msg.toLowerCase();
  msg.trim();
  bool state = (msg == "1" || msg == "true" || msg == "on");

  //iterate activePage objects evaluate trigger condition on mqtt listeners

  // if (String(topic) == TOPIC_BOOSTER "/state") {
  //   state ? wifi_toggle.enable() : wifi_toggle.disable();
  // } else if (String(topic) == TOPIC_CELL "/state") {
  //   state ? cell_toggle.enable() : cell_toggle.disable();
  // } else if (String(topic) == TOPIC_STARLINK "/state") {
  //   state ? starlink_toggle.enable() : starlink_toggle.disable();
  // } else if (String(topic) == TOPIC_AUX1 "/state") {
  //   state ? aux1_toggle.enable() : aux1_toggle.disable();
  // } else if (String(topic) == TOPIC_AUX2 "/state") {
  //   state ? aux2_toggle.enable() : aux2_toggle.disable();
  // } else if (String(topic) == TOPIC_AUX3 "/state") {
  //   state ? aux3_toggle.enable() : aux3_toggle.disable();
  // } 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(client_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(TOPIC_SUB);
      client.subscribe("topib");
      IPAddress ip = WiFi.localIP();
      String ipStr = ip.toString();
      client.publish(TOPIC_IP, ipStr.c_str());
      log_mqtt("Connected to MQTT my IP: " + ipStr);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(". Trying again in 5s");
      delay(5000);
    }
  }
}

void setupTogglePage() {
  Display::Text title("SeaEsta", {400, 80}, 2);
  Display::Toggle wifiToggle("WiFi Booster", TOPIC_BOOSTER, {100, 200});
  Display::Toggle cellToggle("Cell", TOPIC_CELL, {100, 400});
  Display::Toggle starlinkToggle("Starlink", TOPIC_STARLINK, {100, 600});
  Display::Toggle aux1Toggle("AUX1", TOPIC_AUX1, {600, 200});
  Display::Toggle aux2Toggle("AUX2", TOPIC_AUX2, {600, 400});
  Display::Toggle aux3Toggle("AUX3", TOPIC_AUX3, {600, 600});

  togglePage.attachObject(&title);
  togglePage.attachObject(&wifiToggle);
  togglePage.attachObject(&cellToggle);
  togglePage.attachObject(&starlinkToggle);
  togglePage.attachObject(&aux1Toggle);
  togglePage.attachObject(&aux2Toggle);
  togglePage.attachObject(&aux3Toggle);
}

void setupBarPage() {
  Display::Icon waterIcon(waterIconBitmap, {700, 68}, {60, 60});
  Display::Icon fuelIcon(waterIconBitmap, {214, 69}, {60, 60});
  Display::Text titlePW("PORT", {127, 220}, 2);
  Display::Text titleSW("STRB", {300, 220}, 2);
  Display::Text titlePF("PORT", {613, 220}, 2);
  Display::Text titleSF("STRB", {795, 220}, 2);
  Display::Bar waterPort({270, 230}, {405, 490});
  Display::Bar waterStrb({100, 230}, {236, 490});
  Display::Bar fuelPort({579, 230}, {719, 490});
  Display::Bar fuelStrb({772, 230}, {904, 490});

  //maybe use icon for page switching?
  barPage.attachObject(&waterIcon);
  barPage.attachObject(&fuelIcon);
  barPage.attachObject(&titlePW);
  barPage.attachObject(&titleSW);
  barPage.attachObject(&titlePF);
  barPage.attachObject(&titleSF);
  barPage.attachObject(&waterPort);
  barPage.attachObject(&waterStrb);
  barPage.attachObject(&fuelPort);
  barPage.attachObject(&fuelStrb);
}

// void setupToggleListener(Display::Toggle& toggle, const char* topic) {
//     toggle.onClickListener = [topic](Display::Toggle* t) {
//         if (t->state) {
//             // Disable redraw toggle, wait for <topic>/state to be received
//             //t->disable(); 
//             client.publish(topic, "0");
//         } else {
//             // Disable redraw toggle, wait for <topic>/state to be received
//             //t->enable();
//             client.publish(topic, "1");
//         }
//     };
// }

// void waitClick() {
//     setupToggleListener(wifi_toggle, wifi_toggle.name);
//     setupToggleListener(cell_toggle, cell_toggle.name);
//     setupToggleListener(starlink_toggle, starlink_toggle.name);
//     setupToggleListener(aux1_toggle, aux1_toggle.name);
//     setupToggleListener(aux2_toggle, aux2_toggle.name);
//     setupToggleListener(aux3_toggle, aux3_toggle.name);
// }

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  setupTogglePage();
  setupBarPage();
  Synctime::display = activePage->display; // maybe make synctime part of page object makes more sence
  synctime.setTime();
  synctime.getRtcDate();
  // waitClick();
  togglePage.draw();
}

void loop() {
  ArduinoOTA.handle();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // need activePage loop action similar in function to client.loop();

  // wifi_toggle.readCheckState();
  // cell_toggle.readCheckState();
  // starlink_toggle.readCheckState();
  // aux1_toggle.readCheckState();
  // aux2_toggle.readCheckState();
  // aux3_toggle.readCheckState();
  // if (display.rtcCheckAlarmFlag()) {
  //   Synctime::display = &display;
  //   synctime.getRtcDate();
  //   Serial.println("Update min");
  //   //loop_counter++;
  // }
}