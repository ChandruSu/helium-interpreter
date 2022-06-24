#ifndef HE_COMMON_HEADER
#define HE_COMMON_HEADER

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define ERR_COL "\033[31m"
#define WAR_COL "\033[33m"
#define MSG_COL "\033[32m"
#define DEF_COL "\033[0m"

#define MESSAGE "[\033[32mmessage\033[0m]"
#define ERROR "[\033[31merror\033[0m]"
#define WARNING "[\033[33mwarning\033[0m]"

typedef unsigned char boolean;
#define true 1
#define false 0

/**
 * @brief Prints file error and terminates program.
 * 
 * @param msg Error message
 * @param fname File path
 */
void file_error(const char* msg, const char* fname);

/**
 * @brief Reads file from path into character buffer and returns
 *      pointer.
 * 
 * @param filepath Path to file 
 * @return String buffer
 */
const char* read_file(const char* filepath);

const char* get_line(const char* source, int start);

const char* paddchar(char c, int n);

#endif