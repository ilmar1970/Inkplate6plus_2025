#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>       // MQTT client
#include "secrets.h"
#include "Display.h"
//#include "air_ota.h"
#include "Synctime.h"


#define TOPIC_LOG "inkplate/log"
#define TOPIC_ERROR "inkplate/last_error"
#define TOPIC_IP "inkplate/ip"
#define TOPIC_SUB "inkplate/control/+/state"
#define TOPIC_BOOSTER "inkplate/control/booster"
#define TOPIC_CELL "inkplate/control/cell"
#define TOPIC_STARLINK "inkplate/control/starlink"
#define TOPIC_AUX1 "inkplate/control/ERlight"
#define TOPIC_AUX2 "inkplate/control/ERfan"
#define TOPIC_AUX3 "inkplate/control/aux3"
//int loop_counter = 0;

// --- Backlight control ---
constexpr uint32_t BACKLIGHT_TIMEOUT_MS = 60000; // 60s (adjust as you like)
constexpr uint8_t  FRONTLIGHT_ON_LEVEL  = 5;
constexpr uint8_t  FRONTLIGHT_OFF_LEVEL = 0;

static uint32_t lastInteractionMs = 0;
static bool     backlightOn       = true;
static uint32_t wakeGuardUntilMs  = 0; // swallow touches right after wake

Inkplate display(INKPLATE_1BIT);
Display::Text title("SeaEsta", {400, 80}, 2);
Display::Toggle wifi_toggle("WiFi Booster", TOPIC_BOOSTER, {100, 200});
Display::Toggle cell_toggle("Cell", TOPIC_CELL, {100, 400});
Display::Toggle starlink_toggle("Starlink", TOPIC_STARLINK, {100, 600});
Display::Toggle aux1_toggle("ERlight", TOPIC_AUX1, {600, 200});
Display::Toggle aux2_toggle("ERfan", TOPIC_AUX2, {600, 400});
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

// void setup_wifi() {
//   log_mqtt("Connecting to WiFi");
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//   }
//   log_mqtt("WiFi connected. IP address: " + WiFi.localIP().toString());
// }

void setup_wifi() {
  log_mqtt("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(250);
  }
  if (WiFi.status() == WL_CONNECTED) {
    log_mqtt("WiFi connected. IP address: " + WiFi.localIP().toString());
  } else {
    log_mqtt("WiFi connect timeout", /*error=*/true);
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length);
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  // normalize
  String t = topic;
  msg.trim(); msg.toLowerCase();
  bool on = (msg == "1" || msg == "true" || msg == "on");

  // Route by exact /state topic
  if (t == String(TOPIC_BOOSTER) + "/state")       { on ? wifi_toggle.enable()     : wifi_toggle.disable(); }
  else if (t == String(TOPIC_CELL) + "/state")     { on ? cell_toggle.enable()     : cell_toggle.disable(); }
  else if (t == String(TOPIC_STARLINK) + "/state") { on ? starlink_toggle.enable() : starlink_toggle.disable(); }
  else if (t == String(TOPIC_AUX1) + "/state")     { on ? aux1_toggle.enable()     : aux1_toggle.disable(); }
  else if (t == String(TOPIC_AUX2) + "/state")     { on ? aux2_toggle.enable()     : aux2_toggle.disable(); }
  else if (t == String(TOPIC_AUX3) + "/state")     { on ? aux3_toggle.enable()     : aux3_toggle.disable(); }

  // Optional: small log
  char logEntry[128];
  snprintf(logEntry, sizeof(logEntry), "STATE [%s] <= %s", topic, msg.c_str());
  log_mqtt(logEntry);
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(client_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(TOPIC_SUB);
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
    display.clearDisplay();
    display.frontlight(true);
    display.setFrontlight(FRONTLIGHT_ON_LEVEL);
    backlightOn = true;
    lastInteractionMs = millis();   
    display.display();
    delay(3000);
    Display::display = &display;
    if (display.tsInit(true)){
        Serial.println("TS: success");
    } else {
        Serial.println("TS: fail");
    }
}


// Turn on frontlight on first tap; return true if we just woke
inline bool wakeOnAnyTap() {
    // Full-screen touch area (Inkplate 6: 1024x758)
    if (!backlightOn && display.touchInArea(0, 0, 1024, 758)) {
        display.setFrontlight(FRONTLIGHT_ON_LEVEL);
        backlightOn = true;
        lastInteractionMs = millis();
        wakeGuardUntilMs = lastInteractionMs + 300;   // 300ms swallow window
        return true;
    }
    return false;
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
    // topic is the command topic, e.g. "inkplate/control/aux1"
    toggle.onClickListener = [topic](Display::Toggle* t) {
        // If we just woke the light, ignore this press
        if (millis() < wakeGuardUntilMs) return;

        lastInteractionMs = millis(); // real interaction happened

        // invert current UI state -> command payload
        const char* payload = t->state ? "0" : "1";

        // publish command (NOT retained)
        bool ok = client.publish(topic, payload, /*retain=*/false);

        // Optional: log command
        String s = String("CMD [") + topic + "] => " + payload + (ok ? "" : " (fail)");
        log_mqtt(s.c_str());

        // DO NOT change UI here. Wait for "<topic>/state" to arrive and set UI.
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
  initDisplay();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //air_ota();
  //Synctime::display = &display;
  //synctime.setTime();
  drawNetPage();
  //synctime.getRtcDate();
  waitClick();
  display.display();
}

void loop() {
  // Wake on any tap if light is off (and swallow that tap)
  if (wakeOnAnyTap()) {
      // Skip processing toggles this iteration to avoid accidental toggle on wake
      return;
  }

  // Auto turn off after inactivity
  if (backlightOn && (millis() - lastInteractionMs > BACKLIGHT_TIMEOUT_MS)) {
      display.setFrontlight(FRONTLIGHT_OFF_LEVEL);
      backlightOn = false;
  }
    //ArduinoOTA.handle();

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
  // if (display.rtcCheckAlarmFlag()) // update time every minute
  //   {
  //       Synctime::display = &display;
  //       synctime.getRtcDate();
  //       Serial.println("Update min");
  //       loop_counter++;
  //   }
}