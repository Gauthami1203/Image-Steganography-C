/*Name:Gauthami N
  Roll number:24008_053
  Date:16/08/24
  */
#include <stdio.h>               // Includes the standard I/O library for input/output operations.
#include <string.h>              // Includes the string library for string manipulation functions.
#include "encode.h"              // Includes the user-defined header file for encoding operations.
#include "decode.h"              // Includes the user-defined header file for decoding operations.
#include "types.h"               // Includes the user-defined header file for data type definitions (e.g., enums, structs).

int main(int argc, char *argv[]) // Main function that serves as the entry point of the program. It takes command-line arguments.
{
    EncodeInfo encInfo;          // Declares a variable of type EncodeInfo to hold encoding information.
    DecodeInfo decInfo;          // Declares a variable of type DecodeInfo to hold decoding information.
    int ret = check_operation_type(argv); // Calls check_operation_type to determine whether to encode or decode, based on command-line arguments.

    if(ret == e_encode)          // Checks if the operation type returned is 'e_encode'.
    {
        printf("Selected encoding\n"); // Prints that encoding has been selected.
        if(argc >= 4)            // Ensures that at least 4 command-line arguments are passed for encoding.
        {
            if(read_and_validate_encode_args(argv, &encInfo) == e_success) // Validates the encoding arguments.
            {
                if(do_encoding(&encInfo) == e_success) // Performs the encoding operation.
                {
                    printf("Encoding successful\n"); // Indicates successful encoding.
                }
                else
                {
                    printf("Encoding unsuccessful\n"); // Indicates unsuccessful encoding.
                }
            }
            else
            {
                printf("read and validate failure\n"); // Indicates failure in reading or validating encoding arguments.
            }
        }
        else
        {
            printf("Please pass minimum of 4 arguments\n"); // Prompts user to provide at least 4 arguments for encoding.
        }
        
    }
    else if(ret == e_decode)     // Checks if the operation type returned is 'e_decode'.
    {
         printf("Selected decoding\n"); // Prints that decoding has been selected.
         if(argc >= 3)            // Ensures that at least 3 command-line arguments are passed for decoding.
        {
            if(read_and_validate_decode_args(argv, &decInfo) == e_success) // Validates the decoding arguments.
            {
                if(do_decoding(&decInfo) == e_success) // Performs the decoding operation.
                {
                    printf("Decoding successful\n"); // Indicates successful decoding.
                }
                else
                {
                    printf("Decoding unsuccessful\n"); // Indicates unsuccessful decoding.
                }
            }
            else
            {
                printf("read and validate failure\n"); // Indicates failure in reading or validating decoding arguments.
            }
        }
        else
        {
            printf("Please pass minimum of 3 arguments\n"); // Prompts user to provide at least 3 arguments for decoding.
        } 
    } 
    else
    {
        printf("Unsupported format\n"); // Indicates that an unsupported operation type was selected.
    }
}

// Function to determine the operation type (encoding or decoding) based on command-line arguments.
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0) // Checks if the first argument is "-e" for encoding.
    {
        return e_encode;          // Returns the enum value indicating encoding.
    }
    else if(strcmp(argv[1], "-d") == 0) // Checks if the first argument is "-d" for decoding.
    {
        return e_decode;          // Returns the enum value indicating decoding.
    }
    else
    {
        return e_unsupported;     // Returns the enum value indicating an unsupported operation.
    }
}

    

