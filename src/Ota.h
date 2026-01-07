#pragma once

// Optional ArduinoOTA support.
//
// This is intentionally kept small and isolated so it can be compiled out
// for non-ESP32/non-Arduino targets (e.g. the Windows SDL build).

void ota_setup();
void ota_loop();
