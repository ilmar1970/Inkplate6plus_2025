#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>       // MQTT client
#include "secrets.h"
#include "Display.h"
#include <Inkplate.h>          // Inkplate library
#include "page2.h" 
//#include "Water_60x60.h" 
//#include "Diesel_60x60.h" 

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

// --- Backlight control ---
constexpr uint32_t BACKLIGHT_TIMEOUT_MS = 60000; // 60s (adjust as you like)
constexpr uint8_t  FRONTLIGHT_ON_LEVEL  = 4;
constexpr uint8_t  FRONTLIGHT_OFF_LEVEL = 0;

static uint32_t lastInteractionMs = 0;
static bool     backlightOn       = true;
static uint32_t wakeGuardUntilMs  = 0; // swallow touches right after wake
//bool nextPage = false; // page number
enum Page { PAGE_MAIN, PAGE_TWO };
static Page currentPage = PAGE_MAIN;

Inkplate display(INKPLATE_1BIT);
Display::Text title("SeaEsta", {400, 80}, 2);
Display::Toggle wifi_toggle("Booster", TOPIC_BOOSTER, {100, 200});
Display::Toggle cell_toggle("Cell", TOPIC_CELL, {100, 400});
Display::Toggle starlink_toggle("Starlink", TOPIC_STARLINK, {100, 600});
Display::Toggle aux1_toggle("BatLight", TOPIC_AUX1, {600, 200});
Display::Toggle aux2_toggle("BatFan", TOPIC_AUX2, {600, 400});
Display::Toggle aux3_toggle("AUX3", TOPIC_AUX3, {600, 600});

WiFiClient espClient;
PubSubClient client(espClient);

void log_mqtt(const String& msg, bool error=false) {
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
    Display::display = &display;
    delay(1000);
    display.clearDisplay();
    display.frontlight(true);
    display.setFrontlight(FRONTLIGHT_ON_LEVEL);
    Serial.println("TS init: ");
    while (!display.tsInit(true)){
      Serial.print('.');
      delay(1000);
    } 
    Serial.println("TS init: ok");backlightOn = true;
    lastInteractionMs = millis();
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
  client.setKeepAlive(60);
  client.setCallback(callback);
  if (WiFi.status() == WL_CONNECTED) {
    log_mqtt("WiFi connected. IP address: " + WiFi.localIP().toString());
  }
  drawNetPage();
  waitClick();
}


void drawNextPage() {
    // Header
    display.setFont(&FreeSansBold12pt7b);
    display.setTextColor(BLACK, WHITE);   // 1-bit: BLACK on WHITE
    display.setTextSize(1);
    display.setCursor(420, 60);
    display.print("PAGE 2");

    // Draw the two bitmaps in 1-bit mode
    // (If these arrays came from image2cpp as 1-bit, this Just Works)
    display.drawBitmap(bitmap1_x, bitmap1_y, bitmap1_content, bitmap1_w, bitmap1_h, BLACK);
    display.drawBitmap(bitmap0_x, bitmap0_y, bitmap0_content, bitmap0_w, bitmap0_h, BLACK);

    // Four panels (outlined rounded rectangles) in BLACK
    const int r = 8;
    display.drawRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, r, BLACK);
    display.drawRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, r, BLACK);
    display.drawRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, r, BLACK);
    display.drawRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, r, BLACK);

    // Labels (all in BLACK)
    display.setFont(text2_font);
    display.setCursor(text2_cursor_x, text2_cursor_y); display.print(text2_content);

    display.setFont(text5_font);
    display.setCursor(text5_cursor_x, text5_cursor_y); display.print(text5_content);

    display.setFont(text3_font);
    display.setCursor(text3_cursor_x, text3_cursor_y); display.print(text3_content);

    display.setFont(text4_font);
    display.setCursor(text4_cursor_x, text4_cursor_y); display.print(text4_content);

    // Page indicator: outline first dot, fill second
    display.drawCircle(circle0_center_x, circle0_center_y, circle0_radius, BLACK);
    display.fillCircle(circle1_center_x, circle1_center_y, circle1_radius, BLACK);

    // Commit to panel
    display.display();
}


void showMainPage() { display.clearDisplay(); drawNetPage(); }


void showPage2()    { display.clearDisplay(); drawNextPage(); } // already clears

void changePage(){
    lastInteractionMs = millis();   // count as activity
    currentPage = (currentPage == PAGE_MAIN) ? PAGE_TWO : PAGE_MAIN;
    Serial.println("Two fingers - switch page");
    if (currentPage == PAGE_MAIN) showMainPage();
    else                          showPage2();
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
      display.setFrontlight(FRONTLIGHT_OFF_LEVEL);
      backlightOn = false;
  }
  
  std::pair<DisplayCoordinates*, uint16_t> touchRecord = Display::readTouchData();
  if (touchRecord.second > 1) {
    changePage();
  } else if (touchRecord.second == 1 && currentPage == PAGE_MAIN) {
    wifi_toggle.readCheckState(touchRecord.first[0]);
    cell_toggle.readCheckState(touchRecord.first[0]);
    starlink_toggle.readCheckState(touchRecord.first[0]);
    aux1_toggle.readCheckState(touchRecord.first[0]);
    aux2_toggle.readCheckState(touchRecord.first[0]);
    aux3_toggle.readCheckState(touchRecord.first[0]);
  }
}
