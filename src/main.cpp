#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>       // MQTT client
#include "secrets.h"
#include "Display.h"
#include "air_ota.h"
#include "Synctime.h"


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


Inkplate display(INKPLATE_1BIT);
Display::Text title("SeaEsta", {400, 80}, 2);
Display::Toggle wifi_toggle("WiFi Booster", TOPIC_BOOSTER, {100, 200});
Display::Toggle cell_toggle("Cell", TOPIC_CELL, {100, 400});
Display::Toggle starlink_toggle("Starlink", TOPIC_STARLINK, {100, 600});
Display::Toggle aux1_toggle("AUX1", TOPIC_AUX1, {600, 200});
Display::Toggle aux2_toggle("AUX2", TOPIC_AUX2, {600, 400});
Display::Toggle aux3_toggle("AUX3", TOPIC_AUX3, {600, 600});

WiFiClient espClient;
PubSubClient client(espClient);
Synctime synctime;

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

  if (String(topic) == TOPIC_BOOSTER "/state") {
    state ? wifi_toggle.enable() : wifi_toggle.disable();
  } else if (String(topic) == TOPIC_CELL "/state") {
    state ? cell_toggle.enable() : cell_toggle.disable();
  } else if (String(topic) == TOPIC_STARLINK "/state") {
    state ? starlink_toggle.enable() : starlink_toggle.disable();
  } else if (String(topic) == TOPIC_AUX1 "/state") {
    state ? aux1_toggle.enable() : aux1_toggle.disable();
  } else if (String(topic) == TOPIC_AUX2 "/state") {
    state ? aux2_toggle.enable() : aux2_toggle.disable();
  } else if (String(topic) == TOPIC_AUX3 "/state") {
    state ? aux3_toggle.enable() : aux3_toggle.disable();
  } 
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

void initDisplay(){
    // display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    // display.begin();
    // display.frontlight(true);
    // display.setFrontlight(3);
    // display.clearDisplay();
    // Display::display = &display;
    // if (display.tsInit(true)){
    //     Serial.println("TS: success");
    // } else {
    //     Serial.println("TS: fail");
    // }
}

void drawNetPage(){
    title.draw();
    wifi_toggle.draw();
    cell_toggle.draw();
    starlink_toggle.draw();
    aux1_toggle.draw();
    aux2_toggle.draw();
    aux3_toggle.draw();
    display.display();
}

void setupToggleListener(Display::Toggle& toggle, const char* topic) {
    toggle.onClickListener = [topic](Display::Toggle* t) {
        if (t->state) {
            // Disable redraw toggle, wait for <topic>/state to be received
            //t->disable(); 
            client.publish(topic, "0");
        } else {
            // Disable redraw toggle, wait for <topic>/state to be received
            //t->enable();
            client.publish(topic, "1");
        }
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

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //elegant_ota();
  initDisplay();
  Synctime::display = &display;
  synctime.setTime();
  drawNetPage();
  synctime.getRtcDate();
  waitClick();

}

void loop() {
  ArduinoOTA.handle();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  wifi_toggle.readCheckState();
  cell_toggle.readCheckState();
  starlink_toggle.readCheckState();
  aux1_toggle.readCheckState();
  aux2_toggle.readCheckState();
  aux3_toggle.readCheckState();
  if (display.rtcCheckAlarmFlag()) // update time every minute
    {
        Synctime::display = &display;
        synctime.getRtcDate();
        Serial.println("Update min");
        //loop_counter++;
    }
}