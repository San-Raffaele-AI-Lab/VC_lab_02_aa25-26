#pragma once

#include <cassert>
#include <cstring>
#include <cmath>

#include <algorithm>
#include <string>
#include <vector>
#include <span>
#include <stdexcept>

using namespace std;

#include "utils.h"


// DO NOT CHANGE THIS FILE


class Image;

// DO NOT USE THE FOLLOWING FUNCTIONS DIRECTLY
// These are just helper function that allow the Image struct to function properly
// You are going to implement them in image_access.cpp

int pixel_address(const Image& im, int x, int y, int ch);  // access helper
float get_clamped_pixel(const Image& im, int x, int y, int ch); // access with clamping
void set_pixel(Image& im, int x, int y, int c, float value); // setting only in-bounds



class Image
  {

  public:
      int w=0;
      int h=0;
      int c=0;
      std::vector<float> data;

      // constructors and destructors
      Image() = default;
      Image(int size) : data(size) {};
      Image(int w, int h, int c=1) : w(w), h(h), c(c), data(w*h*c) {}

      Image(const Image& other) = default;
      Image& operator=(const Image& other) = default;
      Image(Image&& other) noexcept = default;
      Image& operator=(Image&& other) noexcept = default;
      ~Image() = default;

      // pixel access
      float& operator()(int x, int y, int ch)
        {
        check_bounds(x, y, ch);
        return data[pixel_address(*this,x,y,ch)];
        }

      float& operator()(int x, int y)
        {
        check_bounds(x, y, 0);
        return data[pixel_address(*this,x,y,0)];
        }

      const float& operator()(int x, int y, int ch) const
        {
        check_bounds(x, y, ch);
        return data[pixel_address(*this,x,y,ch)];
        }

      const float& operator()(int x, int y) const
        {
        check_bounds(x, y, 9);
        return data[pixel_address(*this,x,y,0)];
        }

            float& pixel(int x, int y, int ch)       { return operator()(x,y,ch); }
      const float& pixel(int x, int y, int ch) const { return operator()(x,y,ch); }
            float& pixel(int x, int y)               { return operator()(x,y); }
      const float& pixel(int x, int y) const         { return operator()(x,y); }

  private:

      // check input out of bounds
      void check_bounds(int x, int y, int ch) const {
            assert(ch < c && ch >= 0 && "Channel out of bounds");
            assert(x < w && x >= 0 && "X-coordinate out of bounds");
            assert(y < h && y >= 0 && "Y-coordinate out of bounds");
        }

       void check_bounds(int ch) const {
           assert(ch < c && ch >= 0 && "Channel out of bounds");
       }

  public:
      float clamped_pixel(int x, int y, int ch) const
        {
        //assert(ch<c && ch>=0);
        check_bounds(ch);
        return get_clamped_pixel(*this,x,y,ch);
        }

      float clamped_pixel(int x, int y) const
        {
        assert(c==1);
        return get_clamped_pixel(*this,x,y,0);
        }

      void set_pixel(int x, int y, int ch, float v)
        {
        assert(ch<c && ch>=0);
        ::set_pixel(*this,x,y,ch,v);
        }

      //const float* RowPtr(int row, int channel) const { return data+channel*w*h+row*w; }
      //      float* RowPtr(int row, int channel)       { return data+channel*w*h+row*w; }

      std::span<const float> RowPtr(int row, int channel) const {return std::span<const float>(data.data() + (channel * w * h) + (row * w), w);}
      std::span<      float> RowPtr(int row, int channel)       {return std::span<      float>(data.data() + (channel * w * h) + (row * w), w);}

      bool contains(float x, float y) const { return x>-0.5f && x<w-0.5f && y>-0.5f && y<h-0.5f; }

      bool is_empty(int x, int y) const
        {
        assert(x<w && x>=0 && y<h && y>=0);
        for(int q1=0;q1<c;q1++)if(pixel(x,y,q1))return false;
        return true;
        }

      bool is_nonempty_patch(int x, int y, int w=0) const
        {
        for(int q1=x-w;q1<=x+w;q1++)for(int q2=y-w;q2<=y+w;q2++)
          {
          int c1=0;
          for(int ch=0;ch<c;ch++)if(clamped_pixel(q1,q2,ch))c1++;
          if(c1==0)return false;
          }
        return true;
        }

      int size(void) const { return data.size(); }

      void clear(void) {
        std::fill(data.begin(), data.end(), 0.0f);
      }

      // member functions for inexact access
      float pixel_nearest (float x, float y, int c) const;
      float pixel_bilinear(float x, float y, int c) const;

      // member functions for modifying image
      void l1_normalize(void);
      void RGBtoHSV(void);
      void HSVtoRGB(void);
      void LCHtoRGB(void);
      void RGBtoLCH(void);
      void shift(int c, float v);
      void scale(int c, float v);
      void clamp(void);
      void feature_normalize(void);
      void feature_normalize_total(void);


      // Member functions for slicing an image
      Image get_channel(int c) const;
      void set_channel(int c,const Image& im);

      // Member functions for transforming an image
      Image rgb_to_grayscale(void) const;
      Image abs(void) const;

      // Image I/O member functions
      void save_binary(const string& filename) const;
      void load_binary(const string& filename);

      void load_image  (const string& filename);
      void save_png    (string filename) const;
      void save_image  (string filename) const;

  };

// Image I/O functions
inline Image load_binary (const string& filename) { Image im; im.load_binary(filename); return im; }
inline Image load_image  (const string& filename) { Image im; im.load_image(filename);  return im; }
inline void  save_png    (const Image& im, const string& filename) { im.save_png   (filename); }
inline void  save_image  (const Image& im, string filename) { im.save_image (filename); }
inline void  save_binary (const Image& im, string filename) { im.save_binary(filename); }


// Basic operations
Image rgb_to_grayscale(const Image& im);
Image grayscale_to_rgb(const Image& im, float r, float g, float b);
void rgb_to_hsv(Image& im);
void hsv_to_rgb(Image& im);
void lchto_rgb(Image& im);
void rgb_to_lch(Image& im);
void shift_image(Image& im, int c, float v);
void scale_image(Image& im, int c, float v);
void clamp_image(Image& im);

// Image manipulation
Image get_channel(const Image& im, int c);
bool operator ==(const Image& a, const Image& b);
int same_image(const Image& a, const Image& b);

Image sub_image(const Image& a, const Image& b);
Image add_image(const Image& a, const Image& b);
Image operator-(const Image& a, const Image& b);
Image operator+(const Image& a, const Image& b);

// Resizing
Image nearest_resize (const Image& im, int w, int h);
Image bilinear_resize(const Image& im, int w, int h);

// Filtering
Image convolve_image(const Image& im, const Image& filter, bool preserve);
Image convolve_image_fast(const Image& im, const Image& filter, bool preserve);
Image make_box_filter(int w);
Image make_highpass_filter(void);
Image make_sharpen_filter(void);
Image make_emboss_filter(void);
Image make_gaussian_filter(float sigma);
Image make_gx_filter(void);
Image make_gy_filter(void);
void feature_normalize(Image& im);
void feature_normalize_total(Image& im);

void threshold_image(Image& im, float thresh);
pair<Image,Image> sobel_image(const Image&  im);
Image colorize_sobel(const Image&  im);
Image smooth_image(const Image&  im, float sigma);
Image bilateral_filter(const Image& im, float sigma1, float sigma2);
Image bilateral_filter_fast(const Image &im, float sigma1, float sigma2);
Image histogram_equalization_hsv(const Image& im, int num_bins);
Image histogram_equalization_rgb(const Image& im, int num_bins);
