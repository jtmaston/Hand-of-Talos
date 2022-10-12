#pragma once

// I wouldn't recommend this shite to your worst enemy. See src/assets/noSignal.cpp for reasons why.

class Image
{
public:
    int width_;
    int height_;
    const char *pixelData_;

    Image(int w, int h, const char *pix)
    {
        width_ = w;
        height_ = h;
        pixelData_ = pix;
    }
};