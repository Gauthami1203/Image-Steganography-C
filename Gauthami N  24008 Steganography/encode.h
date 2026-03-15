#ifndef ENCODE_H            // Include guard to prevent multiple inclusions of this header file
#define ENCODE_H

#include <stdio.h>           // Standard I/O functions (e.g., fopen, fread, fwrite)
#include "types.h"           // Header file containing user-defined types and status codes

/* 
 * Define constants used in encoding process
 */
#define MAX_SECRET_BUF_SIZE 1        // Maximum buffer size for the secret file data
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)  // Maximum buffer size for image data (8 times secret buffer size)
#define MAX_FILE_SUFFIX 4           // Maximum length for file suffix (extension)

typedef struct _EncodeInfo
{
    /* Source Image Info */
    char *src_image_fname;           // File name of the source image
    FILE *fptr_src_image;            // File pointer for the source image
    uint image_capacity;             // Capacity of the source image to store secret data
    uint bits_per_pixel;             // Bits per pixel in the image
    char image_data[MAX_IMAGE_BUF_SIZE];  // Buffer to hold image data (used in encoding)

    /* Secret File Info */
    char *secret_fname;              // File name of the secret file
    FILE *fptr_secret;               // File pointer for the secret file
    char extn_secret_file[MAX_FILE_SUFFIX]; // Extension of the secret file
    char secret_data[MAX_SECRET_BUF_SIZE];  // Buffer to hold secret file data
    long size_secret_file;           // Size of the secret file

    /* Stego Image Info */
    char *stego_image_fname;         // File name of the stego (output) image
    FILE *fptr_stego_image;          // File pointer for the stego image

} EncodeInfo;

/* Function prototypes for encoding operations */

/* Check the operation type (e.g., encode or decode) */
OperationType check_operation_type(char *argv[]);

/* Read and validate command line arguments for encoding */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding process */
Status do_encoding(EncodeInfo *encInfo);

/* Open input and output files and handle errors */
Status open_files(EncodeInfo *encInfo);

/* Check if the source image has enough capacity to hide the secret file */
Status check_capacity(EncodeInfo *encInfo);

/* Get the image size from the BMP file */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get the size of a file */
uint get_file_size(FILE *fptr);

/* Copy BMP header from source image to destination image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Encode and store the magic string into the stego image */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode the size of the secret file extension */
Status encode_secret_file_extn_size(EncodeInfo *encInfo);

/* Encode the secret file extension */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode the size of the secret file */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode the secret file data into the stego image */
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode data into the image by modifying LSBs */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a single byte into the least significant bits (LSBs) of the image data buffer */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from source to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif // End of include guard
