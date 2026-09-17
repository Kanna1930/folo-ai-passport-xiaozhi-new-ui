#include "pixel_portrait.h"
#include <fstream>
#include <iomanip>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: preview_folo_portrait OUTPUT.svg\n";
        return 1;
    }
    std::ofstream out(argv[1]);
    if (!out) return 1;
    out << "<svg xmlns='http://www.w3.org/2000/svg' width='1024' height='740' viewBox='0 0 1024 740'>"
           "<rect width='1024' height='740' fill='#e7eeee'/>"
           "<g font-family='sans-serif' fill='#30323a'>"
           "<text x='24' y='34' font-size='20'>FOLO / Pixel character - source-rendered preview</text>";
    const PortraitState states[] = {PortraitState::Idle, PortraitState::Listening,
        PortraitState::Thinking, PortraitState::Speaking, PortraitState::Sad,
        PortraitState::Wake, PortraitState::Sleepy};
    const char* titles[] = {"Idle", "Listening", "Thinking", "Speaking", "Low battery",
                            "Wake", "Sleep expression"};
    const char* captions[] = {"Hello, I'm XiaoZhi", "I'm listening", "One moment...", "Speaking...",
                             "Please charge", "Welcome back", "Backlight turns OFF"};
    for (int i = 0; i < 7; ++i) {
        int x = 16 + i % 4 * 252, y = 52 + i / 4 * 340;
        out << "<g transform='translate(" << x << ',' << y << ")'>"
            << "<rect width='240' height='320' fill='#f3f5f5'/>"
            << "<text x='12' y='22' font-size='15'>Wi-Fi</text>"
            << "<text x='85' y='22' font-size='17'>10:24</text>"
            << "<text x='178' y='22' font-size='17'>" << (i == 4 ? "15%" : "86%") << "</text>"
            << "<text x='120' y='47' text-anchor='middle' font-size='17' fill='#28776d'>"
            << titles[i] << "</text>";
        DrawPixelPortrait([&](int px, int py, int w, int h, uint32_t color) {
            out << "<rect x='" << 24 + px * 4 << "' y='" << 50 + py * 4 << "' width='"
                << w * 4 << "' height='" << h * 4 << "' fill='#" << std::hex << std::setw(6)
                << std::setfill('0') << color << std::dec << "'/>";
        }, states[i], 0);
        out << "<text x='120' y='278' text-anchor='middle' font-size='17'>" << captions[i]
            << "</text></g>";
    }
    out << "<text x='788' y='478' font-size='18'>48 x 48 sprite</text>"
           "<text x='788' y='514' font-size='16'>192 x 192 on device</text>"
           "<text x='788' y='550' font-size='16'>7 expression states</text>"
           "<text x='788' y='586' font-size='16'>No image framebuffer</text>"
           "<text x='788' y='634' font-size='13'>Preview labels are illustrative.</text>"
           "<text x='788' y='656' font-size='13'>Firmware uses Chinese text.</text>"
           "</g></svg>";
    return out.good() ? 0 : 1;
}
