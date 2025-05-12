#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>       // MQTT client
//#include "air_ota.h"
#include "secrets.h"
#include "Display.h"

#define TOPIC_LOG "inkplate/log"
#define TOPIC_ERROR "inkplate/last_error"
#define TOPIC_IP "inkplate/ip"
#define TOPIC_SUB "inkplate/control/#"


Inkplate display(INKPLATE_1BIT);
Display::Text title("SeaEsta", {400, 80}, 2);
Display::Toggle wifi_toggle("WiFi Booster", "ether5", {100, 200});
Display::Toggle cell_toggle("Cell", "ether4", {100, 400});
Display::Toggle starlink_toggle("Starlink", "ether1", {100, 600});
Display::Toggle aux1_toggle("AUX1", "AUX1", {600, 200});
Display::Toggle aux2_toggle("AUX2", "AUX2", {600, 400});
Display::Toggle aux3_toggle("AUX3", "AUX3", {600, 600});

WiFiClient espClient;
PubSubClient client(espClient);

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
  String logEntry = "MQTT message on [" + String(topic) + "]: " + msg;
  log_mqtt(logEntry);

  if (String(topic) == "inkplate/control/booster/state") {
    msg ? wifi_toggle.enable() : wifi_toggle.disable();
  } else if (String(topic) == "inkplate/control/cell/state") {
    msg ? cell_toggle.enable() : cell_toggle.disable();
  } else if (String(topic) == "inkplate/control/starlink/state") {
    msg ? starlink_toggle.enable() : starlink_toggle.disable();
  } else if (String(topic) == "inkplate/control/aux1/state") {
    msg ? aux1_toggle.enable() : aux1_toggle.disable();
  } else if (String(topic) == "inkplate/control/aux2/state") {
    msg ? aux2_toggle.enable() : aux2_toggle.disable();
  } else if (String(topic) == "inkplate/control/aux3/state") {
    msg ? aux3_toggle.enable() : aux3_toggle.disable();
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
    display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();
    display.frontlight(true);
    display.setFrontlight(3);
    display.clearDisplay();
    Display::display = &display;
    if (display.tsInit(true)){
        Serial.println("TS: success");
    } else {
        Serial.println("TS: fail");
    }
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


void waitClick(){
    wifi_toggle.onClickListener = [](Display::Toggle* toggle) { //tmp
      if (toggle->state) {
        toggle->disable();
        client.publish("inkplate/control/wifi", "0");
      } else {
        toggle->enable();
        client.publish("inkplate/control/wifi", "1");
      }
    };
    cell_toggle.onClickListener = [](Display::Toggle* toggle) { //tmp
      if (toggle->state) {
        toggle->disable();
        client.publish("inkplate/control/cell", "0");
      } else {
        toggle->enable();
        client.publish("inkplate/control/cell", "1");
      }
    };
    starlink_toggle.onClickListener = [](Display::Toggle* toggle) { //tmp
      if (toggle->state) {
        toggle->disable();
        client.publish("inkplate/control/starlink", "0");
      } else {
        toggle->enable();
        client.publish("inkplate/control/starlink", "1");
      }
    };
    aux1_toggle.onClickListener = [](Display::Toggle* toggle) { //tmp
      if (toggle->state) {
        toggle->disable();
        client.publish("inkplate/control/aux1", "0");
      } else {
        toggle->enable();
        client.publish("inkplate/control/aux1", "1");
      }
    };
    aux2_toggle.onClickListener = [](Display::Toggle* toggle) { //tmp
      if (toggle->state) {
        toggle->disable();
        client.publish("inkplate/control/aux2", "0");
      } else {
        toggle->enable();
        client.publish("inkplate/control/aux2", "1");
      }
    }; 
    aux3_toggle.onClickListener = [](Display::Toggle* toggle) { //tmp
      if (toggle->state) {
        toggle->disable();
        client.publish("inkplate/control/aux3", "0");
      } else {
        toggle->enable();
        client.publish("inkplate/control/aux3", "1");
      }
    };  
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //elegant_ota();
  initDisplay();
  drawNetPage();
  waitClick(); 
}

void loop() {
  //ElegantOTA.loop();
  
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
}