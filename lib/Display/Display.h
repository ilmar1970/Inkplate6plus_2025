#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include <Arduino.h>
#include <Inkplate.h>
#include "FreeSans18pt7b.h"

using DisplayCoordinates = std::pair<int16_t, int16_t>;

template<class T>
using OnClickListener = std::function<void(T)>;

template<class P>
using PointerArray = std::vector<std::shared_ptr<P>>;

namespace Display {
    class Drawable {
        public:
            std::shared_ptr<Inkplate> display;

            Drawable(std::shared_ptr<Inkplate> display);
            std::pair<std::vector<DisplayCoordinates>, uint16_t> readTouchData();

            virtual void draw() const = 0;
            virtual ~Drawable() = default;
    };

    class Text : public Drawable {
        public:
            const char* text;
            DisplayCoordinates textPosition;
            int textSize;
            const GFXfont* font;

            Text(
                std::shared_ptr<Inkplate> display,
                const char* text,
                DisplayCoordinates textPosition,
                int textSize,
                const GFXfont* font
            );
            void draw() const override;
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
                std::shared_ptr<Inkplate> display,
                const char* text,
                DisplayCoordinates textPosition,
                int textSize,
                const GFXfont* font,
                const char* name,
                bool state
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
                std::shared_ptr<Inkplate> display,
                const uint8_t* bitmap,
                DisplayCoordinates iconPosition,
                DisplayCoordinates iconSize
            );
            void draw() const override;
    };

    class Bar : public Drawable {
        public:
            DisplayCoordinates barPosition;
            DisplayCoordinates barSize;

            static int16_t rectRadius;

            Bar(
                std::shared_ptr<Inkplate> display,
                DisplayCoordinates barPosition,
                DisplayCoordinates barSize
            );
            void draw() const override;
    };

    class Page : public Drawable {
        public:
            PointerArray<Drawable> objects;

            Page(std::shared_ptr<Inkplate> display);
            void attachObject(std::shared_ptr<Drawable> object);
            void draw() const override;
            ~Page() = default;
    };
}

#endif