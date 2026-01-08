#include "Ota.h"

#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32)) && !defined(WINDOWS)

#    include <WiFi.h>
#    include <ArduinoOTA.h>
#    include <esp_system.h>
#    include <string>

// Optional secret header (gitignored). If present it should define
// OTA_WIFI_SSID / OTA_WIFI_PASS / OTA_HOSTNAME (and optionally OTA_PASSWORD).
#    if defined(__has_include)
#        if __has_include("OtaSecrets.h")
#            include "OtaSecrets.h"
#        endif
#    endif

#    include "System.h"

static bool ota_enabled = false;
static bool ota_started = false;
static std::string ota_status;
static std::string ota_error;
static std::string ota_ip;
static std::string ota_host;

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
        ota_enabled = false;
        ota_status  = "OTA disabled (no WiFi SSID)";
        return;
    }

    ota_enabled = true;
    ota_host    = ota_hostname();
    ota_status  = "OTA init";
    ota_error.clear();
    ota_ip.clear();

    // If we just brownouted, avoid enabling WiFi immediately (it increases current draw).
    if (esp_reset_reason() == ESP_RST_BROWNOUT) {
        dbg_println("OTA: skipped due to brownout reset");
        ota_status = "OTA skipped (brownout reset)";
        ota_error  = "brownout: WiFi/OTA deferred";
        return;
    }

    // If the user forgot to customize platformio.ini, don't stall boot.
    if (strcmp(ssid, "YOUR_WIFI_SSID") == 0) {
        ota_enabled = false;
        ota_status  = "OTA disabled (secrets not set)";
        return;
    }

    WiFi.mode(WIFI_STA);
    // Keep WiFi sleep enabled to reduce average current draw on marginal supplies.

    dbg_printf("OTA: connecting to WiFi SSID '%s'...\n", ssid);
    WiFi.begin(ssid, ota_pass());

    ota_status = "WiFi connecting";

    const uint32_t start_ms = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start_ms) < 10000) {
        delay(50);
    }

    if (WiFi.status() != WL_CONNECTED) {
        dbg_println("OTA: WiFi connect failed (continuing without OTA)");
        ota_status = "WiFi connect failed";
        ota_error  = "WiFi connect failed";
        return;
    }

    ota_ip     = WiFi.localIP().toString().c_str();
    ota_status = "WiFi connected";
    dbg_printf("OTA: WiFi connected, IP %s\n", ota_ip.c_str());

    ArduinoOTA.setHostname(ota_hostname());

#    ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#    endif

    ArduinoOTA.onStart([]() {
        dbg_println("OTA: start");
        ota_status = "OTA in progress";
    });
    ArduinoOTA.onEnd([]() {
        dbg_println("OTA: end");
        ota_status = "OTA complete";
    });
    ArduinoOTA.onError([](ota_error_t error) {
        dbg_printf("OTA: error %u\n", (unsigned)error);
        ota_status = "OTA error";
        ota_error  = std::string("ota error ") + std::to_string((unsigned)error);
    });

    ArduinoOTA.begin();
    ota_started = true;

    ota_status = "OTA ready";

    dbg_printf("OTA: ready as '%s' (port 3232)\n", ota_hostname());
}

void ota_loop() {
    if (!ota_started) {
        return;
    }
    ArduinoOTA.handle();
}

bool ota_is_enabled() {
    return ota_enabled;
}

bool ota_is_ready() {
    return ota_started;
}

const char* ota_status_line() {
    return ota_status.empty() ? "" : ota_status.c_str();
}

const char* ota_last_error() {
    return ota_error.empty() ? "" : ota_error.c_str();
}

const char* ota_hostname_value() {
    return ota_host.empty() ? "" : ota_host.c_str();
}

const char* ota_local_ip() {
    return ota_ip.empty() ? "" : ota_ip.c_str();
}

#else

void ota_setup() {}
void ota_loop() {}

bool ota_is_enabled() {
    return false;
}

bool ota_is_ready() {
    return false;
}

const char* ota_status_line() {
    return "";
}

const char* ota_last_error() {
    return "";
}

const char* ota_hostname_value() {
    return "";
}

const char* ota_local_ip() {
    return "";
}

#endif
