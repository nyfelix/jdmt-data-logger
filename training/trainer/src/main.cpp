// This is the main application for the Neurnal Network Trainer
// This applicaiton in a first run is desingt to be run on the
// native OS, not on Arduino. However all data processing is
// encapsulated in the image and brain classes. So they could be
// used in an Arduino environement, too.
// Crossplatform compling is prepared, but a major switch has to be
// implemented here.
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <image.h>
#include <sample0001.h>

// Implementation for native OS

void processImage(uint8_t map[]) {
  Image* imgOK = new Image();
  imgOK->loadFrom265GrayArray(map);
  //imgOK->printImage(true);
  imgOK->cropToBoundingBox();
  imgOK->normalize();
  imgOK->classify();
  imgOK->printNormImage();
}

int main()
{
    printf("This is the Image Trainer v.0.1\n");
    processImage(sample0023_map);
    processImage(sample0024_map);
    processImage(sample0025_map);
    processImage(sample0026_map);
    processImage(sample0027_map);

    /*
    printf("Load Image NOK ...\n");
    Image* imgNOK = new Image();
    imgNOK->loadFrom265GrayArray(sample0002_map);
    imgNOK->cropToBoundingBox();
    imgNOK->normalize();
    imgNOK->classify();
    //imgNOK->printNormImage();
    // Classiifcation: Momenten-Invarianten berechnung
    */
    return 0;
}
