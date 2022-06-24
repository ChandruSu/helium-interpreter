#include "common.h"

void file_error(const char* msg, const char* fname)
{
    fprintf(stderr, "Error! %s: %s\n", msg, fname);
    exit(0);
}

const char* read_file(const char* filepath)
{
    FILE* fptr = fopen(filepath, "r+");

    if (fptr == NULL) {
        file_error("Failed to open file", filepath);
    }

    // goes to end of file
    if (fseek(fptr, 0, SEEK_END) != 0) {
        fclose(fptr);
        file_error("Failed to read file", filepath);
    }

    // size of file in bytes
    long fsize = ftell(fptr);

    if (fsize == -1) {
        fclose(fptr);
        file_error("Failed to read file", filepath);
    }

    fseek(fptr, 0, SEEK_SET);

    char* buffer = (char*) malloc(sizeof(char) * fsize);
    size_t new_size = fread(buffer, sizeof(char), fsize, fptr);

    if (ferror(fptr) != 0) {
        fclose(fptr);
        file_error("Failed to read file", filepath);
    } else {
        buffer[new_size++] = '\0';
    }
    
    fclose(fptr);
    return buffer;
}

const char* get_line(const char* source, int start)
{
    char* buf = (char*)malloc(sizeof(char) * 1000);

    int i = 0;
    while (source[start + i] != '\n' && source[start + i] != '\0') {
        buf[i] = source[start + i];
        i++;
    }

    buf[i] = '\0';
    return buf;
}

const char* paddchar(char c, int n) {
    char* buf = (char*) malloc(sizeof(char) * n);
    memset(buf, c, n);
    buf[n+1] = '\0';
    return buf;
}