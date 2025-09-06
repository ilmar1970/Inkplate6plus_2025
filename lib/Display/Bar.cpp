#include "Display.h"

using Display::Bar;

int16_t Bar::rectRadius = 5;

Bar::Bar(InkplatePtr display, DisplayCoordinates barPosition, DisplayCoordinates barSize) : Drawable(display) {
    this->barPosition = barPosition;
    this->barSize = barSize;
}

void Bar::draw() {
    display->drawRoundRect(this->barPosition.first, this->barPosition.second, this->barSize.first, this->barSize.second, Bar::rectRadius, BLACK);
}