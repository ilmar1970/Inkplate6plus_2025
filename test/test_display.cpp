#include <Arduino.h>
#include <unity.h>

// Mocked Display Class for Unit Testing
class MockDisplay {
public:
    bool initialized = false;
    String lastText = "";

    bool begin() {
        initialized = true;
        return initialized;
    }

    void clearDisplay() {
        lastText = "";
    }

    void printText(const char* text, int x, int y, int scale) {
        lastText = String(text);
    }

    void display() {
        // Simulate sending data to the display
    }
};

MockDisplay display;

void setUp() {
    // Code before each test
    display.begin();
}

void tearDown() {
    // Code after each test
    display.clearDisplay();
}

void test_display_initialization() {
    TEST_ASSERT_TRUE(display.initialized);
}

void test_display_print_text() {
    display.printText("Hello, World!", 0, 0, 1);
    TEST_ASSERT_EQUAL_STRING("Hello, World!", display.lastText.c_str());
}

void test_display_clear() {
    display.printText("Clear me!", 0, 0, 1);
    display.clearDisplay();
    TEST_ASSERT_EQUAL_STRING("", display.lastText.c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_display_initialization);
    RUN_TEST(test_display_print_text);
    RUN_TEST(test_display_clear);
    UNITY_END();
}

void loop() {}