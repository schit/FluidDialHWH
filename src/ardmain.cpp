// Copyright (c) 2023 -	Barton Dring
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "System.h"
#include "FileParser.h"
#include "Scene.h"
#include "AboutScene.h"
#include "Ota.h"
#include "Diagnostics.h"

#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
#    include <esp_system.h>
#endif

extern void base_display();
extern void show_logo();

extern const char* git_info;

extern AboutScene aboutScene;

void setup() {
    init_system();

    diag_init();

    const int pref_brightness = aboutScene.getBrightness();
    int       brightness      = pref_brightness;
#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
    const esp_reset_reason_t reset_reason = esp_reset_reason();
    if (reset_reason == ESP_RST_BROWNOUT) {
        // Brownout usually means the supply sagged; reduce load so the unit can boot.
        if (brightness > 32) {
            brightness = 32;
        }
    }
#endif

    diag_note_brightness(pref_brightness, brightness);

    display.setBrightness(brightness);

    show_logo();
    delay_ms(2000);  // view the logo and wait for the debug port to connect

#if defined(ARDUINO) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
    if (esp_reset_reason() == ESP_RST_BROWNOUT) {
        display.setTextColor(RED);
        display.setTextDatum(top_left);
        display.drawString("BROWNOUT (weak 5V)", 2, 2);
        dbg_println("Reset reason: brownout");
        delay_ms(500);
    }
#endif

    base_display();

    dbg_printf("FluidNC Pendant %s\n", git_info);

    // Optional ArduinoOTA support (enabled only if OTA_WIFI_SSID is set at build time).
    ota_setup();

    fnc_realtime(StatusReport);  // Kick FluidNC into action

    // init_file_list();

    extern Scene* initMenus();
    activate_scene(initMenus());
}

void loop() {
    fnc_poll();         // Handle messages from FluidNC
    dispatch_events();  // Handle dial, touch, buttons
    ota_loop();         // Handle OTA update requests (if enabled)
}
