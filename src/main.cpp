#include <WiFi.h>
#include <PubSubClient.h>
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

Display::Page togglePage{};
Display::Page barPage{};
Display::Page* activePage;

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

void setup() {
  Serial.begin(115200);
  setupTogglePage();
  setupBarPage();
  togglePage.draw();
}

void loop() {
}