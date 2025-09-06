#include "Display.h"

using Display::Page;

Page::Page() : objects{} {
    this->display = std::unique_ptr<Inkplate>(new Inkplate(INKPLATE_1BIT));
}

void Page::draw() const {
    display->setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display->begin();
    display->frontlight(true);
    display->setFrontlight(3);
    display->clearDisplay();
    if(display->tsInit(true)) {
        Serial.println("TS: success");
    } else {
        Serial.println("TS: fail");
    }
    for (const std::unique_ptr<Drawable> &obj : this->objects) {
        obj->draw();
    }
    display->display();
}

void Page::attachObject(Drawable* object) {
    objects.push_back(std::unique_ptr<Drawable>(object));
}
