// msDisplay.h
#ifndef MS_DISPLAY_H_
#define MS_DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

// Lifecycle
void msDisplay_init();
void msDisplay_shutdown();

// Frame control
void msDisplay_clear();  // clear frame/backbuffer
void msDisplay_update(); // present frame

// Drawing
void msDisplay_drawCell();
void msDisplay_drawMinefield();

// Optional: handle window resize if supported by backend (no-op on embedded)
void msDisplay_resize();

#endif // MS_DISPLAY_H_