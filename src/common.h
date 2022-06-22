#ifndef HE_COMMON_HEADER
#define HE_COMMON_HEADER

#include "stdio.h"
#include "stdlib.h"

#define ERR_COL "\033[31m"
#define WAR_COL "\033[32m"
#define DEF_COL "\033[0m"

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

#endif