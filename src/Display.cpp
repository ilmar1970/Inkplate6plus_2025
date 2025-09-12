#include "Display.h"

Inkplate* Display::display = nullptr;


constexpr int buttonWidth = 85;
constexpr int buttonHeight = 42;

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


void Display::Toggle::readCheckState(DisplayCoordinates touchCoordinates, const OnClickListener<Toggle *>& customOnClickListener) {
    bool pressed = false;
    int xbr = rectPosition.first + 85;
    int ybr = rectPosition.second + 42;
    // Serial.printf("Touch at (%d,%d); Rect at (%d,%d)-(%d,%d)\n",
    //              touchCoordinates.first, touchCoordinates.second,
    //              rectPosition.first, rectPosition.second, xbr, ybr);
    if (touchCoordinates.first >= rectPosition.first && touchCoordinates.first <= xbr &&
        touchCoordinates.second <= ybr && touchCoordinates.second >= rectPosition.second) {
        pressed = true;
    }
    if (pressed && !wasPressed) {
        auto handler = (customOnClickListener ? customOnClickListener : this->onClickListener);
        if (handler) handler(this);
    }
    wasPressed = pressed;
}

void Display::Toggle::resetPress() {
    wasPressed = false;
}

std::pair<DisplayCoordinates*, uint16_t> Display::readTouchData() {
    static DisplayCoordinates touchEvent;
    uint16_t n = 0;
    uint16_t x[2], y[2];
    if (display->tsAvailable()){
        n = display->tsGetData(x, y);
        for (int i = 0; i < n && i < 2; i++) {
            touchEvent = {x[i], y[i]};
        }
        return {&touchEvent, n};
    }
    // No touch: return nullptr and n=0
    return {nullptr, 0};
}

void Display::Toggle::clearButtonArea() {
    int x = rectPosition.first;
    int y = rectPosition.second;
    int w = buttonWidth;
    int h = buttonHeight;
    display->fillRect(x, y, w, h, WHITE);
    // Redraw the outline so the button shape remains visible
    display->drawRoundRect(x, y, w, h, rectRadius, BLACK);
    display->partialUpdate();
}

