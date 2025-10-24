#pragma once
#include <Inkplate.h>

class Page {
public:
    static constexpr int tankCount = 5;
    static constexpr int bilgeCount = 6;
    static const int rect_a_x[tankCount];
    static const int rect_b_x[tankCount];
    static const int rect_a_y;
    static const int rect_b_y;
    static const int bilge_x[bilgeCount];
    static const int bilge_y[bilgeCount];
    static constexpr int bilge_radius = 28;

    Page(Inkplate& disp);

    // Tank and bilge state
    void setTank(int idx, int pct);
    void setBilge(int idx, int level);

    // Battery, temp, hum, air pressure values
    void setTemp(float t) { tempValue = t; }
    void setHum(float h) { humValue = h; }
    void setAirPressure(float p) { airPressureValue = p; }
    void setBat(float b1, float b2, float b3) { batValue1 = b1; batValue2 = b2; batValue3 = b3; }
    void setSeaTemp(float t) { seaTempValue = t; }
    void setOutTemp(float t) { outTempValue = t; }
    void setOutHum(float h) { outHumValue = h; }
    void setOutPres(float p) { outPresValue = p; }

    // Drawing
    void draw();
    void drawTank(int idx);
    void drawBilge(int idx);

    // Partial update methods
    void updateTemp();
    void updateHum();
    void updateBat();
    void updateAirPressure();
    void updateSeaTemp();
    void updateGateway();
    // Output sensor values shown above the main temp/hum/pressure (info page)
    void updateOutTemp();
    void updateOutHum();
    void updateOutPres();
    
    // Data members
    int percent[tankCount];
    int bilgeLevel[bilgeCount];

    float tempValue = 0.0;
    float humValue = 0.0;
    float airPressureValue = 0.0;
    float batValue1 = 0.0, batValue2 = 0.0, batValue3 = 0.0;
    float seaTempValue = 0.0;
    // new output sensor members (displayed above temp/hum/pressure)
    float outTempValue = 0.0;
    float outHumValue = 0.0;
    float outPresValue = 0000.0;

    char gatewayStr[32] = "-";
    void setGateway(const char* gw) { strncpy(gatewayStr, gw, sizeof(gatewayStr) - 1); gatewayStr[sizeof(gatewayStr) - 1] = '\0'; }

private:
    Inkplate& display;
};
