#include <image.h>
#include <stdio.h>
#include <math.h>

Image::Image() {
  clear();
  threshold = DEFAULT_THRESHOLD;
}

void Image::clear() {
  for (int x=0; x<img_width; x++) {
    for (int y=0; y<img_height; y++) {
      binaryImage[x][y] = 0;
      if (x < norm_img_width && y < norm_img_height) {
        normImage[x][y] = 0;
      }
    }
  }
  width = img_width;
  height = img_height;
  bbX1 = 0;
  bbX2 = width;
  bbY1 = 0;
  bbY2 = height;
}

void Image::loadFrom265GrayArray(uint8_t imageArray[]) {
  clear();
  int l = width*height;
  int x = 0;
  int y = 0;
  for (int i=0; i<l; i++) {
    if(imageArray[i] > threshold) {
      binaryImage[x][y] = 1;
    } else {
      binaryImage[x][y] = 0;
    }
    x++;
    if (x == width) {
      y++;
      x=0;
    }
  }
}

void Image::printImage(bool original) {
  for (int y=0; y<height; y+=6) {
    for (int x=0; x<width; x+=4) {
      if (binaryImage[x][y] == true) {
          printf("*");
      } else {
          printf(" ");
      }
    }
    printf("\n");
  }
}

void Image::printNormImage() {
  for (int y=0; y<norm_img_height; y++) {
    for (int x=0; x<norm_img_width; x++) {
      if (normImage[x][y] == true) {
          printf("*");
      } else {
          printf(" ");
      }
    }
    printf("\n");
  }
}

bool Image::checkMono(int count, int compare) {
  if (count == 0 || count == compare) {
    return true;
  }
  return false;
}

void Image::findBorder(int &bb1, int &bb2, int dim1, int dim2, bool horizontal) {
  bool bb1Found = false;
  bool bb2Found = false;

  int c = dim1/2;
  for (int x=0; x<c; x++) {
    int count1 = 0;
    int count2 = 0;
    for (int y=0; y<dim2; y++) {
      if ((horizontal && binaryImage[c-x-1][y]) || (!horizontal  && binaryImage[y][c-x-1])) {
        count1++;
      }
      if ((horizontal && binaryImage[c+x][y]) || (!horizontal  && binaryImage[y][c+x])) {
        count2++;
      }
    }
    if (!bb1Found && checkMono(count1, dim2)) {
      bb1 = c-x-1;
      bb1Found = true;
    }
    if (!bb2Found && checkMono(count2, dim2)) {
      bb2 = c+x;
      bb2Found = true;
    }
  }
}

void Image::cropToBoundingBox() {
  //from center to left and right
  findBorder(bbX1, bbX2, width, height, true);
  findBorder(bbY1, bbY2, height, width, false);
  int nWidth = bbX2 - bbX1;
  int nHeight = bbY2- bbY1;

  if (width != nWidth && height != nHeight) {
    // Iterate with new Matrix until bbX and bbY are stabel
    printf("Left: %i, Right: %i, Top: %i, Bottom: %i\n", bbX1, bbX2, bbY1, bbY2);

    // Move in Matrix does not work
    for (int x=bbX1; x<bbX2; x++) {
      for (int y=bbY1; y<bbY2; y++) {
        binaryImage[x-bbX1][y-bbY1] = binaryImage[x][y];
      }
    }
    width = nWidth;
    height = nHeight;
    cropToBoundingBox();
  }
}

void Image::normalize() {
  printf("Orignial: %i, %i", width, height);
  int skipX = round(norm_img_width/(width - norm_img_width));
  int skipY = round(norm_img_height/(height - norm_img_height));
  printf("Scale: %f, %f", skipX, skipY);
  int xn = 0;
  for (int x=0; x<width; x++) {
    // Skip Every skipX

    if (x % skipX != 0) {
      int yn = 0;
      for (int y=0; y<height; y++) {
        if (y % skipY != 0) { //Skip if y/skipY
          normImage[xn][yn] = binaryImage[x][y];
          yn++;
        }
      }
      xn++;
    }
  }
}

void Image::caracterize() {
  int cntTopLeft;
  int cntTopRight;
  int cntBotomLeft;
  int cntBotomRight;

  for (int x=0; x<norm_img_width; x++) {
    for (int y=0; x<norm_img_height; y++) {
      //cVector[0]
    }
  }
}
