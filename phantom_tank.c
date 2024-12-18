#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include <stdio.h>
#include "stb-master/stb_image.h"
#include "stb-master/stb_image_write.h"
#include "stb-master/stb_image_resize2.h"

unsigned char* convert_to_grayscale(const char* input_path, int *w, int *h, int *c);
unsigned char* resize(unsigned char* old_img, int width, int height, int channel);
void change_gray(unsigned char* grayscale, int width, int height, int op);

int main(int argc, char* argv[]) 
{
    if (argc < 4) 
    {
        printf("Usage: %s <input_image> <output_image>\n", argv[0]);
        return 1;
    }

    const char* input_1_path = argv[1];
    const char* input_2_path = argv[2];
    const char* output = argv[3];
    int width, height, channels;

    unsigned char* grayscale_1 = convert_to_grayscale(input_1_path, &width, &height, &channels);
    unsigned char* grayscale_2 = convert_to_grayscale(input_2_path, &width, &height, &channels);

    change_gray(grayscale_1, width, height, 128);
    change_gray(grayscale_2, width, height, 0);

    // caculate new grayscale and alpha
    unsigned char* res = (unsigned char *)malloc(width * height * 2);
    for(int i = 0; i < width * height; i++)
    {
        float alpha = (float)grayscale_2[i] - (float)grayscale_1[i] + 255;
        
        // alpha
        res[i * 2 + 1] = (unsigned char)alpha;

        // grayscale
        if(alpha == 0)
            res[i * 2] = 0;
        else
            res[i * 2] = (unsigned char)((float)grayscale_2[i] * 255 / alpha); 
    }
    
    // write new image
    if (stbi_write_png(output, width, height, 2, res, width * 2) == 0)
    {
        printf("Error: Could not write image to %s\n", output);
    } 
    else 
    {
        printf("Grayscale image saved to %s\n", output);
    }
    
    free(grayscale_1);
    free(grayscale_2);
    free(res);

    return 0;
}

unsigned char* convert_to_grayscale(const char* input_path, int *w, int *h, int *c) 
{
    int width, height, channels;

    // load image
    unsigned char* image = stbi_load(input_path, &width, &height, &channels, 0);
    if (!image) // check
    {
        printf("Error: Could not load image %s\n", input_path);
        return NULL;
    }

    // check channels
    if (channels < 3) 
    {
        printf("Error: Input image must be a color image with at least 3 channels.\n");
        stbi_image_free(image);
        return NULL;
    }
    
    // 存放新的圖片的空間
    unsigned char* new_img = (unsigned char*)malloc(width * height);

    // 將彩色轉換為灰階
    for (int i = 0; i < width * height; i++) 
    {
        int r = image[i * channels + 0]; // Red
        int g = image[i * channels + 1]; // Green
        int b = image[i * channels + 2]; // Blue
        
        // 灰階公式：Gray = 0.3 * R + 0.59 * G + 0.11 * B
        new_img[i] = (unsigned char)(0.3 * r + 0.59 * g + 0.11 * b);

        // i * channels + 0 -> Red
        //              + 1 -> Green
        //              + 2 -> Blue
        //              + 3 -> alpha
    }

    stbi_image_free(image);
    // store width, height, channel
    *w = width;
    *h = height;
    *c = channels;
    return new_img;
}

// unsigned char* resize(unsigned char* old_img, int old_width, int old_height, int channel, int new_)
// {
//     unsigned char *new_img = (unsigned char*)malloc(width * height);

//     int res = 
//     //STBIRDEF unsigned char * stbir_resize_uint8_srgb( const unsigned char *input_pixels , int input_w , int input_h, int input_stride_in_bytes,
//     //                                                    unsigned char *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
//     //                                                    stbir_pixel_layout pixel_type );

// }

void change_gray(unsigned char* grayscale, int width, int height, int op)
{
    for(int i = 0; i < width * height; i++)
    {
        grayscale[i] = (unsigned char)((float)grayscale[i] / 255. * 127. + op);
        // normalize: * (127. / 255.) + op 
        // op: 決定放在[0, 127] or [128, 255] 
    }
}