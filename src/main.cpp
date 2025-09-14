#include <WiFi.h>             // ESP32 / ESP8266 WiFi
#include <PubSubClient.h>     // MQTT client
#include "secrets.h"
#include "Display.h"
#include <Inkplate.h>         // Inkplate library
#include "Page.h" 
#include <Adafruit_HDC302x.h>

#define TOPIC_LOG "inkplate/log/info"
#define TOPIC_ERROR "inkplate/log/last_error"
#define TOPIC_IP "inkplate/log/ip"
#define TOPIC_SUB "inkplate/control/+/state"
#define TOPIC_BOOSTER "inkplate/control/booster"
#define TOPIC_CELL "inkplate/control/cell"
#define TOPIC_STARLINK "inkplate/control/starlink"
#define TOPIC_B_LIGHT "inkplate/control/batlight"
#define TOPIC_B_FAN "inkplate/control/batfan"
#define TOPIC_DECKWASH "inkplate/control/deckwash"
#define TOPIC_AC_PORT "inkplate/control/acport"
#define TOPIC_AC_STRB "inkplate/control/acstrb"
#define TOPIC_ENV "inkplate/env"
#define TOPIC_INFO "info/#" // info/victron|history/pumps|tanks/ 

// --- sensor settings ---
static uint32_t lastEnvMs = 60000; // force reading on first loop
constexpr uint32_t ENV_INTERVAL_MS = 60000; // 60 seconds

// --- Backlight control ---
constexpr uint32_t BACKLIGHT_TIMEOUT_MS = 60000; // 60s (adjust as you like)
constexpr uint8_t  FRONTLIGHT_ON_LEVEL  = 4;
constexpr uint8_t  FRONTLIGHT_OFF_LEVEL = 0;

static uint32_t lastInteractionMs = 0;
static bool     backlightOn       = true;
static uint32_t wakeGuardUntilMs  = 0; // swallow touches right after wake

enum Page2 { INFO_PAGE, SWITCH_PAGE };
static Page2 currentPage = INFO_PAGE;

static uint32_t lastRedrawMs = 0;
constexpr uint32_t FULL_REDRAW_INTERVAL_MS = 600000; // 10 minutes

int fuelPort = 0, fuelStb = 0, waterPort = 0, waterStb = 0, soc = 0;
float batValue1 = 0.0, batValue2 = 0.0, batValue3 = 0.0;

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

struct ToggleEntry {
    const char* topic;
    Display::Toggle* toggle;
};

ToggleEntry toggles[] = {
    { TOPIC_BOOSTER "/state",   &wifi_toggle },
    { TOPIC_CELL "/state",      &cell_toggle },
    { TOPIC_STARLINK "/state",  &starlink_toggle },
    { TOPIC_B_LIGHT "/state",   &b_light },
    { TOPIC_B_FAN "/state",     &b_fan },
    { TOPIC_DECKWASH "/state",  &deck_wash },
    { TOPIC_AC_PORT "/state",   &ac_port_toggle },
    { TOPIC_AC_STRB "/state",   &ac_strb_toggle }
};
constexpr int numToggles = sizeof(toggles) / sizeof(toggles[0]);

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_HDC302x hdc = Adafruit_HDC302x();

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
    // Parse payload
    char buf[64];
    size_t len = (length < sizeof(buf) - 1) ? length : sizeof(buf) - 1;
    memcpy(buf, payload, len);
    buf[len] = '\0';
    bool on = (length == 1 && payload[0] == '1');

    // --- Handle toggles ---
    for (int i = 0; i < numToggles; ++i) {
        if (strcmp(topic, toggles[i].topic) == 0) {
            toggles[i].toggle->state = on;
            toggles[i].toggle->draw(currentPage == SWITCH_PAGE);
            goto log_and_return;
        }
    }

    // --- Handle tanks ---
    if (strncmp(topic, "info/victron/tanks", 18) == 0) {
        int idx = -1;
        if (strcmp(topic, "info/victron/tanks/fuelPort") == 0) idx = 0;
        else if (strcmp(topic, "info/victron/tanks/fuelStb") == 0) idx = 1;
        else if (strcmp(topic, "info/victron/tanks/soc") == 0) idx = 2;
        else if (strcmp(topic, "info/victron/tanks/waterPort") == 0) idx = 3;
        else if (strcmp(topic, "info/victron/tanks/waterStb") == 0) idx = 4;
        if (idx >= 0) {
            int val = round(atof(buf)); // FIXED: use buf
            bool skip = false;
            switch (idx) {
                case 0: skip = (val == fuelPort);  break;
                case 1: skip = (val == fuelStb);   break;
                case 2: skip = (val == soc);       break;
                case 3: skip = (val == waterPort); break;
                case 4: skip = (val == waterStb);  break;
            }
            if (!skip) {
                switch (idx) {
                    case 0: fuelPort = val;  break;
                    case 1: fuelStb = val;   break;
                    case 2: soc = val;       break;
                    case 3: waterPort = val; break;
                    case 4: waterStb = val;  break;
                }
                page.setTank(idx, val);
                if (currentPage == INFO_PAGE) page.drawTank(idx);
            }
            goto log_and_return;
        }
    }

    // --- Handle battery voltage ---
    if (strncmp(topic, "info/victron/bat/voltage", 24) == 0) {
        int idy = -1;
        if (strcmp(topic, "info/victron/bat/voltage/48v") == 0) idy = 0;
        else if (strcmp(topic, "info/victron/bat/voltage/24v") == 0) idy = 1;
        else if (strcmp(topic, "info/victron/bat/voltage/12v") == 0) idy = 2;
        if (idy >= 0) {
            float val = atof(buf); // FIXED: use buf
            bool skip = false;
            switch (idy) {
                case 0: skip = (val == batValue1); break;
                case 1: skip = (val == batValue2); break;
                case 2: skip = (val == batValue3); break;
            }
            if (!skip) {
                switch (idy) {
                    case 0: batValue1 = val; break;
                    case 1: batValue2 = val; break;
                    case 2: batValue3 = val; break;
                }
                page.setBat(batValue1, batValue2, batValue3);
                if (currentPage == INFO_PAGE) page.updateBat();
            }
            goto log_and_return;
        }
    }

    // --- Handle bat ---
    if (strncmp(topic, "info/nodered/bilge", 18) == 0) {
        int idx = -1;
        if (strcmp(topic, "info/nodered/bilge/PortFwd/m_per_xh") == 0) idx = 0;
        else if (strcmp(topic, "info/nodered/bilge/PortMid/m_per_xh") == 0) idx = 1;
        else if (strcmp(topic, "info/nodered/bilge/PortEng/m_per_xh") == 0) idx = 2;
        else if (strcmp(topic, "info/nodered/bilge/StbFwd/m_per_xh") == 0) idx = 3;
        else if (strcmp(topic, "info/nodered/bilge/StbMid/m_per_xh") == 0) idx = 4;
        else if (strcmp(topic, "info/nodered/bilge/StbEng/m_per_xh") == 0) idx = 5;
        if (idx >= 0) {
            int value = atoi(buf); // FIXED: use buf
            page.setBilge(idx, value);
            if (currentPage == INFO_PAGE) page.drawBilge(idx);
            goto log_and_return;
        }
    }


    // --- Handle sea temp ---
    if (strcmp(topic, "info/nodered/seaTemp") == 0) {
        float val = atof(buf); // FIXED: use buf
        page.setSeaTemp(val);
        if (currentPage == INFO_PAGE) page.updateSeaTemp();
        goto log_and_return;
    }   

    // --- Log ---
    log_and_return:
    char logEntry[128];
    snprintf(logEntry, sizeof(logEntry), "STATE [%s] <= %.*s", topic, length, payload);
    log_mqtt(logEntry);
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(client_id, mqtt_user, mqtt_pass)) {
            Serial.println("connected");
            client.subscribe(TOPIC_SUB);
            client.subscribe(TOPIC_INFO);
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

void initDisplay() {
    display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();
    delay(1000);
    Display::display = &display;
    display.clearDisplay();
    display.frontlight(true);
    display.setFrontlight(FRONTLIGHT_ON_LEVEL);
    Serial.println("TS init: ");
    while (!display.tsInit(true)) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println("TS init: ok");
    backlightOn = true;
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

void SwitchPage() { 
    display.clearDisplay(); 
    title.draw();
    wifi_toggle.draw(true,false);
    cell_toggle.draw(true,false);
    starlink_toggle.draw(true,false);
    b_light.draw(true,false);
    b_fan.draw(true,false);
    deck_wash.draw(true,false);
    ac_port_toggle.draw(true,false);
    ac_strb_toggle.draw(true,false);
    waitClick();
    display.display();
}

void InfoPage() { 
    display.clearDisplay();
    page.draw();
    display.display();
}


void changePage() {
    lastInteractionMs = millis();
    currentPage = (currentPage == INFO_PAGE) ? SWITCH_PAGE : INFO_PAGE;
    //Serial.println("Two fingers - switch page");
    if (currentPage == INFO_PAGE) InfoPage();
    else SwitchPage();
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
    if (! hdc.begin(0x46, &Wire)) {
        Serial.println("Could not find sensor?");
        while (1);
    }
    delay(1000);
    InfoPage();
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

    if (touchRecord.second > 1) {changePage();}

    if (currentPage == SWITCH_PAGE) {
        if (touchRecord.second == 1 && touchRecord.first != nullptr) {
            wifi_toggle.readCheckState(*touchRecord.first);
            cell_toggle.readCheckState(*touchRecord.first);
            starlink_toggle.readCheckState(*touchRecord.first);
            b_light.readCheckState(*touchRecord.first);
            b_fan.readCheckState(*touchRecord.first);
            deck_wash.readCheckState(*touchRecord.first);
            ac_port_toggle.readCheckState(*touchRecord.first);
            ac_strb_toggle.readCheckState(*touchRecord.first);
        } else {
            wifi_toggle.resetPress();
            cell_toggle.resetPress();
            starlink_toggle.resetPress();
            b_light.resetPress();
            b_fan.resetPress();
            deck_wash.resetPress();
            ac_port_toggle.resetPress();
            ac_strb_toggle.resetPress();
        }
    }

    // Full redraw every 5 minutes
    if (!backlightOn && (millis() - lastRedrawMs > FULL_REDRAW_INTERVAL_MS)) {
        lastRedrawMs = millis();
        if (currentPage == SWITCH_PAGE) SwitchPage();
        else InfoPage();
    }
   
    if (millis() - lastEnvMs > ENV_INTERVAL_MS) {
        lastEnvMs = millis();
        double temp = 0.0;
        double RH = 0.0;
        hdc.readTemperatureHumidityOnDemand(temp, RH, TRIGGERMODE_LP0);
        char tempStr[16], humStr[16];
        snprintf(tempStr, sizeof(tempStr), "%.2f", temp);
        snprintf(humStr, sizeof(humStr), "%.2f", RH);
        client.publish(TOPIC_ENV "/temp", tempStr, true); // retained
        client.publish(TOPIC_ENV "/hum", humStr, true);   // retained
        page.setTemp(temp);
        page.setHum(RH);
        page.setAirPressure(1023.5); // dummy value
        if (currentPage == INFO_PAGE) {
            page.updateTemp();
            page.updateHum();
            page.updateAirPressure();
        }
    }
}

