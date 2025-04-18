// Header for image processing API functions (imgproc_mirror_h, etc.)
// as well as any helper functions they rely on.

#ifndef IMGPROC_H
#define IMGPROC_H

#include "image.h" // for struct Image and related functions

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img );

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
void imgproc_rgb( struct Image *input_img, struct Image *output_img );

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
void imgproc_fade( struct Image *input_img, struct Image *output_img );

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
int imgproc_kaleidoscope( struct Image *input_img, struct Image *output_img );

// TODO: add prototypes for your helper functions

// Helper function to extract the red component from a pixel.
//
// Parameters:
//   pixel - the pixel value
//
// Returns:
//   the red component of the pixel
uint32_t get_r( uint32_t pixel );

// Helper function to extract the green component from a pixel.
//
// Parameters:
//   pixel - the pixel value
//
// Returns:
//   the green component of the pixel
uint32_t get_g( uint32_t pixel );

// Helper function to extract the blue component from a pixel.
//
// Parameters:
//   pixel - the pixel value
//
// Returns:
//   the blue component of the pixel
uint32_t get_b( uint32_t pixel );

// Helper function to extract the alpha component from a pixel.
//
// Parameters:
//   pixel - the pixel value
//
// Returns:
//   the alpha component of the pixel
uint32_t get_a( uint32_t pixel );

// Helper function to create a pixel from its components.
//
// Parameters:
//   r - the red component
//   g - the green component
//   b - the blue component
//   a - the alpha component
//
// Returns:
//   the composed pixel value
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a );

// Helper function to compute the index of a pixel in the image data array.
//
// Parameters:
//   img - pointer to the Image
//   col - the column of the pixel
//   row - the row of the pixel
//
// Returns:
//   the index of the pixel in the image data array
int32_t compute_index( struct Image *img, int32_t col, int32_t row );

// Helper function to convert a pixel to grayscale.
//
// Parameters:
//   pixel - the pixel value
//
// Returns:
//   the grayscale pixel value
uint32_t to_grayscale( uint32_t pixel );

// Helper function to compute a gradient value.
//
// Parameters:
//   x   - the current value
//   max - the maximum value
//
// Returns:
//   the computed gradient value
int64_t gradient( int64_t x, int64_t max );

#endif // IMGPROC_H
