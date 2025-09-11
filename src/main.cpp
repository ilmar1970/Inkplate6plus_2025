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
#define TOPIC_TANKS "tanks/#"
#define TOPIC_AC_PORT "inkplate/control/acport"
#define TOPIC_AC_STRB "inkplate/control/acstrb"
#define TOPIC_PUMPS "inkplate/pumps/#"

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

static uint32_t lastRedrawMs = 0;
constexpr uint32_t FULL_REDRAW_INTERVAL_MS = 300000; // 5 minutes

static uint32_t lastTempMs = 0;
constexpr uint32_t TEMP_INTERVAL_MS = 60000; // every 60 seconds

int fuelPort = 0;
int fuelStb = 0;
int waterPort = 0;
int waterStb = 0;

Inkplate display(INKPLATE_1BIT);
Page page(display);
Display::Text title("SeaEsta", {400, 80}, 2);
Display::Toggle wifi_toggle("Booster", TOPIC_BOOSTER, {100, 200});
Display::Toggle cell_toggle("Cell", TOPIC_CELL, {100, 350});
Display::Toggle starlink_toggle("Starlink", TOPIC_STARLINK, {100, 500});
Display::Toggle ac_port_toggle("AC_Fl_Port", TOPIC_AC_PORT, {100, 650});

Display::Toggle b_light("B_Light", TOPIC_B_LIGHT, {600, 200});
Display::Toggle b_fan("B_Fan", TOPIC_B_FAN, {600, 350});
Display::Toggle deck_wash("DeckFresh", TOPIC_DECKWASH, {600, 500});
Display::Toggle ac_strb_toggle("AC_Fl_Stb", TOPIC_AC_STRB, {600, 650});

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
    // --- Convert payload to lowercase, trimmed C-string ---
    char msg[32]; // Adjust size as needed for your payloads
    unsigned int copyLen = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
    memcpy(msg, payload, copyLen);
    msg[copyLen] = '\0';

    // Trim leading/trailing whitespace
    char* start = msg;
    while (*start && isspace((unsigned char)*start)) ++start;
    char* end = msg + strlen(msg) - 1;
    while (end > start && isspace((unsigned char)*end)) *end-- = '\0';

    // Lowercase payload
    for (char* p = start; *p; ++p) *p = tolower((unsigned char)*p);

    // --- Convert topic to lowercase C-string for case-insensitive matching ---
    char t[64]; // Adjust size as needed for your topics
    strncpy(t, topic, sizeof(t) - 1);
    t[sizeof(t) - 1] = '\0';
    for (char* p = t; *p; ++p) *p = tolower((unsigned char)*p);

    // Parse payload as boolean, start is pointer on msg
    bool on = (strcmp(start, "1") == 0 || strcmp(start, "true") == 0 || strcmp(start, "on") == 0);
    // Serial.printf("MQTT msg [%s] => %s\n", topic, start);

    // --- Topic routing ---
    if (strcmp(t, "inkplate/control/booster/state") == 0)       { on ? wifi_toggle.enable()     : wifi_toggle.disable(); }
    else if (strcmp(t, "inkplate/control/cell/state") == 0)     { on ? cell_toggle.enable()     : cell_toggle.disable(); }
    else if (strcmp(t, "inkplate/control/starlink/state") == 0) { on ? starlink_toggle.enable() : starlink_toggle.disable(); }
    else if (strcmp(t, "inkplate/control/batlight/state") == 0) { on ? b_light.enable()         : b_light.disable(); }
    else if (strcmp(t, "inkplate/control/batfan/state") == 0)   { on ? b_fan.enable()           : b_fan.disable(); }
    else if (strcmp(t, "inkplate/control/deckwash/state") == 0) { on ? deck_wash.enable()       : deck_wash.disable(); }
    else if (strcmp(t, "inkplate/control/acport/state") == 0) { on ? ac_port_toggle.enable() : ac_port_toggle.disable(); }
    else if (strcmp(t, "inkplate/control/acstrb/state") == 0) { on ? ac_strb_toggle.enable() : ac_strb_toggle.disable(); }
    else if (strcmp(t, "tanks/fuelport") == 0) {
        fuelPort = atoi(start);
        page.setTank(0, fuelPort);
        if (currentPage == PAGE_TWO) page.drawTank(0);
    }
    else if (strcmp(t, "tanks/fuelstb") == 0) {
        fuelStb = atoi(start);
        page.setTank(1, fuelStb);
        if (currentPage == PAGE_TWO) page.drawTank(1);
    }
    else if (strcmp(t, "tanks/waterport") == 0) {
        waterPort = atoi(start);
        page.setTank(2, waterPort);
        if (currentPage == PAGE_TWO) page.drawTank(2);
    }
    else if (strcmp(t, "tanks/waterstb") == 0) {
        waterStb = atoi(start);
        page.setTank(3, waterStb);
        if (currentPage == PAGE_TWO) page.drawTank(3);
    }
    else if (strcmp(t, "inkplate/pumps/portfwd") == 0)  {
        page.setBilge(0, on);
        if (currentPage == PAGE_TWO) page.drawBilge(0);
    }
    else if (strcmp(t, "inkplate/pumps/portmid") == 0)   {
        page.setBilge(1, on);
        if (currentPage == PAGE_TWO) page.drawBilge(1);
    }
    else if (strcmp(t, "inkplate/pumps/porteng") == 0)  {
        page.setBilge(2, on);
        if (currentPage == PAGE_TWO) page.drawBilge(2);
    }
    else if (strcmp(t, "inkplate/pumps/stbfwd") == 0)  {
        page.setBilge(3, on);
        if (currentPage == PAGE_TWO) page.drawBilge(3);
    }
    else if (strcmp(t, "inkplate/pumps/stbmid") == 0)   {
        page.setBilge(4, on);
        if (currentPage == PAGE_TWO) page.drawBilge(4);
    }
    else if (strcmp(t, "inkplate/pumps/stbeng") == 0)   {
        page.setBilge(5, on);
        if (currentPage == PAGE_TWO) page.drawBilge(5);
    }

    // Optional: small log
    char logEntry[128];
    snprintf(logEntry, sizeof(logEntry), "STATE [%s] <= %s", topic, start);
    log_mqtt(logEntry);
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(client_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(TOPIC_SUB);
      client.subscribe(TOPIC_TANKS);
      client.subscribe(TOPIC_PUMPS);
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
    b_light.draw();
    b_fan.draw();
    deck_wash.draw();
    ac_port_toggle.draw();
    ac_strb_toggle.draw();
    display.display();
}


void setupToggleListener(Display::Toggle& toggle, const char* topic) {
    // topic is the command topic, e.g. "inkplate/control/aux1"
    toggle.onClickListener = [topic](Display::Toggle* t) {
    if (millis() < wakeGuardUntilMs) return;
    lastInteractionMs = millis();
    const char* payload = t->state ? "0" : "1";
    bool ok = client.publish(topic, payload, false);
    String s = String("CMD [") + topic + "] => " + payload + (ok ? "" : " (fail)");
    log_mqtt(s.c_str());

    // Immediately clear the button area (show pending)
    t->clearButtonArea();
};
}


void waitClick() {
    setupToggleListener(wifi_toggle, wifi_toggle.name);
    setupToggleListener(cell_toggle, cell_toggle.name);
    setupToggleListener(starlink_toggle, starlink_toggle.name);
    setupToggleListener(b_light, b_light.name);
    setupToggleListener(b_fan, b_fan.name);
    setupToggleListener(deck_wash, deck_wash.name);
    setupToggleListener(ac_port_toggle, ac_port_toggle.name);
    setupToggleListener(ac_strb_toggle, ac_strb_toggle.name);
}


void showMainPage() { 
    // Serial.println("Main Page"); 
    display.clearDisplay(); 
    drawNetPage(); 
}

void showPage2() { 
    // Serial.println("Page2"); 
    display.clearDisplay();
    page.draw();
    display.display();
}


void getTemp() {
    float temp = display.readTemperature();
    // Serial.print("Temp: ");
    // Serial.println(temp);
    // char buf[16];
    // snprintf(buf, sizeof(buf), "Temp: %.1fC", temp);
    // log_mqtt(buf);

    // Publish to MQTT topic "inkplate/temp"
    char tempStr[8];
    snprintf(tempStr, sizeof(tempStr), "%.1f", temp);
    client.publish("inkplate/temp", tempStr, true); // true = retained, optional
}


void changePage() {
    lastInteractionMs = millis();
    currentPage = (currentPage == PAGE_MAIN) ? PAGE_TWO : PAGE_MAIN;
    Serial.println("Two fingers - switch page");
    getTemp();
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
  getTemp();
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
  
  auto touchRecord = Display::readTouchData();
  if (touchRecord.second > 1) {
    changePage();
  } else if (touchRecord.second == 1 && touchRecord.first != nullptr && currentPage == PAGE_MAIN) {
    wifi_toggle.readCheckState(*touchRecord.first);
    cell_toggle.readCheckState(*touchRecord.first);
    starlink_toggle.readCheckState(*touchRecord.first);
    b_light.readCheckState(*touchRecord.first);
    b_fan.readCheckState(*touchRecord.first);
    deck_wash.readCheckState(*touchRecord.first);
    ac_port_toggle.readCheckState(*touchRecord.first);
    ac_strb_toggle.readCheckState(*touchRecord.first);
  }
  else {
    wifi_toggle.resetPress();
    cell_toggle.resetPress();
    starlink_toggle.resetPress();
    b_light.resetPress();
    b_fan.resetPress();
    deck_wash.resetPress();
    ac_port_toggle.resetPress();
    ac_strb_toggle.resetPress();
  }

  // Full redraw every 5 minutes
  if (millis() - lastRedrawMs > FULL_REDRAW_INTERVAL_MS) {
      lastRedrawMs = millis();
      if (currentPage == PAGE_MAIN) showMainPage();
      else showPage2();
  }

  if (millis() - lastTempMs > TEMP_INTERVAL_MS) {
    lastTempMs = millis();
    getTemp();
}
}
