/*
 * Name: merge.c
 * Author: Blake Wingard
 * Desc: Merge two files into one.
 * Vers: 1.0.0 05/23/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include "..\library\CBWfile.h"

int main( int argc, char** argv ){
     if( argc < 3 ){
          printf("ERROR: Not enough inputs.\n");
          return( EXIT_FAILURE );
     } else if( argc == 3 ){
          if( readFileToFile( argv[ 1 ], "newfile.txt" ) <= 0 ){
               printf("ERROR: opening %s.\n", argv[ 1 ]);
               return( EXIT_FAILURE );
               }
          if( appendFileToFile( argv[ 2 ], "newfile.txt" ) <= 0 ){
               printf("ERROR: opening %s.\n", argv[ 2 ]);
          }
     } else {
          if( readFileToFile( argv[ 1 ], argv[ 3 ] ) <= 0 ){
               printf("ERROR: opening %s.\n", argv[ 1 ]);
               return( EXIT_FAILURE );
          }
          if( appendFileToFile( argv[ 2 ], argv[ 3 ] ) <= 0 ){
               printf("ERROR: opening %s.\n", argv[ 2 ]);
          }
     }

     return( EXIT_SUCCESS );
}
