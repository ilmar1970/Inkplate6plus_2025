#include "Display.h"

std::pair<std::vector<DisplayCoordinates>, uint16_t> Display::Drawable::readTouchData() {
    std::vector<DisplayCoordinates> touchEvents;
    uint16_t n = 0;
    uint16_t x[2], y[2];
    if (display->tsAvailable()){
        n = display->tsGetData(x, y);
        for (int i = 0; i < n && i < 2; i++) {
            touchEvents.push_back({x[i], y[i]});
        }
    }
    return {touchEvents, n};
}