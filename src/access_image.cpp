#include "image.h"
#include <cstring>
#include <chrono>
#include <iostream>

// HW0 #1
// const Image& im: input image
// int x,y: pixel coordinates
// int ch: channel of interest
// returns the 0-based location of the pixel value in the data array
int pixel_address(const Image &im, int x, int y, int ch) {

    int pixel_idx = 0;
    int ch_size = im.w * im.h;
    pixel_idx = (ch * ch_size) + (y * im.w) + x;
    return pixel_idx;
}

// HW0 #1
// const Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
// returns the value of the clamped pixel at channel ch
float get_clamped_pixel(const Image &im, int x, int y, int ch) {
    // TODO: clamp the coordinates and return the correct pixel value

    if (x < 0) x = 0;
    if (x >= im.w) x = im.w - 1;
    if (y < 0) y = 0;
    if (y >= im.h) y = im.h - 1;
    if (ch < 0) ch = 0;
    if (ch >= im.c) ch = im.c - 1;

    return im.data[pixel_address(im, x, y, ch)];
    //NOT_IMPLEMENTED();
    //return 0.0;
}

// TODO: modify for unexpected values
// HW0 #1
// Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
void set_pixel(Image &im, int x, int y, int c, float value) {
    if ((x >= 0 && x < im.w) && (y >= 0 && y < im.h) && (c >= 0 && c < im.c))
        im.data[pixel_address(im, x, y, c)] = value;
}
