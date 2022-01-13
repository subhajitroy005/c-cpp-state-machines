#include "drv_file_read.h"

FILE *fptr; // File pointer

void file_drv_init(char * file_name)
{
    fptr = fopen(file_name,"r+");   // OPEN the file and get pointer
}


char char_read_from_file()
{
     return getc(fptr);
}


void file_drv_deinit()
{
    fclose(fptr);   // Close the file pointer
}
