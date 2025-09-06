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

using InkplatePtr = std::shared_ptr<Inkplate>;
using PagePtr = std::shared_ptr<Display::Page>;
using TextPtr = std::shared_ptr<Display::Text>;
using TogglePtr = std::shared_ptr<Display::Toggle>;

InkplatePtr display;
PagePtr togglePage;
PagePtr barPage;
PagePtr activePage;

void setupTogglePage() {
  togglePage->attachObject(TextPtr(new Display::Text(display, "SeaEsta", {400, 80}, 2, &FreeSans18pt7b)));

  // togglePage->attachObject(new Display::Text("SeaEsta", {400, 80}, 2));
  // togglePage->attachObject(new Display::Toggle("WiFi Booster", TOPIC_BOOSTER, {100, 200}));
  // togglePage->attachObject(new Display::Toggle("Cell", TOPIC_CELL, {100, 400}));
  // togglePage->attachObject(new Display::Toggle("Starlink", TOPIC_STARLINK, {100, 600}));
  // togglePage->attachObject(new Display::Toggle("AUX1", TOPIC_AUX1, {600, 200}));
  // togglePage->attachObject(new Display::Toggle("AUX2", TOPIC_AUX2, {600, 400}));
  // togglePage->attachObject(new Display::Toggle("AUX3", TOPIC_AUX3, {600, 600}));
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

void setup() {
  Serial.begin(115200);

  display = InkplatePtr(new Inkplate(INKPLATE_1BIT));

  if (!display) {
    Serial.println("display is null!");
    return;
  }

  togglePage = PagePtr(new Display::Page(display));

  if (!togglePage) {
    Serial.println("togglePage is null!");
    return;
  }

  // barPage = new Display::Page(display);

  setupTogglePage();
  // setupBarPage();

  Serial.println("Calling togglePage->draw()...");
  togglePage->draw();
  Serial.println("togglePage->draw() finished");
}

void loop() {
}