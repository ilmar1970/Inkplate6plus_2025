#include "Display.h"

using Display::Toggle;

int16_t Toggle::circleRadius = 15;
int16_t Toggle::rectRadius = 15;
OnClickListener<Toggle*> Toggle::onClickListener = nullptr;

Toggle::Toggle(
    std::shared_ptr<Inkplate> display,
    const char* text,
    const char* topic,
    DisplayCoordinates textPosition,
    int textSize,
    const GFXfont* font,
    bool state
) : Text(display, text, textPosition, textSize, font){
    this->topic = topic;
    this->rectPosition = {textPosition.first + 221, textPosition.second - 30};
    this->circlePosition = {textPosition.first + 285, textPosition.second - 10};
    this->state = state;
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
    display->drawRoundRect(rectPosition.first, rectPosition.second, 85, 42, rectRadius, BLACK);
    display->fillCircle(circlePosition.first - 45, circlePosition.second, circleRadius, BLACK);
    if (is_partial){
        display->partialUpdate();
    }
    state = false;
}

void Toggle::readCheckState(DisplayCoordinates touchCoordinates, const OnClickListener<Toggle *> &customOnClickListener) {
    bool pressed = false;
    int xbr = rectPosition.first + 85;
    int ybr = rectPosition.second + 42;
    if (touchCoordinates.first >= rectPosition.first && touchCoordinates.first <= xbr &&
        touchCoordinates.second <= ybr && touchCoordinates.second >= rectPosition.second) {
        pressed = true;
    }
    if (pressed) {
        auto handler = (customOnClickListener ? customOnClickListener : this->onClickListener);
        if (handler) handler(this);
    }
}

void Toggle::draw() {
    Text::draw();
    state ? enable(false) : disable(false);
}