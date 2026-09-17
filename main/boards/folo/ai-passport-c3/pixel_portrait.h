#pragma once

#include <cstdint>
#include <initializer_list>

enum class PortraitState { Idle, Listening, Thinking, Speaking, Sad, Sleepy, Wake };

// A 48x48 code-native sprite. The same rectangle renderer drives LVGL and host previews.
// No framebuffer, image decoder, or per-pixel LVGL objects are needed.
template <typename Rect>
void DrawPixelPortrait(Rect rect, PortraitState state, unsigned frame) {
    constexpr uint32_t outline = 0x302B38, hair = 0x493E48, light = 0x75606A;
    constexpr uint32_t skin = 0xFFD7C5, shade = 0xEAAF9E, blush = 0xEC91A1;
    constexpr uint32_t white = 0xFFF7F5, shirt = 0xC5E5DE, seam = 0x80B9B1;
    constexpr uint32_t rose = 0xDE6E91, ink = 0x302534;
    auto r = [&](int x, int y, int w, int h, uint32_t c) { rect(x, y, w, h, c); };

    // Stepped hair silhouette and long side locks.
    r(17, 2, 15, 2, outline); r(12, 4, 24, 2, outline);
    r(9, 6, 29, 3, outline); r(7, 9, 33, 9, outline);
    r(6, 18, 35, 17, outline); r(4, 32, 39, 9, outline);
    r(3, 40, 41, 4, outline);
    r(16, 4, 16, 2, hair); r(12, 6, 23, 3, hair);
    r(9, 9, 29, 17, hair); r(8, 23, 31, 12, hair);
    r(6, 34, 35, 7, hair); r(5, 40, 37, 2, hair);
    r(12, 8, 3, 4, light); r(10, 12, 2, 12, light);
    r(8, 27, 2, 9, light); r(6, 37, 2, 3, light);
    r(30, 7, 4, 2, light); r(34, 10, 2, 5, light);
    r(37, 23, 2, 11, light); r(39, 36, 2, 4, light);

    // Neck, shoulders, knit top.
    r(20, 31, 9, 7, shade); r(21, 31, 7, 6, skin);
    r(14, 36, 21, 2, outline); r(10, 38, 29, 3, outline);
    r(8, 41, 33, 6, outline);
    r(14, 37, 20, 3, shirt); r(11, 40, 27, 2, shirt);
    r(9, 42, 31, 5, shirt);
    r(17, 37, 4, 2, white); r(21, 39, 8, 2, white);
    r(29, 37, 4, 2, white); r(14, 43, 2, 4, seam);
    r(34, 43, 2, 4, seam); r(19, 44, 11, 1, seam);

    // Face and asymmetric fringe.
    r(12, 19, 3, 7, shade); r(34, 19, 3, 7, shade);
    r(14, 12, 20, 16, shade); r(16, 27, 16, 4, shade);
    r(19, 31, 10, 2, shade);
    r(15, 12, 18, 14, skin); r(16, 25, 16, 4, skin);
    r(19, 29, 11, 2, skin);
    r(14, 9, 13, 6, hair); r(13, 13, 10, 4, hair);
    r(13, 17, 7, 2, hair); r(13, 19, 4, 2, hair);
    r(23, 9, 4, 4, hair); r(26, 10, 3, 3, hair);
    r(29, 11, 3, 5, hair); r(32, 13, 3, 6, hair);
    r(16, 10, 2, 5, light); r(20, 9, 2, 4, light);
    r(33, 15, 4, 1, rose); r(34, 16, 3, 1, white);
    r(33, 18, 4, 1, rose);

    const bool closed = state == PortraitState::Sleepy || frame % 16 == 15;
    const bool smile = state == PortraitState::Speaking;
    for (int x : {17, 27}) {
        r(x, 20, 5, 1, hair);
        if (closed || smile || (state == PortraitState::Wake && x == 27)) {
            r(x, 23, 5, 1, ink);
            if (!closed) r(x + 1, 22, 3, 1, ink);
        } else {
            r(x, 22, 5, 1, ink); r(x + 1, 23, 3, 3, ink);
            r(x + 2, 24, 2, 2, light); r(x + 1, 22, 2, 2, white);
        }
    }
    r(15, 26, 4, 2, blush); r(30, 26, 4, 2, blush);
    r(24, 26, 1, 1, shade);
    if (state == PortraitState::Sad) {
        r(23, 29, 3, 1, rose); r(22, 30, 1, 1, rose); r(26, 30, 1, 1, rose);
        r(33, 24, 1, 3, seam);
    } else if (smile && frame % 2 == 0) {
        r(22, 28, 5, 2, rose); r(23, 30, 3, 1, rose); r(23, 28, 3, 1, white);
    } else {
        r(22, 28, 1, 1, rose); r(26, 28, 1, 1, rose); r(23, 29, 3, 1, rose);
    }
    if (state == PortraitState::Thinking) {
        r(26, 34, 4, 7, shade); r(25, 30, 4, 7, skin);
        r(24, 29, 2, 3, skin); r(28, 35, 2, 2, shade);
        for (int i = 0; i < 3; ++i) r(36 + i * 4, 5, 2, 2, i <= int(frame % 3) ? rose : seam);
    } else if (state == PortraitState::Speaking || state == PortraitState::Wake) {
        r(37, 32, 4, 9, shade); r(38, 29, 5, 7, skin);
        r(37, 27, 1, 5, skin); r(39, 25, 1, 5, skin);
        r(41, 25, 1, 5, skin); r(43, 27, 1, 6, skin);
        r(35, 31, 3, 2, skin); r(2, 14, 2, 2, rose); r(4, 17, 2, 2, rose);
    } else if (state == PortraitState::Listening) {
        for (int i = 0; i < 3; ++i) {
            int h = 2 + ((frame + i) % 3) * 2;
            r(i * 2, 23 - h / 2, 1, h, rose);
            r(43 + i * 2, 23 - h / 2, 1, h, rose);
        }
        r(17, 36, 4, 4, skin); r(28, 36, 4, 4, skin);
    } else if (state == PortraitState::Idle) {
        r(4, 12, 2, 2, rose); r(7, 12, 2, 2, rose);
        r(4, 14, 5, 2, rose); r(5, 16, 3, 1, rose); r(6, 17, 1, 1, rose);
    }
}
