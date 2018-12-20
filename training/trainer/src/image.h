#ifndef Image_h
#define Image_h

#include <stdint.h>

//Size of input Image
static const int img_width = 160;
static const int img_height = 120;
//Size of normalized and scaled bounding box image
static const int norm_img_width = 40;
static const int norm_img_height = 30;

class Image
{
  public:
    Image();
    void printImage(bool original = false);
    void cropToBoundingBox();
    void loadFrom265GrayArray(uint8_t imageArray[]);
    void clear();

  protected:
    bool checkMono(int count, int compare);
    void findBorder(int &bb1, int &bb2, int dim1, int dim2, bool horizontal);
    int width;
    int height;
    bool binaryImage [img_width][img_height];
    bool normImage [norm_img_width][norm_img_height];
    int bbX1;
    int bbY1;
    int bbX2;
    int bbY2;
    int threshold;
};

#endif
