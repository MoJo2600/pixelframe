
#include "Arduino.h"
#include <stdio.h>
#include <lib/stdinout.h>

// On non-AVR platforms, use the somewhat more complex "cookie"-based
// approach to custom streams. This is a GNU-specific extension to libc.
static ssize_t uart_putchar (void *, const char *buf, size_t len) {
    auto res = Serial.write(buf, len);
    // Since the C interface has no meaningful way to flush (fflush() is a
    // no-op on AVR since stdio does not introduce any buffering), just flush
    // every byte.
    Serial.flush();
    return res;
}

static cookie_io_functions_t functions = {
    .read = NULL,
    .write = uart_putchar,
    .seek = NULL,
    .close = NULL
};

void hal_printf_init() {
    stdout = fopencookie(NULL, "w", functions);
    // Disable buffering, so the callbacks get called right away
    setbuf(stdout, nullptr);
}
