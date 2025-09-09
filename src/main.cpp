#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>       // MQTT client
#include "secrets.h"
#include "Display.h"
#include <Inkplate.h>          // Inkplate library
#include "Page.h" 

#define TOPIC_LOG "inkplate/log"
#define TOPIC_ERROR "inkplate/last_error"
#define TOPIC_IP "inkplate/ip"
#define TOPIC_SUB "inkplate/control/+/state"
#define TOPIC_BOOSTER "inkplate/control/booster"
#define TOPIC_CELL "inkplate/control/cell"
#define TOPIC_STARLINK "inkplate/control/starlink"
#define TOPIC_B_LIGHT "inkplate/control/batlight"
#define TOPIC_B_FAN "inkplate/control/batfan"
#define TOPIC_DECKWASH "inkplate/control/deckwash"
#define TOPIC_TANK "tanks/#"
#define TOPIC_AC_PORT "inkplate/control/acport"
#define TOPIC_AC_STRB "inkplate/control/acstrb"

// --- Backlight control ---
constexpr uint32_t BACKLIGHT_TIMEOUT_MS = 60000; // 60s (adjust as you like)
constexpr uint8_t  FRONTLIGHT_ON_LEVEL  = 4;
constexpr uint8_t  FRONTLIGHT_OFF_LEVEL = 0;

static uint32_t lastInteractionMs = 0;
static bool     backlightOn       = true;
static uint32_t wakeGuardUntilMs  = 0; // swallow touches right after wake
//bool nextPage = false; // page number
enum Page2 { PAGE_MAIN, PAGE_TWO };
static Page2 currentPage = PAGE_MAIN;
int fuelPort = 0;
int fuelStb = 0;
int waterPort = 0;
int waterStb = 0;

// Example dynamic variables for each tank
int dp_percent = 75; // 0..100
int ds_percent = 50;
int wp_percent = 30;
int ws_percent = 90;
bool bilge1 = true;
bool bilge2 = false;
bool bilge3 = true;
bool bilge4 = false;
bool bilge5 = false;
bool bilge6 = false;

Inkplate display(INKPLATE_1BIT);
Page page(display);
Display::Text title("SeaEsta", {400, 80}, 2);
Display::Toggle wifi_toggle("Booster", TOPIC_BOOSTER, {100, 200});
Display::Toggle cell_toggle("Cell", TOPIC_CELL, {100, 400});
Display::Toggle starlink_toggle("Starlink", TOPIC_STARLINK, {100, 600});
Display::Toggle b_light("BatLight", TOPIC_B_LIGHT, {600, 200});
Display::Toggle b_fan("BatFan", TOPIC_B_FAN, {600, 400});
Display::Toggle deck_wash("AUX3", TOPIC_DECKWASH, {600, 600});

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
  if (strcmp(topic, TOPIC_BOOSTER "/state") == 0)       { on ? wifi_toggle.enable()     : wifi_toggle.disable(); }
  else if (strcmp(topic, TOPIC_CELL "/state") == 0)     { on ? cell_toggle.enable()     : cell_toggle.disable(); }
  else if (strcmp(topic, TOPIC_STARLINK "/state") == 0) { on ? starlink_toggle.enable() : starlink_toggle.disable(); }
  else if (strcmp(topic, TOPIC_B_LIGHT "/state") == 0)  { on ? b_light.enable()         : b_light.disable(); }
  else if (strcmp(topic, TOPIC_B_FAN "/state") == 0)    { on ? b_fan.enable()           : b_fan.disable(); }
  else if (strcmp(topic, TOPIC_DECKWASH "/state") == 0) { on ? deck_wash.enable()       : deck_wash.disable(); }
  else if (strcmp(topic, "tanks/fuelPort") == 0)        { fuelPort = msg.toInt(); Serial.println(fuelPort); }
  else if (strcmp(topic, "tanks/fuelStb") == 0)         { fuelStb = msg.toInt(); Serial.println(fuelStb); }
  else if (strcmp(topic, "tanks/waterPort") == 0)       { waterPort = msg.toInt(); Serial.println(waterPort); }
  else if (strcmp(topic, "tanks/waterStb") == 0)        { waterStb = msg.toInt(); Serial.println(waterStb); }

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
      client.subscribe(TOPIC_TANK);
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


// Helper to draw a tank panel
void drawTankPanel(int rect_a_x, int rect_a_y, int rect_b_x, int rect_b_y, int percent, bool bilge, int circle_x, int circle_y) {
    int w = rect_b_x - rect_a_x;
    int h = rect_b_y - rect_a_y;
    int fill_h = (h * percent) / 100;
    int fill_y = rect_b_y - fill_h;

    // Draw main rectangle outline
    display.drawRoundRect(rect_a_x, rect_a_y, w, h, 8, BLACK);

    // Draw filled black rectangle (percent fill, from bottom up)
    display.fillRect(rect_a_x, fill_y, w, fill_h, BLACK);

    // Draw label: just percent + " %"
    String text = String(percent) + " %";
    display.setFont(&FreeSansBold24pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(rect_a_x + 10, rect_a_y - 10); // Adjust as needed
    display.print(text);

    // Draw circle (bilge indicator)
    if (bilge)
        display.fillCircle(circle_x, circle_y, 30, BLACK);
    else
        display.drawCircle(circle_x, circle_y, 30, BLACK);
}


void drawNetPage(){
    title.draw();
    wifi_toggle.draw();
    cell_toggle.draw();
    starlink_toggle.draw();
    b_light.draw();
    b_fan.draw();
    deck_wash.draw();
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
    setupToggleListener(b_light, b_light.name);
    setupToggleListener(b_fan, b_fan.name);
    setupToggleListener(deck_wash, deck_wash.name);
}


void showMainPage() { 
    Serial.println("Main Page"); 
    display.clearDisplay(); 
    drawNetPage(); 
}

void showPage2() { 
    Serial.println("Page 2"); 
    display.clearDisplay();

    // Set tank values before drawing
    page.setTank(0, dp_percent);
    page.setTank(1, ds_percent);
    page.setTank(2, wp_percent);
    page.setTank(3, ws_percent);
    page.setBilge(0, bilge1);
    page.setBilge(1, bilge2);
    page.setBilge(2, bilge3);
    page.setBilge(3, bilge4);
    page.setBilge(4, bilge5);
    page.setBilge(5, bilge6);
    page.draw();
    display.display();
}


void changePage() {
    lastInteractionMs = millis();
    currentPage = (currentPage == PAGE_MAIN) ? PAGE_TWO : PAGE_MAIN;
    Serial.println("Two fingers - switch page");
    if (currentPage == PAGE_MAIN) showMainPage();
    else showPage2();
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
  showMainPage();   // <--- Use this instead of drawNetPage()
  waitClick();
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
    b_light.readCheckState(touchRecord.first[0]);
    b_fan.readCheckState(touchRecord.first[0]);
    deck_wash.readCheckState(touchRecord.first[0]);
  }
}
