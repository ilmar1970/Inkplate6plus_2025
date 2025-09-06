#include "Display.h"

using Display::Icon;

Icon::Icon(InkplatePtr display, const uint8_t* bitmap, DisplayCoordinates iconPosition, DisplayCoordinates iconSize) : Drawable(display) {
    this->bitmap = bitmap;
    this->iconPosition = iconPosition;
    this->iconSize = iconSize;
}

void Icon::draw() {
    display->drawBitmap3Bit(this->iconPosition.first, this->iconPosition.second, this->bitmap, this->iconSize.first, this->iconSize.second);
}