// Include all the necessary headers, including user-defined headers
#include <stdio.h>          // Standard I/O operations
#include "decode.h"         // Header file containing the decode structure and function prototypes
#include "types.h"          // Header file containing custom types and status codes
#include <string.h>         // String manipulation functions
#include "common.h"         // Header file containing common constants like MAGIC_STRING

/* Validate and store arguments passed in structure
 * Inputs: command line arguments, structure pointer
 * Output: store source file name, output file name
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the source file (stego image) has a ".bmp" extension
    char *stego_file_ext = strstr(argv[2], ".bmp");  // Find the ".bmp" extension in the file name
    int stego_file_len = strlen(argv[2]);            // Get the length of the file name

    // Verify that the ".bmp" extension is at the end of the file name
    if ((stego_file_ext - argv[2] == stego_file_len - 4) && stego_file_ext)
    {
        decInfo->stego_image_fname = argv[2];   // Store the stego image file name in the structure
    }
    else
    {
        printf("Encoded Image file is not in .bmp format\n");
        return e_failure;  // Return failure if the file is not a valid BMP file
    }

    // Check if an argument is passed for the output file name
    if (argv[3])
    {
        decInfo->output_fname = argv[3];  // Store the output file name in the structure
    }
    // If the output file name is not passed, assign a default file name "decoded.txt"
    else
    {
        decInfo->output_fname = "decoded.txt";
    }
    return e_success;  // Return success if arguments are validated and stored successfully
}

// Function to open the stego image file
Status open_stego_image(DecodeInfo *decInfo)
{
    // Open the stego image file in read mode
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Handle errors if the file cannot be opened
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");  // Print error message
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;  // Return failure if the file cannot be opened
    }
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);  // Skip the BMP header (54 bytes)
    return e_success;  // Return success after opening the file and skipping the header
}

// Function to perform the entire decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    // Step 1: Open the stego image file
    if (open_stego_image(decInfo) == e_failure)
    {
        printf("Open Decode file failure\n");
        return e_failure;  // Return failure if the file could not be opened
    }
    printf("Open Decode file success\n");

    // Step 2: Decode the magic string (used to identify if the image contains encoded data)
    if (decode_magic_string(strlen(MAGIC_STRING), decInfo) == e_failure)
    {
        return e_failure;  // Return failure if the magic string could not be decoded
    }
    printf("Magic string is decoded\n");

    // Step 3: Decode the size of the output file extension
    if (decode_extn_size_of_output(decInfo) == e_failure)
    {
        printf("Secret File Extension Size not decoded\n");
        return e_failure;  // Return failure if the extension size could not be decoded
    }
    printf("Secret File Extension Size decoded\n");

    // Step 4: Decode the output file extension
    if (decode_output_extn(decInfo) == e_failure)
    {    
        printf("Secret File Extension not decoded\n");
        return e_failure;  // Return failure if the file extension could not be decoded
    }
    printf("Secret File Extension decoded\n");

    // Step 5: Open the output file where the decoded data will be stored
    if (open_output_file(decInfo) == e_failure)
    {
        printf("Output file not opened\n");
        return e_failure;  // Return failure if the output file could not be opened
    }
    printf("Output file opened\n");

    // Step 6: Decode the size of the output file
    if (get_output_file_size(decInfo) == e_failure)
    {
        printf("Output File Size not decoded\n");
        return e_failure;  // Return failure if the file size could not be decoded
    }
    printf("Output File Size decoded\n");
  
    // Step 7: Decode the actual file data from the stego image
    if (decode_output_file_data(decInfo) == e_failure)
    {
        printf("Secret File Data not decoded\n");
        return e_failure;  // Return failure if the file data could not be decoded
    } 
    printf("Output file decoded\n");
    return e_success;  // Return success if all decoding steps are completed successfully
}

// Function to decode the magic string from the stego image
Status decode_magic_string(int magic_len, DecodeInfo *decInfo)
{
    char buff[magic_len + 1];  // Buffer to hold the decoded magic string, including null terminator
    decInfo->data_size = magic_len;  // Set the data size to the length of the magic string

    // Decode the magic string from the least significant bits (LSBs) of the image
    if (decode_byte_from_lsb(buff, decInfo) == e_failure)
    {
        printf("Magic String is not decoded\n");
        return e_failure;  // Return failure if the magic string could not be decoded
    }

    // Compare the decoded string with the expected MAGIC_STRING
    if (strcmp(buff, MAGIC_STRING) != 0)
    {
        printf("Magic String is not matched\n");
        return e_failure;  // Return failure if the decoded string does not match the expected magic string
    }
    return e_success;  // Return success if the magic string is decoded and matched
}

// Function to reverse the bits in a byte (used in decoding)
char reverse(char data) 
{
    char reverse = 0;  // Initialize the reversed byte to zero
    int i;
    for (i = 0; i < 8; i++)  // Loop through all 8 bits of the byte
    {
        if ((data & (1 << i)))  // Check if the i-th bit is set in the original byte
            reverse |= 1 << ((8 - 1) - i);  // Set the corresponding bit in the reversed byte
    }
    return reverse;  // Return the byte with reversed bits
}

// Function to decode a byte from the least significant bits (LSBs) of the image
Status decode_byte_from_lsb(char data[], DecodeInfo *decInfo)
{
    int i, j;
    char lsb[8];  // Buffer to hold the 8 bits read from the image
    
    for (j = 0; j < decInfo->data_size; j++)  // Loop to decode each byte of the data
    {
        fread(lsb, 1, 8, decInfo->fptr_stego_image);  // Read 8 bits from the image

        for (i = 0; i < 8; i++)
        {
            data[j] = (data[j] << 1) | (lsb[i] & 1);  // Extract LSB and shift to form the byte
        }
        data[j] = reverse(data[j]);  // Reverse the bit order of the decoded byte
    }
    data[decInfo->data_size] = '\0';  // Null terminate the decoded string
    return e_success;  // Return success after decoding the byte(s)
}

// Function to decode the size of the output file extension from the stego image
Status decode_extn_size_of_output(DecodeInfo *decInfo)
{
    // Decode the extension size (stored as an integer) from the image
    if (decode_size_from_lsb(&decInfo->extn_output_size, decInfo) == e_failure)
    {
        printf("Output extension size not decoded\n");
        return e_failure;  // Return failure if the extension size could not be decoded
    }
    return e_success;  // Return success if the extension size is decoded successfully
}

// Function to decode an integer value (size) from the least significant bits (LSBs) of the image
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo)
{
    char buff[32] = {0};  // Initialize buffer to zero to hold 32 bits (for an integer size)
    fread(buff, 1, 32, decInfo->fptr_stego_image);  // Read 32 bits from the image

    *size = 0;  // Initialize the size to zero
    for (int i = 31; i >= 0; i--)  // Loop through each bit of the buffer (starting from the most significant bit)
    {
        *size = (*size << 1) | (buff[i] & 1);  // Extract LSB and shift to form the size integer
    }
    return e_success;  // Return success after decoding the size
}

// Function to decode the output file extension from the stego image
Status decode_output_extn(DecodeInfo *decInfo)    
{
    char buff[decInfo->extn_output_size + 1];  // Buffer to hold the decoded extension, including null terminator
    decInfo->data_size = decInfo->extn_output_size;  // Set the data size to the length of the extension

    // Decode the extension from the least significant bits (LSBs) of the image
    if (decode_byte_from_lsb(buff, decInfo) == e_failure)
    {
        return e_failure;  // Return failure if the extension could not be decoded
    }
    strcpy(decInfo->extn_output_file, buff);  // Copy the decoded extension to the structure
    return e_success;  // Return success if the extension is decoded successfully
}

// Function to open the output file where the decoded data will be stored
Status open_output_file(DecodeInfo *decInfo)
{
    // Handle if the output file name already has an extension
    char *pos = strstr(decInfo->output_fname, ".");
    if (pos != NULL)
    {
        int index = pos - decInfo->output_fname;  // Calculate the position of the extension
        char new_output_name[index + decInfo->extn_output_size + 1];  // Allocate memory for the new name

        strncpy(new_output_name, decInfo->output_fname, index);  // Copy the file name without the old extension
        new_output_name[index] = '\0';
        strcat(new_output_name, decInfo->extn_output_file);  // Append the decoded extension

        decInfo->output_fname = strdup(new_output_name);  // Store the new output file name in the structure
    }
    else
    {
        strcat(decInfo->output_fname, decInfo->extn_output_file);  // Append the extension to the output file name
    }

    // Open the output file in write mode
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    // Handle errors if the file cannot be opened
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");  // Print error message
        fprintf(stderr, "ERROR: Unable to open output file %s\n", decInfo->output_fname);
        return e_failure;  // Return failure if the file cannot be opened
    }
    return e_success;  // Return success after opening the output file
}

// Function to decode the size of the output file from the stego image
Status get_output_file_size(DecodeInfo *decInfo)
{
    // Decode the file size (stored as an integer) from the image
    if (decode_size_from_lsb(&decInfo->size_output_file, decInfo) == e_failure)
    {
        printf("Output file size not decoded\n");
        return e_failure;  // Return failure if the file size could not be decoded
    }
    return e_success;  // Return success if the file size is decoded successfully
}

// Function to decode the actual data of the output file from the stego image
Status decode_output_file_data(DecodeInfo *decInfo)
{
    char buff[decInfo->size_output_file + 1];  // Buffer to hold the decoded file data, including null terminator
    decInfo->data_size = decInfo->size_output_file;  // Set the data size to the size of the output file

    // Decode the file data from the least significant bits (LSBs) of the image
    if (decode_byte_from_lsb(buff, decInfo) == e_failure)
    {
        return e_failure;  // Return failure if the file data could not be decoded
    }
    fwrite(buff, 1, decInfo->size_output_file, decInfo->fptr_output);  // Write the decoded data to the output file
    return e_success;  // Return success after decoding and writing the file data
}

