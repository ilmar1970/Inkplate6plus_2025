#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include <Arduino.h>
#include <Inkplate.h>
#include "FreeSans18pt7b.h"
#include "Alias.h"

using InkplatePtr = std::shared_ptr<Inkplate>;
using DisplayCoordinates = std::pair<int16_t, int16_t>;

namespace Display {
    class Drawable {
        public:
            InkplatePtr display;

            Drawable(InkplatePtr display);
            std::pair<std::vector<DisplayCoordinates>, uint16_t> readTouchData();

            virtual void draw() = 0;
            ~Drawable() = default;
    };

    class Text : public Drawable {
        public:
            const char* text;
            DisplayCoordinates textPosition;
            int textSize;
            const GFXfont* font;

            Text(
                InkplatePtr display,
                const char* text,
                DisplayCoordinates textPosition,
                int textSize,
                const GFXfont* font
            );
            void draw() override;
    };

    class Toggle : public Text {
        public:
            const char* topic;
            DisplayCoordinates rectPosition;
            DisplayCoordinates circlePosition;
            OnEventListener<Toggle*> onClickListener;
            bool state;

            static int16_t circleRadius;
            static int16_t rectRadius;

            Toggle(
                InkplatePtr display,
                const char* text,
                const char* topic,
                DisplayCoordinates textPosition,
                int textSize,
                const GFXfont* font,
                bool state
            );
            void readCheckState(DisplayCoordinates touchCoordinates, const OnEventListener<Toggle*>& onClickListener = nullptr);
            void enable(bool is_partial=true);
            void disable(bool is_partial=true);

            void draw() override;
    };

    class Icon : public Drawable {
        public:
            const uint8_t* bitmap;
            DisplayCoordinates iconPosition;
            DisplayCoordinates iconSize;
            
            Icon(
                InkplatePtr display,
                const uint8_t* bitmap,
                DisplayCoordinates iconPosition,
                DisplayCoordinates iconSize
            );
            void draw() override;
    };

    class Bar : public Drawable {
        public:
            DisplayCoordinates barPosition;
            DisplayCoordinates barSize;

            static int16_t rectRadius;

            Bar(
                InkplatePtr display,
                DisplayCoordinates barPosition,
                DisplayCoordinates barSize
            );
            void draw() override;
    };

    class Page : public Drawable {
        public:
            PointerArray<Drawable> objects;

            Page(InkplatePtr display);
            void attachObject(std::shared_ptr<Drawable> object);
            void draw() override;
    };
}

using PagePtr = std::shared_ptr<Display::Page>;
using TextPtr = std::shared_ptr<Display::Text>;
using TogglePtr = std::shared_ptr<Display::Toggle>;

#endif