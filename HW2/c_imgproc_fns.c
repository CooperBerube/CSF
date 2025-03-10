// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// TODO: define your helper functions here

//Gets the value of r, the red of the pixel.
uint32_t get_r( uint32_t pixel ) {
  return (pixel >> 24) & 0xFF;
}

//Gets the value of g, the green of the pixel.
uint32_t get_g( uint32_t pixel ) {
  return (pixel >> 16) & 0xFF;
}

//Gets the value of b, the blue of the pixel.
uint32_t get_b( uint32_t pixel ) {
  return (pixel >> 8) & 0xFF;
}

//Gets the value of a, which is the alpha value determining opacity.
uint32_t get_a( uint32_t pixel ) {
  return pixel & 0xFF;
}

//takes rgb and a and returns a pixel 
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a ) {
  return (r << 24) | (g << 16) | (b << 8) | a;
}

//Finds the index of a specific x,y location defined
//by rol and col in an Image struct.
int32_t compute_index( struct Image *img, int32_t col, int32_t row ) {
  return row * img->width + col;
}

//Function for computing the new gray pixel for
//a specific pixel. The variable y calculates the new
//value for rgb with the input rgb.
uint32_t to_grayscale( uint32_t pixel ) {
  uint32_t r = get_r(pixel);
  uint32_t g = get_g(pixel);
  uint32_t b = get_b(pixel);
  uint32_t a = get_a(pixel);
  uint32_t y = (79 * r + 128 * g + 49 * b) >> 8;
  return make_pixel(y, y, y, a);
}

//Function for computing the total fade value for a specific picture 
//x is the location in either the column or row.
//max is the pictures width or height.
int64_t gradient( int64_t x, int64_t max ) {
  int64_t tmp = (2000000000 * x) / (1000000 * max);
  return 1000000 - ((tmp - 1000) * (tmp - 1000));
}


// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  if (input_img == NULL || output_img == NULL) {
    return;
  }

  int32_t width = input_img->width;
  int32_t height = input_img->height;

  //gets each individual pixel, converts to grayscale
  //with helper function above.
  for (int32_t y = 0; y < height; ++y) {
    for (int32_t x = 0; x < width; ++x) {
      uint32_t input_pixel = input_img->data[y * width + x];
      uint32_t output_pixel = to_grayscale(input_pixel);
      output_img->data[compute_index(output_img, x, y)] = output_pixel;
    }
  }
}

// Render an output image containing 4 replicas of the original image,
// refered to as A, B, C, and D in the following diagram:
//
//   +---+---+
//   | A | B |
//   +---+---+
//   | C | D |
//   +---+---+
//
// The width and height of the output image are (respectively) twice
// the width and height of the input image.
//
// A is an exact copy of the original input image. B has only the
// red color component values of the input image, C has only the
// green color component values, and D has only the blue color component
// values.
//
// Each of the copies (A-D) should use the same alpha values as the
// original image.
//
// This transformation always succeeds.
//
// Parameters:
//   input_img - pointer to the input Image
//   output_img - pointer to the output Image (which will have
//                width and height twice the width/height of the
//                input image)
void imgproc_rgb( struct Image *input_img, struct Image *output_img ) {
  if (input_img == NULL || output_img == NULL) {
        return;
  }
  
  int32_t input_width = input_img->width;
  int32_t input_height = input_img->height;

  //Iterates through each pixel getting the rgb 
  //components and seperating them into 
  //the output image which has a sector for each
  //of the rgb components
  for (int32_t y = 0; y < input_height; ++y) {
    for (int32_t x = 0; x < input_width; ++x) {
      uint32_t input_pixel = input_img->data[compute_index(input_img, x, y)];

      uint32_t r = get_r(input_pixel);
      uint32_t g = get_g(input_pixel);
      uint32_t b = get_b(input_pixel);
      uint32_t a = get_a(input_pixel);

      uint32_t pixel_unchanged = make_pixel(r, g, b, a);
      uint32_t pixel_red_shift = make_pixel(r, 0, 0, a);
      uint32_t pixel_green_shift = make_pixel(0, g, 0, a);
      uint32_t pixel_blue_shift = make_pixel(0, 0, b, a);

      output_img->data[compute_index(output_img, x, y)] = pixel_unchanged;
      output_img->data[compute_index(output_img, x + input_width, y)] = pixel_red_shift;
      output_img->data[compute_index(output_img, x, y + input_height)] = pixel_green_shift;
      output_img->data[compute_index(output_img, x + input_width, y + input_height)] = pixel_blue_shift;
    }
  }
}

// Render a "faded" version of the input image.
//
// See the assignment description for an explanation of how this transformation
// should work.
//
// This transformation always succeeds.
//
// Parameters:
//   input_img - pointer to the input Image
//   output_img - pointer to the output Image
void imgproc_fade( struct Image *input_img, struct Image *output_img ) {
  if (input_img == NULL || output_img == NULL) {
    return;
  }
  
    int32_t width = input_img->width;
    int32_t height = input_img->height;

    //Iterates through each pixel, gets its rgb parts and then computes appropriate fade value.
    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {

            uint32_t input_pixel = input_img->data[compute_index(input_img, x, y)];

            uint32_t r = get_r(input_pixel);
            uint32_t g = get_g(input_pixel);
            uint32_t b = get_b(input_pixel);
            uint32_t a = get_a(input_pixel);

            //computes fade constant values based on location and total picture size
            int64_t tr = gradient(y, height);
            int64_t tc = gradient(x, width);

            r = (tr * tc * r) / 1000000000000;
            g = (tr * tc * g) / 1000000000000;
            b = (tr * tc * b) / 1000000000000;

            uint32_t output_pixel = make_pixel(r, g, b, a);
            output_img->data[compute_index(output_img, x, y)] = output_pixel;
        }
    }

}

// Render a "kaleidoscope" transformation of input_img in output_img.
// The input_img must be square, i.e., the width and height must be
// the same. Assume that the input image is divided into 8 "wedges"
// like this:
//
//    +----+----+
//    |\ A |   /|
//    | \  |  / |
//    |  \ | /  |
//    | B \|/   |
//    +----+----+
//    |   /|\   |
//    |  / | \  |
//    | /  |  \ |
//    |/   |   \|
//    +----+----+
//
// The wedge labeled "A" is the one that will be replicated 8 times
// in the output image. In the output image,
//
//    Wedge A will have exactly the same pixels as wedge A in
//    the input image, in the same positions.
//
//    Wedge B will have exactly the same pixels as wedge A, but the
//    column and row are transposed. (I.e., it is a reflection across
//    the diagonal.)
//
//    All other wedges are mirrored versions of A such that the overall
//    image is symmetrical across all four lines dividing the image
//    (vertical split, horizontal split, and the two diagonal splits.)
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
//
// Returns:
//   1 if successful, 0 if the transformation fails because the
//   width and height of input_img are not the same.
int imgproc_kaleidoscope( struct Image *input_img, struct Image *output_img ) {
  if (input_img == NULL || output_img == NULL) {
    return 0;
  }
  
  int32_t input_width = input_img->width;
  int32_t input_height = input_img->height;

  if (input_height != input_width) {
    return 0;
  }
  //Checks to see if the picture is odd or even
  int32_t odd_factor = 0;
  if (input_width % 2 == 1) {
    odd_factor = 1;
  }
  /*If odd you kneed to add one to input width to copy it over
  * successfully. Variable l stores the proper length for the temp array.
  */
  int32_t wedge_length = (input_width + odd_factor)/2;
  int32_t l = wedge_length*2;
  // Temp Array allows you to copy for odd and cut off the final row and column at the end.
  int32_t temp_result[l][l];
  

  /* copies the top corner known as A over to t the rest of the appropriate
  *  tiles in the temp array 
  */
  for (int32_t y = 0; y < wedge_length; y++) {
    for (int32_t x = y; x < wedge_length; x++) {
      uint32_t input_pixel = input_img->data[compute_index(input_img, x, y)];
      uint32_t output_pixel = input_pixel;

      temp_result[x][y] = output_pixel;
      temp_result[y][x] = output_pixel;

      temp_result[l-x-1][y] = output_pixel;
      temp_result[y][l-x-1] = output_pixel;

      temp_result[x][l-y-1] = output_pixel;
      temp_result[l-y-1][x] = output_pixel;

      temp_result[l-x-1][l-y-1] = output_pixel;
      temp_result[l-y-1][l-x-1] = output_pixel;
    }
  }

  //Transfers the temp 2D array over to the ouput image leaving out the extra row if odd.
  for (int32_t x = 0; x < output_img->width; x++) {
    for (int32_t y = 0; y < output_img->height; y++) {
      output_img->data[compute_index(output_img, x, y)] = temp_result[x][y];
    }
  }
  return 1;

}