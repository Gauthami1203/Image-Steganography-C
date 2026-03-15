#include <stdio.h>          // Standard I/O operations
#include "encode.h"         // Header file containing the encode structure and function prototypes
#include "types.h"          // Header file containing custom types and status codes
#include <string.h>         // String manipulation functions
#include "common.h"         // Header file containing common constants like MAGIC_STRING

/* Function Definitions */

/* Get image size
 * Input: Image file pointer
 * Output: width * height * bytes per pixel (3 in our case for RGB)
 * Description: In BMP Image, width is stored at offset 18,
 * and height immediately after that. Each size is 4 bytes.
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    
    // Seek to the 18th byte to read the width
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an integer, 4 bytes)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an integer, 4 bytes)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return the image size in bytes (width * height * 3 for RGB pixels)
    return width * height * 3;
}

/* Get the size of the file
 * Input: File pointer
 * Output: Size of the file in bytes
 */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);  // Move to the end of the file
    unsigned int size = ftell(fptr);  // Get the position, which is the file size
    rewind(fptr);  // Rewind to the beginning of the file
    return size;
}

/* Open input and output files and handle errors
 * Inputs: Source image file, secret file, and stego image file names
 * Output: File pointers for the above files
 * Return Value: e_success or e_failure depending on file open operations
 */
Status open_files(EncodeInfo *encInfo)
{
    // Open the source image file in read mode
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Handle errors if the file cannot be opened
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");  // Print the error message
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Open the secret file in read mode
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Get the size of the secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    // Handle errors if the file cannot be opened
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");  // Print the error message
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Open the stego image file in write mode
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Handle errors if the file cannot be opened
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");  // Print the error message
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;  // Return success if all files are opened successfully
}

/* Validate command line arguments for encoding
 * Inputs: Command line arguments and structure to store validated info
 * Output: Store validated file names in structure
 * Return Value: e_success or e_failure based on validation
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check if source file has a ".bmp" extension
    char *src_file_ext = strstr(argv[2], ".bmp");  
    int src_file_len = strlen(argv[2]);

    // Validate the ".bmp" extension position
    if ((src_file_ext - argv[2] == src_file_len - 4) && src_file_ext)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("Source file is not in .bmp format\n");
        return e_failure;
    }

    // Extract file extension from the secret file name
    char *sec_file_ext = strstr(argv[3], ".");
    strcpy(encInfo->extn_secret_file, sec_file_ext);  // Copy extension to the structure
    int size = strlen(argv[3]);

    // Validate the extension position in the secret file name
    if (sec_file_ext > argv[3] && (sec_file_ext - argv[3] < size))
        encInfo->secret_fname = argv[3];
    else
    {
        printf("Secret file is not in proper file format\n");
        return e_failure;
    }

    // Check if an output file name is provided
    if (argv[4])
    {
        // Validate the output file name has a ".bmp" extension
        char *out_file_ext = strstr(argv[4], ".bmp");
        int out_file_len = strlen(argv[4]);

        // Validate the ".bmp" extension position
        if ((out_file_ext - argv[4] == out_file_len - 4) && out_file_ext)
            encInfo->stego_image_fname = argv[4];
        else
        {
            printf("Output file is not in .bmp format\n");
            return e_failure;
        }
    }
    else
        encInfo->stego_image_fname = "stego.bmp";  // Default output file name if not provided

    return e_success;  // Return success if all arguments are validated
}

/* Perform the entire encoding process
 * Inputs: Structure with file pointers and other information
 * Output: Encoded image file
 * Return Value: e_success or e_failure based on the encoding process
 */
Status do_encoding(EncodeInfo *encInfo)
{
    // Open the required files
    if (open_files(encInfo) == e_failure)
    {
        printf("Open file failure\n");
        return e_failure;
    }

    // Check if the image has enough capacity to hold the secret file
    if (check_capacity(encInfo) == e_failure)
    {
        printf("Capacity not available\n");
        return e_failure;
    }

    // Copy the BMP header from the source image to the stego image
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Header not copied\n");
        return e_failure;
    }

    // Encode the magic string in the stego image
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("Magic String not encoded\n");
        return e_failure;
    }

    // Encode the size of the secret file extension
    if (encode_secret_file_extn_size(encInfo) == e_failure)
    {
        printf("Secret File Extension Size not Encoded\n");
        return e_failure;
    }

    // Encode the secret file extension
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("Secret File Extension not Encoded\n");
        return e_failure;
    }

    // Encode the size of the secret file
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Secret File Size not Encoded\n");
        return e_failure;
    }

    // Encode the secret file data
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Secret File Data not Encoded\n");
        return e_failure;
    }

    // Copy the remaining image data to the stego image
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Remaining image data is not copied\n");
        return e_failure;
    }

    // Print the size of the stego image and source image
    printf("output image size = %d\n", get_file_size(encInfo->fptr_stego_image));
    printf("source image size = %d\n", get_file_size(encInfo->fptr_src_image));

    return e_success;  // Return success if encoding is completed
}

/* Copy the BMP header from source image to destination image
 * Input: Source and destination file pointers
 * Output: Copied header in destination file
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];  // Buffer to hold BMP header (54 bytes)
    
    rewind(fptr_dest_image);           // Rewind destination file pointer
    fseek(fptr_src_image, 0, SEEK_SET);  // Seek to the start of the source file
    fread(arr, 54, 1, fptr_src_image);  // Read the header from source file
    fwrite(arr, 54, 1, fptr_dest_image);  // Write the header to destination file

    return e_success;
}

/* Check if the source image has enough capacity to hide the secret file
 * Input: Encoding info structure
 * Output: Capacity check result
 */
Status check_capacity(EncodeInfo *encInfo)
{
    int magic_len = strlen(MAGIC_STRING);  // Length of the magic string
    int ext_sec_len = strlen(encInfo->extn_secret_file);  // Length of secret file extension
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  // Get image capacity

    // Check if image capacity is sufficient to store all data
    if (encInfo->image_capacity >= 54 + (magic_len + 4 + ext_sec_len + 4 + encInfo->size_secret_file) * 8)
    {
        return e_success;
    }

    return e_failure;  // Return failure if not enough capacity
}

/* Encode the magic string into the stego image
 * Input: Magic string and encoding info structure
 * Output: Encoded magic string in the stego image
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // Encode the magic string data to the stego image
    if (encode_data_to_image(magic_string, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error in encoding magic string\n");
        return e_failure;
    }

    return e_success;
}

/* Encode the size of the secret file extension
 * Input: Encoding info structure
 * Output: Encoded size in the stego image
 */
Status encode_secret_file_extn_size(EncodeInfo *encinfo)
{
    uint len = strlen(encinfo->extn_secret_file);  // Length of the secret file extension

    // Encode the size of the secret file extension
    if (encode_secret_file_size(len, encinfo) == e_failure)
    {
        printf("Error in encoding secret file extn size\n");
        return e_failure;
    }

    return e_success;
}

/* Encode the secret file extension into the stego image
 * Input: File extension and encoding info structure
 * Output: Encoded extension in the stego image
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encinfo)
{
    int a = strlen(encinfo->extn_secret_file);  // Length of the secret file extension

    // Encode the secret file extension data to the stego image
    if (encode_data_to_image(encinfo->extn_secret_file, a, encinfo->fptr_src_image, encinfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}

/* Encode the size of the secret file
 * Input: File size and encoding info structure
 * Output: Encoded size in the stego image
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encinfo)
{
    char buffer[32];  // Buffer to hold the file size data
    
    // Read 32 bytes from the source image
    fread(buffer, 1, 32, encinfo->fptr_src_image);

    // Encode the file size into the buffer using LSBs
    for (int i = 0; i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | ((file_size >> i) & 1);
    }

    // Write the buffer with encoded file size to the stego image
    fwrite(buffer, 1, 32, encinfo->fptr_stego_image);

    return e_success;
}

/* Encode the secret file data into the stego image
 * Input: Encoding info structure
 * Output: Encoded data in the stego image
 */
Status encode_secret_file_data(EncodeInfo *encinfo)
{
    char buffer[encinfo->size_secret_file];  // Buffer to hold the secret file data

    // Read the secret file data into the buffer
    fread(buffer, 1, sizeof(buffer), encinfo->fptr_secret);

    // Encode the secret file data to the stego image
    if (encode_data_to_image(buffer, encinfo->size_secret_file, encinfo->fptr_src_image, encinfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}

/* Encode data into the stego image by modifying the LSBs of image bytes
 * Input: Data to encode, size of data, and file pointers
 * Output: Encoded data in the stego image
 */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];  // Buffer to hold 8 bytes of the image

    // Encode each byte of data into the image
    for (int i = 0; i < size; i++)
    {
        fread(buffer, 1, 8, fptr_src_image);  // Read 8 bytes from the source image
        if (encode_byte_to_lsb(data[i], buffer) == e_failure)
        {
            printf("Error in encoding byte to LSB\n");
            return e_failure;
        }
        fwrite(buffer, 1, 8, fptr_stego_image);  // Write the modified bytes to the stego image
    }

    return e_success;
}

/* Encode a single byte into the least significant bits (LSBs) of the image buffer
 * Input: Data byte and image buffer
 * Output: Modified buffer with encoded byte
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = ((image_buffer[i] & 0xFE) | (data & 1));  // Modify the LSB
        data = (unsigned int)data >> 1;  // Shift the data byte right by 1
    }

    return e_success;
}

/* Copy the remaining image data from source to destination
 * Input: Source and destination file pointers
 * Output: Remaining image data copied to the destination file
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    // Read from source and write to destination until end of file
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;
}






    
    
    


