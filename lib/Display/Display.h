#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include <Arduino.h>
#include <Inkplate.h>
#include "FreeSans18pt7b.h"

using DisplayCoordinates = std::pair<int16_t, int16_t>;
template<class T>
using OnClickListener = std::function<void(T)>;

namespace Display {
    class Drawable {
        public:
            Inkplate* display;

            std::pair<std::vector<DisplayCoordinates>, uint16_t> readTouchData();

            virtual void draw() const;
            virtual ~Drawable() {}
    };

    class Text : public Drawable {
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
            bool wasPressed;

            static int16_t circleRadius;
            static int16_t rectRadius;
            static OnClickListener<Toggle*> onClickListener;

            Toggle(
                const char* text,
                const char* name,
                DisplayCoordinates textPosition,
                int textSize=1,
                const GFXfont* font=&FreeSans18pt7b,
                bool state=false
            );
            using Text::draw;
            void readCheckState(DisplayCoordinates touchCoordinates, const OnClickListener<Toggle*>& customOnClickListener = nullptr);
            void enable(bool is_partial=true);
            void disable(bool is_partial=true);
    };

    class Icon : public Drawable {
        public:
            const uint8_t* bitmap;
            DisplayCoordinates iconPosition;
            DisplayCoordinates iconSize;
            
            Icon(
                const uint8_t* bitmap,
                DisplayCoordinates iconPosition,
                DisplayCoordinates iconSize
            );
            void draw() const;
    };

    class Bar : public Drawable {
        public:
            DisplayCoordinates barPosition;
            DisplayCoordinates barSize;
            // display.fillRect would likely fill everything. Need partial filling

            static int16_t rectRadius;

            Bar(
                DisplayCoordinates barPosition,
                DisplayCoordinates barSize
            );
            void draw() const;
    };

    class Page : public Drawable {
        public:
            Inkplate* display;
            Drawable** objects;
            int lastObjectIndex;

            Page();
            void attachObject(Drawable* object);
            void draw() const;
            ~Page();
    };
}

#endif