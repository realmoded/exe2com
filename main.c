#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXE_HEADER_SIZE 0x40   /* 64 bytes header for 16-bit DOS EXE files */
#define COM_FILE_SIZE  0x1000  /* 4 KB (0x1000 bytes) is the typical size for .COM files */
#define EXE_MAGIC "MZ"         /* Magic signature for DOS EXE files */

/* Function prototypes */
void print_usage(void);
int is_valid_exe(FILE *exe);
void convert_exe_to_com(const char *exe_file, const char *com_file);

/* Function to print the usage */
void print_usage()
{
    printf("EXE to COM Converter\n");
    printf("Usage:\n");
    printf("  EXE2COM.exe <exe_file> <com_file>\n");
    printf("  Converts a 16-bit DOS .EXE file to a .COM file by stripping the EXE header and padding with zeros.\n");
    printf("\n");
    printf("Options:\n");
    printf("  /?      Display this help message.\n");
    printf("\n");
}

/* Function to check if the file is a valid EXE file (starts with 'MZ') */
int is_valid_exe(FILE *exe)
{
    unsigned char magic[2];

    fseek(exe, 0, SEEK_SET);
    fread(magic, 1, 2, exe);
    return (magic[0] == 'M' && magic[1] == 'Z');
}

/* Function to convert EXE to COM */
void convert_exe_to_com(const char *exe_file, const char *com_file)
{
    FILE *exe, *com;
    unsigned char buffer[1024];  /* Buffer to hold data while reading */
    int bytes_read;
    long current_size;

    /* Open the EXE file for reading */
    exe = fopen(exe_file, "rb");
    if (exe == NULL) {
        perror("Error opening .exe file");
        return;
    }

    /* Validate the EXE file format */
    if (!is_valid_exe(exe)) {
        printf("Error: The file '%s' is not a valid DOS EXE file.\n", exe_file);
        fclose(exe);
        return;
    }

    /* Create the COM file for writing */
    com = fopen(com_file, "wb");
    if (com == NULL) {
        perror("Error creating .com file");
        fclose(exe);
        return;
    }

    /* Skip the 64-byte EXE header for 16-bit DOS EXE files */
    fseek(exe, EXE_HEADER_SIZE, SEEK_SET);

    /* Read the rest of the EXE file (program code) */
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), exe)) > 0) {
        fwrite(buffer, 1, bytes_read, com);
    }

    /* Now pad the .com file with zero bytes until the desired size (e.g., 4KB) */
    current_size = ftell(com);
    while (current_size < COM_FILE_SIZE) {
        fputc(0, com);  /* Padding with zero bytes */
        current_size++;
    }

    printf("Conversion complete: %s -> %s\n", exe_file, com_file);

    fclose(exe);
    fclose(com);
}

int main(int argc, char *argv[])
{
    const char *exe_filename;
    const char *com_filename;
    FILE *exe;  /* Declare exe as a FILE pointer */

    /* Check if the user asked for help */
    if (argc == 2 && strcmp(argv[1], "/?") == 0) {
        print_usage();
        return 0;
    }

    /* Check for valid arguments */
    if (argc < 3) {
        /* If parameters are missing, show the usage */
        print_usage();
        return 1;
    } else {
        /* If arguments are provided, use them */
        exe_filename = argv[1];
        com_filename = argv[2];
    }

    /* Validate the input .exe file before proceeding */
    exe = fopen(exe_filename, "rb");  /* Open the EXE file */
    if (exe == NULL) {
        perror("Error opening the .exe file");
        return 1;
    }
    fclose(exe);  /* Close the EXE file after validation */

    /* Proceed with conversion */
    convert_exe_to_com(exe_filename, com_filename);

    return 0;
}
