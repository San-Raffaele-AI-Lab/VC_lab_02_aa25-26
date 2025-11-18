// You probably don't want to edit this file
#include <cstdio>
#include <cstdlib>

#include <string>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <fstream>


#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/*
void save_image_stb(const Image& im, const string& name, int png)
  {
  unsigned char *data = (unsigned char *)calloc(im.w*im.h*im.c, sizeof(char));

  for(int k = 0; k < im.c; ++k)for(int i = 0; i < im.w*im.h; ++i)
    data[i*im.c+k] = (unsigned char) roundf((255*im.data[i + k*im.w*im.h]));

  string file=name + (png?".png":".jpg");

  int success = 0;
  if(png)success = stbi_write_png(file.c_str(), im.w, im.h, im.c, data, im.w*im.c);
  else   success = stbi_write_jpg(file.c_str(), im.w, im.h, im.c, data, 100);

  free(data);

  if(!success) fprintf(stderr, "Failed to write image %s\n", file.c_str());
  }
  */

void save_image_stb(const Image& im, string name, bool is_png)
{
    auto buffer = std::vector<unsigned char>(im.size());

    // copy from planar format(CHW) to interleaved (HWC)
    const size_t ch_size = static_cast<size_t>(im.h) * im.w;
    for (auto k = 0; k < im.c; ++k) {
        for (auto i = 0; i < ch_size; ++i) {
            buffer[i * im.c + k] = static_cast<unsigned char>(std::roundf((255 * im.data[i + k * ch_size])));
        }
    }

    name += (is_png?".png":".jpg");

    int success = 0;
    if (is_png)
        success = stbi_write_png(name.c_str(), im.w, im.h, im.c, buffer.data(), im.w*im.c);
    else
        success = stbi_write_jpg(name.c_str(), im.w, im.h, im.c, buffer.data(), 100);

    if (!success)
        throw std::ios_base::failure("Failed to write image " + name);
}

void Image::save_png(string name) const { save_image_stb(*this, std::move(name), true); }

void Image::save_image(string name) const { save_image_stb(*this, std::move(name), false); }

//
// Load an image using stb
// channels = [0..4]
// channels > 0 forces the image to have that many channels
//
/*
Image& load_image_stb(const string& filename, int channels)
  {
  int w, h, c;
  unsigned char *data = stbi_load(filename.c_str(), &w, &h, &c, channels);
  if (!data)
    {
    fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename.c_str(), stbi_failure_reason());
    exit(0);
    }

  if (channels) c = channels;

  Image im(w, h, c);

  for(int k = 0; k < c; ++k)
    for(int j = 0; j < h; ++j)
      for(int i = 0; i < w; ++i)
        {
        int dst_index = i + w*j + w*h*k;
        int src_index = k + c*i + c*w*j;
        im.data[dst_index] = (float)data[src_index]/255.f;
        }
  //We don't like alpha channels, #YOLO
  if(im.c == 4) im.c = 3;
  free(data);
  return std::move(im);
  }
  */
Image load_image_stb(const string& filename, int channels)
  {
  int w, h, c;

  auto stbi_deleter = [](unsigned char* ptr) {
    stbi_image_free(ptr);
    };

  std::unique_ptr<unsigned char[], decltype(stbi_deleter)>data(stbi_load(filename.c_str(), &w, &h, &c, channels), stbi_deleter);

  if (!data)
    throw std::ios_base::failure("Cannot load image " + filename + "\nSTB reason: " + stbi_failure_reason());

  if (channels) c = channels;

  Image im(w, h, c);

  for(int k = 0; k < c; ++k)
    for(int j = 0; j < h; ++j)
      for(int i = 0; i < w; ++i)
        {
        int dst_index = i + w*j + w*h*k;
        int src_index = k + c*i + c*w*j;
        im.data[dst_index] = static_cast<float>(data[src_index])/255.f;
        }
  //We don't like alpha channels, #YOLO
  if(im.c == 4) im.c = 3;
  return im;
  }

void Image::load_image(const string& filename) { *this=load_image_stb(filename,0); }


void Image::save_binary(const string& filename) const
  {
  FILE*fn=fopen(filename.c_str(),"wb");
  fwrite(&w,sizeof(w),1,fn);
  fwrite(&h,sizeof(h),1,fn);
  fwrite(&c,sizeof(c),1,fn);
  fwrite(data.data(),sizeof(float),size(),fn);
  fclose(fn);
  }
/*
void Image::load_binary(const std::string& filename)
  {
  int w,h,c;
  FILE*fn=fopen(filename.c_str(),"rb");
  fread(&w,sizeof(w),1,fn);
  fread(&h,sizeof(h),1,fn);
  fread(&c,sizeof(c),1,fn);
  Image im(w,h,c);
  fread(im.data,sizeof(float),im.size(),fn);
  fclose(fn);
  *this=im;
  }
  */

void Image::load_binary(const std::string& filename)
  {
    int w,h,c;
    std::ifstream file(filename, std::ios::binary);
    if(!file) {
    throw std::runtime_error("Faled to open file: " + filename);
    }

    file.read(reinterpret_cast<char*>(&w), sizeof(w));
    file.read(reinterpret_cast<char*>(&h), sizeof(h));
    file.read(reinterpret_cast<char*>(&c), sizeof(c));
    if(!file) {
        throw std::runtime_error("Cannot read header of file: " + filename);
    }

    data.resize(static_cast<size_t>(w) * h * c);
    file.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(float));
    if(!file) {
        throw std::runtime_error("Unable to read data from file: " + filename);
    }

  }




