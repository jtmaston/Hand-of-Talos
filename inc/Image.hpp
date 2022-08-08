#pragma once

// I wouldn't recommend this shite to your worst enemy. See src/assets/noSignal.cpp for reasons why.

class Image
{
public:
    int width;
    int height;
    const char *pixel_data;

    Image(int w, int h, const char *pix)
    {
        width = w;
        height = h;
        pixel_data = pix;
    }
};