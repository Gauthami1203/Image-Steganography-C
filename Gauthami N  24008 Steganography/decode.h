#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user-defined types

/* 
 * Structure to store information required for
 * decoding a secret file from a source image.
 * Info about output and intermediate data is
 * also stored in this structure.
 */

#define MAX_SECRET_BUF_SIZE 1  // Maximum buffer size for secret data (1 byte at a time)
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)  // Buffer size for image data (8 bits for 1 byte of secret data)
#define MAX_FILE_SUFFIX 4  // Maximum size for file extension suffix (e.g., ".txt", ".png")

// Structure to hold information required for decoding
typedef struct _DecodeInfo
{
    /* Output file information */
    char *output_fname;  // Name of the output file
    FILE *fptr_output;  // File pointer for the output file
    char extn_output_file[MAX_FILE_SUFFIX];  // Extension of the output file (e.g., "txt")
    char output_data[MAX_SECRET_BUF_SIZE];  // Buffer to hold decoded data temporarily
    int size_output_file;  // Size of the decoded output file
    int extn_output_size;  // Size of the file extension (e.g., 3 for "txt")

    /* Stego Image Information */
    char *stego_image_fname;  // Name of the stego (encoded) image file
    FILE *fptr_stego_image;  // File pointer for the stego image file
    uint image_capacity;  // Capacity of the image in terms of how much data it can hide
    char image_data[MAX_IMAGE_BUF_SIZE];  // Buffer to hold data read from the image

    // Temporary size used during decoding
    int data_size;  // Temporary variable to store size of data being processed

} DecodeInfo;

/* Function prototypes for decoding operations */

// Function to read and validate decoding arguments from command-line arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// Function to perform the decoding process
Status do_decoding(DecodeInfo *decInfo);

// Function to open the stego image file
Status open_stego_image(DecodeInfo *decInfo);

// Function to decode the magic string (used to verify if the image is encoded)
Status decode_magic_string(int magic_len, DecodeInfo *decInfo); 

// Function to decode a byte from the least significant bits (LSBs) of the image data
Status decode_byte_from_lsb(char *data, DecodeInfo *decInfo);

// Function to decode the size of the output file extension from the image data
Status decode_extn_size_of_output(DecodeInfo *decInfo);

// Function to decode an integer size value from the least significant bits (LSBs) of the image data
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo);

// Function to decode the file extension of the output file
Status decode_output_extn(DecodeInfo *decInfo);

// Function to open the output file where the decoded data will be written
Status open_output_file(DecodeInfo *decInfo);

// Function to get the size of the output file by decoding it from the image data
Status get_output_file_size(DecodeInfo *decInfo);

// Function to decode the actual file data from the stego image and write it to the output file
Status decode_output_file_data(DecodeInfo *decInfo);

#endif  // End of DECODE_H guard
