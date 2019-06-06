/*
 * Name: CBWfile.h
 * Author: Blake Wingard
 * Desc: To provide general purpose file functions
 * Vers: 1.2.0 05/26/2019 CBW - Added fileReadLine
 * Vers: 1.1.0 05/23/2019 CBW - Added appendFileToFile.
 * Vers: 1.0.0 05/22/2019 CBW - Original code.
 */
#ifndef CBWFILE_H
#define CBWFILE_H

/*
 * Name: int readFileToStd( char *filename );
 * Desc: Prints the contents of a file to standard output.
 * Args:
 *      char *filename - the name of the file
 * Return value: The number of characters read.
 */
int readFileToSTD( char *filename );

/*
 * Name: int readFileToFile( char *inputFilename, char* outputFilename )
 * Desc: Prints the contents of a file to another file.
 * Args:
 *      char *inputFilename - the name of the input file
 *      char *outputFilename - the name of the output file
 * Return value: The number of characters read.
 */
int readFileToFile( char *inputFilename, char* outputFilename );

/*
 * Name: int appendFileToFile( char *inputFilename, char* outputFilename )
 * Desc: Prints the contents of a file to the end of another file.
 * Args:
 *      char *inputFilename - the name of the input file
 *      char *outputFilename - the name of the output file
 * Return value: The number of characters read.
 */
int appendFileToFile( char *inputFilename, char *outputFilename );

/*
 * Name: fileReadLine( FILE *file, char *line )
 * Desc: Reads a line in a file to line.
 * Args:
 *      FILE *file - the file to read from
 *      char *line - the line to read to
 */
int fileReadLine( FILE *file, char *line );

#endif
