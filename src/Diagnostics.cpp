#include "Diagnostics.h"

#include <string>

#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
#    include <esp_system.h>
#endif

static int pref_brightness_snapshot    = -1;
static int applied_brightness_snapshot = -1;

#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
static esp_reset_reason_t reset_reason_snapshot = ESP_RST_UNKNOWN;
#else
static int reset_reason_snapshot = 0;
#endif

void diag_init() {
#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
    reset_reason_snapshot = esp_reset_reason();
#else
    reset_reason_snapshot = 0;
#endif
}

void diag_note_brightness(int pref_brightness, int applied_brightness) {
    pref_brightness_snapshot    = pref_brightness;
    applied_brightness_snapshot = applied_brightness;
}

const char* diag_reset_reason_str() {
#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
    switch (reset_reason_snapshot) {
        case ESP_RST_POWERON:
            return "poweron";
        case ESP_RST_EXT:
            return "external";
        case ESP_RST_SW:
            return "software";
        case ESP_RST_PANIC:
            return "panic";
        case ESP_RST_INT_WDT:
            return "int_wdt";
        case ESP_RST_TASK_WDT:
            return "task_wdt";
        case ESP_RST_WDT:
            return "wdt";
        case ESP_RST_DEEPSLEEP:
            return "deepsleep";
        case ESP_RST_BROWNOUT:
            return "brownout";
        case ESP_RST_SDIO:
            return "sdio";
        default:
            return "unknown";
    }
#else
    return "n/a";
#endif
}

bool diag_reset_was_brownout() {
#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
    return reset_reason_snapshot == ESP_RST_BROWNOUT;
#else
    return false;
#endif
}

int diag_pref_brightness() {
    return pref_brightness_snapshot;
}

int diag_applied_brightness() {
    return applied_brightness_snapshot;
}

bool diag_brightness_was_clamped() {
    if (pref_brightness_snapshot < 0 || applied_brightness_snapshot < 0) {
        return false;
    }
    return applied_brightness_snapshot < pref_brightness_snapshot;
}
