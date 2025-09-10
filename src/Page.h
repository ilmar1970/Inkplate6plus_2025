#pragma once
#include "Arduino.h"
#include "Inkplate.h"
#include "Fonts/FreeSansBold24pt7b.h"
#include "icons.h"

class Page {
public:
    Page(Inkplate& display);

    // Setters for dynamic values
    void setTank(int idx, int pct);
    void setBilge(int idx, bool state);

    // Draw the page
    void draw();
    void drawTank(int idx);
    void drawBilge(int idx);

private:
    Inkplate& display;

    // Tank data
    static const int tankCount = 4;
    static const int bilgeCount = 6;
    int percent[tankCount];
    static const int bilge_x[bilgeCount];
    static const int bilge_y[bilgeCount];
    static constexpr int bilge_radius = 30;
    bool bilgeState[bilgeCount];

    // Geometry for rectangles and circles
    static const int rect_a_x[tankCount];
    static const int rect_a_y;
    static const int rect_b_x[tankCount];
    static const int rect_b_y;
    static const int circle_x[tankCount];
    static const int circle_y;
    static constexpr int circle_radius = 30;

};