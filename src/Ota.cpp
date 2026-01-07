#include "Ota.h"

#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32)) && !defined(WINDOWS)

#    include <WiFi.h>
#    include <ArduinoOTA.h>
#    include <esp_system.h>

// Optional secret header (gitignored). If present it should define
// OTA_WIFI_SSID / OTA_WIFI_PASS / OTA_HOSTNAME (and optionally OTA_PASSWORD).
#    if defined(__has_include)
#        if __has_include("OtaSecrets.h")
#            include "OtaSecrets.h"
#        endif
#    endif

#    include "System.h"

static bool ota_started = false;

static const char* ota_ssid() {
#    ifdef OTA_WIFI_SSID
    return OTA_WIFI_SSID;
#    else
    return nullptr;
#    endif
}

static const char* ota_pass() {
#    ifdef OTA_WIFI_PASS
    return OTA_WIFI_PASS;
#    else
    return nullptr;
#    endif
}

static const char* ota_hostname() {
#    ifdef OTA_HOSTNAME
    return OTA_HOSTNAME;
#    else
    return "FluidDial";
#    endif
}

void ota_setup() {
    const char* ssid = ota_ssid();
    if (!ssid || !*ssid) {
        return;
    }

    // If we just brownouted, avoid enabling WiFi immediately (it increases current draw).
    if (esp_reset_reason() == ESP_RST_BROWNOUT) {
        dbg_println("OTA: skipped due to brownout reset");
        return;
    }

    // If the user forgot to customize platformio.ini, don't stall boot.
    if (strcmp(ssid, "YOUR_WIFI_SSID") == 0) {
        return;
    }

    WiFi.mode(WIFI_STA);
    // Keep WiFi sleep enabled to reduce average current draw on marginal supplies.

    dbg_printf("OTA: connecting to WiFi SSID '%s'...\n", ssid);
    WiFi.begin(ssid, ota_pass());

    const uint32_t start_ms = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start_ms) < 10000) {
        delay(50);
    }

    if (WiFi.status() != WL_CONNECTED) {
        dbg_println("OTA: WiFi connect failed (continuing without OTA)");
        return;
    }

    dbg_printf("OTA: WiFi connected, IP %s\n", WiFi.localIP().toString().c_str());

    ArduinoOTA.setHostname(ota_hostname());

#    ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#    endif

    ArduinoOTA.onStart([]() {
        dbg_println("OTA: start");
    });
    ArduinoOTA.onEnd([]() {
        dbg_println("OTA: end");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        dbg_printf("OTA: error %u\n", (unsigned)error);
    });

    ArduinoOTA.begin();
    ota_started = true;

    dbg_printf("OTA: ready as '%s' (port 3232)\n", ota_hostname());
}

void ota_loop() {
    if (!ota_started) {
        return;
    }
    ArduinoOTA.handle();
}

#else

void ota_setup() {}
void ota_loop() {}

#endif
