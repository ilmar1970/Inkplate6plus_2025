#include "Display.h"

Inkplate* Display::display = nullptr;

using Display::Text;

Text::Text(const char *text, DisplayCoordinates textPosition, int textSize, const GFXfont *font) {
    this->text = text;
    this->textPosition = textPosition;
    this->textSize = textSize;
    this->font = font;
}

void Text::draw() const {
    display->setFont(font);
    display->setTextColor(BLACK, WHITE);
    display->setTextSize(textSize);
    display->setCursor(textPosition.first, textPosition.second);
    display->print(text);
}

using Display::Toggle;

int16_t Toggle::circleRadius = 15;
int16_t Toggle::rectRadius = 15;
OnClickListener<Toggle*> Toggle::onClickListener = nullptr;

Toggle::Toggle(
        const char* text,
        const char* name,
        DisplayCoordinates textPosition,
        int textSize,
        const GFXfont* font,
        bool state
) : Text(text, textPosition, textSize, font){
    this->name = name;
    this->rectPosition = {textPosition.first + 221, textPosition.second - 30};
    this->circlePosition = {textPosition.first + 285, textPosition.second - 10};
    this->state = state;
}

void Toggle::draw() {
    Text::draw();
    state ? enable(false) : disable(false);
}

void Toggle::enable(bool is_partial) {
    display->fillRoundRect(rectPosition.first, rectPosition.second, 85, 42, rectRadius, BLACK);
    display->fillCircle(circlePosition.first, circlePosition.second, circleRadius, WHITE);
    if (is_partial){
        display->partialUpdate();
    }
    state = true;
}

void Toggle::disable(bool is_partial) {
    display->fillRoundRect(rectPosition.first, rectPosition.second, 85, 42, rectRadius, WHITE);
    display->drawRoundRect(rectPosition.first, rectPosition.second, 85, 42, rectRadius,BLACK);
    display->fillCircle(circlePosition.first - 45, circlePosition.second, circleRadius, BLACK);
    if (is_partial){
        display->partialUpdate();
    }
    state = false;
}

void Display::Toggle::readCheckState(const OnClickListener<Toggle *>& customOnClickListener) {
    if(display->touchInArea(rectPosition.first, rectPosition.second, 85, 42)){
        state ? disable() : enable();
        customOnClickListener == nullptr ? Toggle::onClickListener(this) : customOnClickListener(this);
    }
}