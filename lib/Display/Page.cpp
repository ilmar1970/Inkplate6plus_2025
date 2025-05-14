#include "Display.h"

using Display::Page;

Page::Page() {
    this->display = new Inkplate(INKPLATE_1BIT);
    this->objects = new Drawable*[100];
    this->lastObjectIndex = 0;
}

void Page::draw() const {
    display->setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display->begin();
    display->frontlight(true);
    display->setFrontlight(3);
    display->clearDisplay();
    if (display->tsInit(true)){
        Serial.println("TS: success");
    } else {
        Serial.println("TS: fail");
    }
    for(int i = 0; i < lastObjectIndex; i++) {
        this->objects[i]->draw();
    }
}

void Page::attachObject(Drawable* object) {
    objects[lastObjectIndex] = object;
    lastObjectIndex++;
    object->display = display;
}

Page::~Page() {
    delete display;
    for(int i = 0; i < lastObjectIndex; i++){
        delete objects[i];
    }
    delete[] objects;
}