/*
 * Name: fileList.c
 * Author: Blake Wingard
 * Desc: Read a list of all files in a directory to another file
 * Vers: 1.0.0 05/24/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

int main( int argc, char** argv ){
     DIR *directory;
     struct dirent *directoryList;
     FILE *outFile;
     char *filename;
     int index;
          filename = malloc( sizeof( filename ) * 100 );
          strcpy( filename, "fileList.txt");
     if( argc >= 2 ){
          index = 0;
          // get filename
          while( argv[ 1 ][ index ] != '\0' ){
               filename[ index ] = argv[ 1 ][ index ];
               ++index;
               if(( index % 100 ) == 0 ){
                    filename = realloc( filename, sizeof( filename ) * ( index + 100 ));
               }
          }
          filename[ index ] = '\0';
     }
     outFile = fopen( filename, "w" );
     if( argc >= 3 ){
          if( chdir( argv[ 2 ] ) == -1 ){
               printf("ERROR: Can't access directory.\n");
               return( EXIT_FAILURE );
          }
     }
     directory = opendir( "." );
     if( directory == NULL ){
          printf("ERROR: Can't view current directory.\n");
          return( EXIT_FAILURE );
     }
     // read output to file
     while(( directoryList = readdir( directory )) != NULL ){
          if( opendir( directoryList->d_name ) == NULL ){
               fprintf( outFile, "%s\n", directoryList->d_name );
          }
     }

     closedir( directory );
     fclose( outFile );

     return( EXIT_SUCCESS );
}
