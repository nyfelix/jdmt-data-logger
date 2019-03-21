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
  int scaleY = 6;
  int scaleX = 4;
  if (original) {
    scaleY = 1;
    scaleX = 1;
  }
  for (int y=0; y<height; y+=scaleY) {
    for (int x=0; x<width; x+=scaleX) {
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
  int nWidth = fmin(bbX2 - bbX1, width);
  int nHeight = fmin(bbY2 - bbY1, height);

  if (width != nWidth || height != nHeight) {
    // Iterate with new Matrix until bbX and bbY are stabel
    //printf("Left: %i, Right: %i, Top: %i, Bottom: %i\n", bbX1, bbX2, bbY1, bbY2);

    // Transform Matrx to Bounding Box
    for (int x=bbX1; x<bbX2; x++) {
      for (int y=bbY1; y<bbY2; y++) {
        binaryImage[x-bbX1][y-bbY1] = binaryImage[x][y];
      }
    }
    width = nWidth;
    height = nHeight;
    //printf("Width: %i, Height: %i\n", width, height);
    cropToBoundingBox();
  }
}

// Algorithm accordung to: https://stackoverflow.com/questions/299267/image-scaling-and-rotating-in-c-c
void Image::normalize() {
  //printf("Orignial: %i, %i -> Norm: %i, %i\n", width, height, norm_img_width, norm_img_height);
  for (int dy = 0; dy < norm_img_height; dy++) {
    for (int dx = 0; dx < norm_img_width; dx++) {
      int blockX = int(1.0*dx*width/norm_img_width);
      int blockY = int(1.0*dy*height/norm_img_height);
      normImage[dx][dy] = binaryImage[blockX][blockY];
    }
  }
}


void Image::classify() {
  int cntTopLeft=0;
  int cntTopRight=0;
  int cntBotomLeft=0;
  int cntBotomRight=0;
  int cWhite=0;
  int border = 6;

  for (int x=border; x<norm_img_width-2*border; x++) {
    for (int y=border; y<norm_img_height-2*border; y++) {
      cWhite += normImage[x][y];
      if (x < 0.5*norm_img_width) {
        if (y < 0.5*norm_img_height) {
          cntTopLeft += normImage[x][y];
        } else {
          cntBotomLeft += normImage[x][y];
        }
      } else {
        if (y < 0.5*norm_img_height) {
          cntTopRight += normImage[x][y];
        } else {
          cntBotomRight += normImage[x][y];
        }
      }
    }
  }
  printf("cntTopLeft: %i, cntTopRight: %i, cntBotomLeft: %i, cntBotomRight: %i\n", cntTopLeft, cntTopRight, cntBotomLeft, cntBotomRight);
}
