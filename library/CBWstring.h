/*
 * Name: CBWstring.h
 * Author: Blake Wingard
 * Desc: String function for general use.
 * Vers: 1.0.0 05/26/2019 CBW - Original code.
 */

#ifndef CBWSTRING_H
#define CBWSTRING_H

#define CBW_ROW_MAX 50
#define CBW_COL_MAX 1000

/*
 * Name: stringSplit( char *chunk, char del, char **chunks );
 * Desc: Splits a string into chunks by a delimiter
 * Args:
 *      char *chunk - the string to split
 *      char del - the delimiter
 *      char **chunks - the chunks to store the split
 */
 int stringSplit( char *chunk, char del, char chunks[ CBW_ROW_MAX][ CBW_COL_MAX ] );

#endif
