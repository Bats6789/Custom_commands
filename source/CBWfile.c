/*
 * Name: CBWfile.c
 * Author: Blake Wingard
 * Desc: To provide general purpose file functions
 * Vers: 1.2.0 05/26/2019 CBW - Added fileReadLine
 * Vers: 1.1.0 05/23/2019 CBW - Added appendFileToFile.
 * Vers: 1.0.0 05/22/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../library/CBWfile.h"

int readFileToSTD( char *filename ){
     FILE *file;
     char chunk;
     int count;

     file = fopen( filename, "r" );
     if( file == NULL ){
          return( EXIT_FAILURE );
     }

     while( fscanf( file, "%c", &chunk ) != EOF ){
          printf( "%c", chunk );
          ++count;
     }

     fclose( file );

     return( count );
}

int readFileToFile( char *inputFilename, char *outputFilename ){
     FILE *inputFile;
     FILE *outputFile;
     char chunk;
     int count;

     inputFile = fopen( inputFilename, "r" );
     if( inputFile == NULL ){
          return( EXIT_FAILURE );
     }
     outputFile = fopen( outputFilename, "w" );

     while( fscanf( inputFile, "%c", &chunk ) != EOF ){
          fprintf( outputFile, "%c", chunk );
          ++count;
     }

     fclose( inputFile );
     fclose( outputFile );

     return( count );
}

int appendFileToFile( char *inputFilename, char *outputFilename ){
     FILE *inputFile;
     FILE *outputFile;
     char chunk;
     int count;

     inputFile = fopen( inputFilename, "r" );
     if( inputFile == NULL ){
          return( EXIT_FAILURE );
     }
     outputFile = fopen( outputFilename, "a" );

     while( fscanf( inputFile, "%c", &chunk ) != EOF ){
          fprintf( outputFile, "%c", chunk );
          ++count;
     }

     fclose( inputFile );
     fclose( outputFile );

     return( count );
}

int fileReadLine( FILE *file, char *line ){
     char chunk;
     int count;
     if( file == NULL ){
          printf("ERROR: NULL file pointer.\n");
          return( -1 );
     }
     count = 0;
     while(( fscanf( file, "%c", &chunk ) != EOF ) && ( chunk != '\n' )){
          line[ count ] = chunk;
          ++count;
     }
     line[ count ] = '\0';
     return( count );
}
