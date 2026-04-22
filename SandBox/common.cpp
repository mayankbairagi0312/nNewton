#include <cmath>
#include <algorithm>

struct nColor {
    float r, g, b, a;

    nColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {
    }

   
    nColor(int r, int g, int b, int a = 255)
        : r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(a / 255.0f) {
    }

   
    explicit nColor(int index) {
        if (index < 0 || index > 255) {
            *this = Invalid;
            return;
        }
        if (index < 16) {
            static const float ansi[16][3] = {
                {0.00f,0.00f,0.00f}, {0.80f,0.00f,0.00f}, {0.00f,0.80f,0.00f}, {0.80f,0.80f,0.00f},
                {0.00f,0.00f,0.80f}, {0.80f,0.00f,0.80f}, {0.00f,0.80f,0.80f}, {0.75f,0.75f,0.75f},
                {0.50f,0.50f,0.50f}, {1.00f,0.00f,0.00f}, {0.00f,1.00f,0.00f}, {1.00f,1.00f,0.00f},
                {0.00f,0.00f,1.00f}, {1.00f,0.00f,1.00f}, {0.00f,1.00f,1.00f}, {1.00f,1.00f,1.00f}
            };
            r = ansi[index][0];
            g = ansi[index][1];
            b = ansi[index][2];
            a = 1.0f;
        }
        else if (index < 232) {
            int idx = index - 16;
            int red = (idx / 36) % 6;
            int green = (idx / 6) % 6;
            int blue = idx % 6;
            r = (red == 0) ? 0.0f : (red * 40 + 55) / 255.0f;
            g = (green == 0) ? 0.0f : (green * 40 + 55) / 255.0f;
            b = (blue == 0) ? 0.0f : (blue * 40 + 55) / 255.0f;
            a = 1.0f;
        }
        else {
            float gray = (index - 232) * 10 + 8;  
            r = g = b = gray / 255.0f;
            a = 1.0f;
        }
    }

    
    static const nColor White;
    static const nColor Red;
    static const nColor Green;
    static const nColor Blue;
    static const nColor Yellow;
    static const nColor Cyan;
    static const nColor Magenta;
    static const nColor Invalid;   // “no color”

    
    bool isValid() const {
        return !(r == -1.0f && g == -1.0f && b == -1.0f && a == -1.0f);
    }
};


const nColor nColor::White(1.0f, 1.0f, 1.0f, 1.0f);
const nColor nColor::Red(1.0f, 0.0f, 0.0f, 1.0f);
const nColor nColor::Green(0.0f, 1.0f, 0.0f, 1.0f);
const nColor nColor::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const nColor nColor::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const nColor nColor::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
const nColor nColor::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
const nColor nColor::Invalid(-1.0f, -1.0f, -1.0f, -1.0f);