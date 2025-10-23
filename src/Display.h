#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Inkplate.h>
#include "Fonts/FreeSans18pt7b.h"
#include <functional>

using DisplayCoordinates = std::pair<int16_t, int16_t>;
template<class T>
using OnClickListener = std::function<void(T)>;

namespace Display {
    extern Inkplate* display;

    std::pair<DisplayCoordinates*, uint16_t> readTouchData();

    class Text {
        public:
            const char* text;
            DisplayCoordinates textPosition;
            int textSize;
            const GFXfont* font;

            Text(
                const char* text,
                DisplayCoordinates textPosition,
                int textSize=1,
                const GFXfont* font=&FreeSans18pt7b
            );
            void draw() const;
    };

    class Toggle : public Text {
        public:
            const char* name;
            DisplayCoordinates rectPosition;
            DisplayCoordinates circlePosition;
            bool state;

            static int16_t circleRadius;
            static int16_t rectRadius;
            // static OnClickListener<Toggle*> onClickListener;
            OnClickListener<Toggle*> onClickListener = nullptr;
            bool wasPressed = false;

            Toggle(
                const char* text,
                const char* name,
                DisplayCoordinates textPosition,
                int textSize=1,
                const GFXfont* font=&FreeSans18pt7b,
                bool state=false
            );
            void draw(bool shouldDraw=true, bool is_partial=true);
            void readCheckState(DisplayCoordinates touchCoordinates, const OnClickListener<Toggle*>& customOnClickListener = nullptr);
            void enable();
            void disable();
            void resetPress();
            void clearButtonArea();
            void setActive(bool a);
            bool isActive() const;

        private:
            bool active = true;
    };

}

#endif