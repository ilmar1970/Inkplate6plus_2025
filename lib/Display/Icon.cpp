#include "Display.h"

using Display::Icon;

Icon::Icon(const uint8_t* bitmap, DisplayCoordinates iconPosition, DisplayCoordinates iconSize) {
    this->bitmap = bitmap;
    this->iconPosition = iconPosition;
    this->iconSize = iconSize;
}

void Icon::draw() const {
    display->drawBitmap3Bit(this->iconPosition.first, this->iconPosition.second, this->bitmap, this->iconSize.first, this->iconSize.second);
}