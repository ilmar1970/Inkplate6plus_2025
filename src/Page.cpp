#include "Page.h"
#include "icons.h" 

const int Page::rect_a_x[Page::tankCount] = {150, 320, 554, 734}; // 50
const int Page::rect_b_x[Page::tankCount] = {290, 470, 694, 874}; 
const int Page::rect_a_y = 220;
const int Page::rect_b_y = 480;
// Bilge circle positions and radius
const int Page::bilge_x[Page::bilgeCount] = {70, 70, 70, 950, 950, 950};
const int Page::bilge_y[Page::bilgeCount] = {245, 435, 625, 245, 435, 625};

Page::Page(Inkplate& disp) : display(disp) {
    for (int i = 0; i < tankCount; ++i) {
        percent[i] = 0;
        bilgeState[i] = false;
    }
}

void Page::setTank(int idx, int pct) {
    if (idx < 0 || idx >= tankCount) return;
    percent[idx] = pct;
}
void Page::setBilge(int idx, bool state) {
    if (idx < 0 || idx >= bilgeCount) return;
    bilgeState[idx] = state;
}

void Page::draw() {
    display.drawBitmap(280, 65, water, 60, 60, BLACK);
    display.drawBitmap(694, 65, diesel, 60, 60, BLACK);
    display.drawBitmap(30, 70, bilge, 80, 60, BLACK);
    display.drawBitmap(910, 70, bilge, 80, 60, BLACK);

    // Draw tanks
    for (int i = 0; i < tankCount; ++i) {
        int w = rect_b_x[i] - rect_a_x[i];
        int h = rect_b_y - rect_a_y;
        int fill_h = (h * percent[i]) / 100;
        int fill_y = rect_b_y - fill_h;

        // Draw outlined rectangle
        display.drawRoundRect(rect_a_x[i], rect_a_y, w, h, 8, BLACK);

        // Draw filled black rectangle (percent fill, from bottom up)
        display.fillRect(rect_a_x[i], fill_y, w, fill_h, BLACK);

        // Draw label: just percent + " %"
        String label = String(percent[i]) + " %";
        display.setFont(&FreeSansBold24pt7b);
        display.setTextColor(BLACK, WHITE);
        display.setTextSize(1);
        display.setCursor(rect_a_x[i] + 20, rect_a_y - 10);
        display.print(label);
    }
    // Draw bilge circles
    for (int i = 0; i < bilgeCount; ++i) {
        if (bilgeState[i])
            display.fillCircle(bilge_x[i], bilge_y[i], bilge_radius, BLACK);
        else
            display.drawCircle(bilge_x[i], bilge_y[i], bilge_radius, BLACK);
    }
}

void Page::drawTank(int idx) {
    int x0 = rect_a_x[idx], y0 = rect_a_y, x1 = rect_b_x[idx], y1 = rect_b_y;
    int w = x1 - x0, h = y1 - y0;
    int fill_h = (h * percent[idx]) / 100;
    int fill_y = y1 - fill_h;

    // --- Clear the entire tank area (including label area) ---
    // Add margin if needed to cover outline thickness and label
    int clear_x = x0 - 2;
    int clear_y = y0 - 45; // enough to cover label above
    int clear_w = w + 4;
    int clear_h = h + 45;  // enough to cover label and outline
    display.fillRect(clear_x, clear_y, clear_w, clear_h, WHITE);

    // Draw tank rectangle and fill
    display.drawRoundRect(x0, y0, w, h, 8, BLACK);
    display.fillRect(x0, fill_y, w, fill_h, BLACK);

    // --- Print new label ---
    display.setFont(&FreeSansBold24pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(x0 + 20, y0 - 10);
    String text = String(percent[idx]) + " %";
    display.print(text);

    display.partialUpdate();
}


void Page::drawBilge(int idx) {
    // Redraw only the bilge circle at index idx
    int x = bilge_x[idx], y = bilge_y[idx], r = bilge_radius;
    // Clear area
    display.fillRect(x - r - 2, y - r - 2, 2*r + 4, 2*r + 4, WHITE);

    if (bilgeState[idx])
        display.fillCircle(x, y, r, BLACK);
    else
        display.drawCircle(x, y, r, BLACK);
    display.partialUpdate();
}