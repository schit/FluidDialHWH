#pragma once

// Small runtime diagnostics snapshot used by UI (e.g. About screen).
// On non-ESP builds these functions return safe defaults.

void diag_init();
void diag_note_brightness(int pref_brightness, int applied_brightness);

const char* diag_reset_reason_str();
bool        diag_reset_was_brownout();

int  diag_pref_brightness();
int  diag_applied_brightness();
bool diag_brightness_was_clamped();
