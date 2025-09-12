#include "Page.h"
#include "icons.h" 
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

// --- Icon and text positions for bottom row ---
constexpr int temp_icon_x = 270, temp_icon_y = 600;
constexpr int bat_icon_x  = 40,  bat_icon_y  = 600;
constexpr int hum_icon_x  = 500, hum_icon_y  = 600;
constexpr int air_icon_x  = 750, air_icon_y  = 600;
constexpr int icon_w = 60, icon_h = 60;

// --- Text positions (relative to icons) ---
constexpr int temp_text_x = temp_icon_x + icon_w + 10;
constexpr int temp_text_y = temp_icon_y + icon_h - 10;

constexpr int bat_text_x = bat_icon_x + icon_w + 20;
constexpr int bat_text_y = bat_icon_y + 15;
constexpr int bat_line = 35;

constexpr int hum_text_x = hum_icon_x + icon_w + 10;
constexpr int hum_text_y = hum_icon_y + icon_h - 10;

constexpr int air_text_x = air_icon_x + icon_w + 10;
constexpr int air_text_y = air_icon_y + icon_h - 10;

const int Page::rect_a_x[Page::tankCount] = {160, 330, 574, 744}; // 50
const int Page::rect_b_x[Page::tankCount] = {280, 460, 694, 864}; 
const int Page::rect_a_y = 220;
const int Page::rect_b_y = 480;
// Bilge circle positions and radius
const int Page::bilge_x[Page::bilgeCount] = {70, 70, 70, 950, 950, 950};
const int Page::bilge_y[Page::bilgeCount] = {245, 345, 445, 245, 345, 445};

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
    // --- Draw bottom icons ---
    display.drawBitmap(temp_icon_x, temp_icon_y, temp, icon_w, icon_h, BLACK);
    display.drawBitmap(bat_icon_x,  bat_icon_y,  bat,  icon_w, icon_h, BLACK);
    display.drawBitmap(hum_icon_x,  hum_icon_y,  hum,  icon_w, icon_h, BLACK);
    display.drawBitmap(air_icon_x,  air_icon_y,  air_pressure, icon_w, icon_h, BLACK);

    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);

    // --- Temperature (right of temp icon) ---
    display.setCursor(temp_text_x, temp_text_y);
    display.print(String(tempValue, 1) + " °C");

    // --- Battery (right of bat icon, three values in a column) ---
    display.setCursor(bat_text_x, bat_text_y);
    display.print(String(batValue1, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + bat_line);
    display.print(String(batValue2, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + 2 * bat_line);
    display.print(String(batValue3, 1) + " V");

    // --- Humidity (right of hum icon) ---
    display.setCursor(hum_text_x, hum_text_y);
    display.print(String(humValue, 1) + " %");

    // --- Air Pressure (right of air_pressure icon) ---
    display.setCursor(air_text_x, air_text_y);
    display.print(String(airPressureValue, 0) + " hPa");

    // --- Restore old icons ---
    // Example positions, adjust as needed:
    display.drawBitmap(280, 65, water, 60, 60, BLACK);   // Water icon
    display.drawBitmap(694, 65, diesel, 60, 60, BLACK);  // Diesel icon
    display.drawBitmap(30, 70, bilge, 80, 60, BLACK);    // Left bilge icon
    display.drawBitmap(910, 70, bilge, 80, 60, BLACK);   // Right bilge icon

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

void Page::updateTemp() {
    display.fillRect(temp_text_x, temp_text_y - 30, 100, 40, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(temp_text_x, temp_text_y);
    display.print(String(tempValue, 1) + " °C");
    display.partialUpdate();
}

void Page::updateHum() {
    display.fillRect(hum_text_x, hum_text_y - 30, 100, 40, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(hum_text_x, hum_text_y);
    display.print(String(humValue, 1) + " %");
    display.partialUpdate();
}

void Page::updateBat() {
    display.fillRect(bat_text_x, bat_text_y - 30, 100, 3 * bat_line + 40, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(bat_text_x, bat_text_y);
    display.print(String(batValue1, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + bat_line);
    display.print(String(batValue2, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + 2 * bat_line);
    display.print(String(batValue3, 1) + " V");
    display.partialUpdate();
}

void Page::updateAirPressure() {
    display.fillRect(air_text_x, air_text_y - 30, 100, 40, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(air_text_x, air_text_y);
    display.print(String(airPressureValue, 0) + " hPa");
    display.partialUpdate();
}