#include "Page.h"
#include "icons.h" 
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

// --- Icon and text positions for bottom row ---
constexpr int temp_icon_x = 265, temp_icon_y = 550;
constexpr int bat_icon_x  = 40,  bat_icon_y  = 550;
constexpr int hum_icon_x  = 495, hum_icon_y  = 550;
constexpr int air_icon_x  = 750, air_icon_y  = 550;
constexpr int icon_w = 60, icon_h = 60;
constexpr int gw_x = 350; 
constexpr int gw_y = 710;

// --- Text positions (relative to icons) ---
constexpr int next_line = 35;
constexpr int temp_text_x = temp_icon_x + icon_w + 7; // 340
constexpr int temp_text_y = temp_icon_y + icon_h - 10; // 600
constexpr int sea_text_y = temp_text_y + next_line; // 635

constexpr int bat_text_x = bat_icon_x + icon_w + 20; // 120
constexpr int bat_text_y = bat_icon_y + 15; // 605

constexpr int hum_text_x = hum_icon_x + icon_w + 7; // 570
constexpr int hum_text_y = hum_icon_y + icon_h - 10; // 600

constexpr int air_text_x = air_icon_x + icon_w + 8; // 760
constexpr int air_text_y = air_icon_y + icon_h - 10; // 600

const int Page::rect_a_x[Page::tankCount] = {140, 300, 460, 604, 764}; 
const int Page::rect_b_x[Page::tankCount] = {260, 420, 564, 724, 884}; 
const int Page::rect_a_y = 220;
const int Page::rect_b_y = 480;
// Bilge circle positions and radius
const int Page::bilge_x[Page::bilgeCount] = {70, 70, 70, 950, 950, 950};
const int Page::bilge_y[Page::bilgeCount] = {245, 345, 445, 245, 345, 445};

Page::Page(Inkplate& disp) : display(disp) {
    for (int i = 0; i < tankCount; ++i) percent[i] = 0;
    for (int i = 0; i < bilgeCount; ++i) bilgeLevel[i] = 0;
    
}

void Page::setTank(int idx, int pct) {
    if (idx < 0 || idx >= tankCount) return;
    percent[idx] = pct;
}
void Page::setBilge(int idx, int level) {
    if (idx < 0 || idx >= bilgeCount) return;
    bilgeLevel[idx] = level;
}

void Page::draw() {
    // --- Upper icons row
    display.drawBitmap(30, 70, bilge, 80, 60, BLACK);
    display.drawBitmap(250, 65, diesel, 60, 60, BLACK);
    display.drawBitmap(480, 65, bat, 60, 60, BLACK);
    display.drawBitmap(720, 65, water, 60, 60, BLACK);
    display.drawBitmap(910, 70, bilge, 80, 60, BLACK);

    // --- Draw bottom icons ---
    display.drawBitmap(temp_icon_x, temp_icon_y, temp, icon_w, icon_h, BLACK);
    display.drawBitmap(bat_icon_x,  bat_icon_y,  bat,  icon_w, icon_h, BLACK);
    display.drawBitmap(hum_icon_x,  hum_icon_y,  hum,  icon_w, icon_h, BLACK);
    display.drawBitmap(air_icon_x,  air_icon_y,  air_pressure, icon_w, icon_h, BLACK);

    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);

     // --- Battery (right of bat icon, three values in a column) ---
    display.setCursor(bat_text_x, bat_text_y);
    display.print(String(batValue1, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + next_line);
    display.print(String(batValue2, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + 2 * next_line);
    display.print(String(batValue3, 1) + " V");

    //  temperature 
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(temp_text_x, temp_text_y - next_line);
    display.print(String(outTempValue, 1) + " Out");
    display.setCursor(temp_text_x, temp_text_y);
    display.print(String(tempValue, 1) + " In");
    display.setCursor(temp_text_x, sea_text_y);
    display.print(String(seaTempValue, 1) + " Sea");

    // humidity 
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(hum_text_x, hum_text_y - next_line);
    display.print(String(outHumValue, 1) + " %");
    display.setCursor(hum_text_x, hum_text_y);
    display.print(String(humValue, 1) + " %");

    // pressure 
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(air_text_x, air_text_y - next_line);
    display.print(String(outPresValue, 0) + " hPa");
    display.setCursor(air_text_x, air_text_y);
    display.print(String(airPressureValue, 0) + " hPa");
    
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

        // Draw label: 
        String label = String(percent[i]) + " %";
        display.setFont(&FreeSansBold24pt7b);
        display.setTextColor(BLACK, WHITE);
        display.setTextSize(1);
        display.setCursor(rect_a_x[i] + 5, rect_a_y - 20);
        display.print(label);
    }
    // Draw bilge circles
    for (int i = 0; i < bilgeCount; ++i) {
        display.drawCircle(bilge_x[i], bilge_y[i], bilge_radius, BLACK);
    }

    // Centered at bottom
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(gw_x, gw_y);
    display.print("GW: ");
    display.print(gatewayStr);
}

void Page::drawTank(int idx) {
    int x0 = rect_a_x[idx], y0 = rect_a_y, x1 = rect_b_x[idx], y1 = rect_b_y;
    int w = x1 - x0, h = y1 - y0;
    int fill_h = (h * percent[idx]) / 100;
    int fill_y = y1 - fill_h;

    // --- Clear the entire tank area (including label area) ---
    // Add margin if needed to cover outline thickness and label
    int clear_x = x0 - 2;
    int clear_y = y0 - 55; // enough to cover label above
    int clear_w = w + 10;
    int clear_h = h + 55;  // enough to cover label and outline
    display.fillRect(clear_x, clear_y, clear_w, clear_h, WHITE);

    // Draw tank rectangle and fill
    display.drawRoundRect(x0, y0, w, h, 8, BLACK);
    display.fillRect(x0, fill_y, w, fill_h, BLACK);

    // --- Print new label ---
    display.setFont(&FreeSansBold24pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(x0 + 5, y0 - 20);
    String text = String(percent[idx]) + " %";
    display.print(text);
    display.partialUpdate();
}

void Page::drawBilge(int idx) {
    if (idx < 0 || idx >= bilgeCount) return;
    // Always draw the outer circle
    display.fillCircle(bilge_x[idx], bilge_y[idx], bilge_radius, WHITE);
    display.drawCircle(bilge_x[idx], bilge_y[idx], bilge_radius, BLACK);

    // Draw inner black circle according to level (0-7)
    int r = bilgeLevel[idx] * 4;
    if (bilgeLevel[idx] > 0) {
        display.fillCircle(bilge_x[idx], bilge_y[idx], r, BLACK);
    }
    display.partialUpdate();
}


void Page::updateBat() {
    display.fillRect(bat_text_x, bat_text_y - 30, 100, 3 * next_line + next_line, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(bat_text_x, bat_text_y);
    display.print(String(batValue1, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + next_line);
    display.print(String(batValue2, 1) + " V");
    display.setCursor(bat_text_x, bat_text_y + 2 * next_line);
    display.print(String(batValue3, 1) + " V");
    display.partialUpdate();
}

void Page::updateOutTemp() {
    const int x = temp_text_x;
    const int outY = temp_text_y - next_line; // baseline used in draw()
    const int w = 140;
    const int h = 28; // height of clear box
    const int clearY = outY - h; // small top margin

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(outTempValue, 1) + " Out");
    display.partialUpdate();
}

void Page::updateTemp() {
    const int x = temp_text_x;
    const int outY = temp_text_y;
    const int w = 140;
    const int h = 28;
    const int clearY = outY - h;

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(tempValue, 1) + " In");
    display.partialUpdate();
}

void Page::updateSeaTemp() {
    const int x = temp_text_x;
    const int outY = sea_text_y;
    const int w = 160;
    const int h = 28;
    const int clearY = outY - h;

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(seaTempValue, 1) + " Sea");
    display.partialUpdate();
}

void Page::updateOutHum() {
    const int x = hum_text_x;
    const int outY = hum_text_y - next_line;
    const int w = 100;
    const int h = 28;
    const int clearY = outY - h;

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(outHumValue, 1) + " %");
    display.partialUpdate();
}

void Page::updateHum() {
    const int x = hum_text_x;
    const int outY = hum_text_y;
    const int w = 100;
    const int h = 28;
    const int clearY = outY - h;

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(humValue, 1) + " %");
    display.partialUpdate();
}

void Page::updateOutPres() {
    const int x = air_text_x;
    const int outY = air_text_y - next_line;
    const int w = 260;
    const int h = 28;
    const int clearY = outY - h;

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(outPresValue, 0) + " hPa");
    display.partialUpdate();
}

void Page::updateAirPressure() {
    const int x = air_text_x;
    const int outY = air_text_y;
    const int w = 260;
    const int h = 28;
    const int clearY = outY - h;

    display.fillRect(x, clearY, w, h, WHITE);
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(x, outY);
    display.print(String(airPressureValue, 0) + " hPa");
    display.partialUpdate();
}

void Page::updateGateway() {
    display.fillRect(gw_x, gw_y - 30, 500, next_line, WHITE); // clear area
    display.setFont(&FreeSans18pt7b);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(gw_x, gw_y);
    display.print("GW: ");
    display.print(gatewayStr);
    display.partialUpdate();
}