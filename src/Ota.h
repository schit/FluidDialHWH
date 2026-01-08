#pragma once

// Optional ArduinoOTA support.
//
// This is intentionally kept small and isolated so it can be compiled out
// for non-ESP32/non-Arduino targets (e.g. the Windows SDL build).

void ota_setup();
void ota_loop();

// Status getters for UI (e.g. About screen). These always return safe values,
// even when OTA is compiled out.
bool        ota_is_enabled();
bool        ota_is_ready();
const char* ota_status_line();
const char* ota_last_error();
const char* ota_hostname_value();
const char* ota_local_ip();
