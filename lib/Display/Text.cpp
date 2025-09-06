#include "Display.h"

using Display::Text;

Text::Text(std::shared_ptr<Inkplate> display, const char* text, DisplayCoordinates textPosition, int textSize, const GFXfont* font) : Drawable(display) {
    this->text = text;
    this->textPosition = textPosition;
    this->textSize = textSize;
    this->font = font;
}

void Text::draw() {
    display->setFont(font);
    display->setTextColor(BLACK, WHITE);
    display->setTextSize(textSize);
    display->setCursor(textPosition.first, textPosition.second);
    display->print(text);
}