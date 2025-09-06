#include "Display.h"

using Display::Page;

Page::Page(std::shared_ptr<Inkplate> display) : Drawable(display) {
    this->objects = PointerArray<Drawable>{};
}

void Page::draw() {
    display->setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display->begin();
    delay(1000);
    display->clearDisplay();
    display->frontlight(true);
    display->setFrontlight(3);
    if(display->tsInit(true)) {
        Serial.println("TS: success");
    } else {
        Serial.println("TS: fail");
    }
    for (const auto &obj : this->objects) {
        if (obj) obj->draw();
    }
    display->display();
}

void Page::attachObject(std::shared_ptr<Drawable> object) {
    if (object) {
        objects.push_back(object);
    } else {
        Serial.printf("Attemped to attach null Drawable to page: %p\n", this);
    }
}
