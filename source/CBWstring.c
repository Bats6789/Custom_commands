/*
 * Name: CBWstring.c
 * Author: Blake Wingard
 * Desc: String function for general use.
 * Vers: 1.0.0 05/26/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../library/CBWstring.h"

int stringSplit( char *chunk, char del, char chunks[ CBW_ROW_MAX ][ CBW_COL_MAX ] ){
     int index;
     int rowIndex;
     int colIndex;
     int delCount;
     index = 0;
     rowIndex = 0;
     colIndex = 0;
     delCount = 0;
     while( chunk[ index ] != '\0' ){
          if( chunk[ index ] == del ){
               chunks[ colIndex ][ rowIndex ] = '\0';
               ++index;
               rowIndex = 0;
               ++colIndex;
               ++delCount;
          } else {
               chunks[ colIndex ][ rowIndex ] = chunk[ index ];
               ++index;
               ++rowIndex;
          }
     }
     chunks[ colIndex ][ rowIndex ] = '\0';
     return( delCount + 1 );
}

int stringCmpI( char *firstString, char *secondString ){
     char firstChunk;
     char secondChunk;
     int firstIndex;
     int secondIndex;
     int found;

     firstIndex = 0;
     secondIndex = 0;
     found = 0;

     firstChunk = firstString[ firstIndex ];
     secondChunk = secondString[ secondIndex ];
     ++firstIndex;
     ++secondIndex;
     while(( firstChunk != '\0' ) && ( secondChunk != '\0' ) && ( found == 0 )){
          if(( firstChunk >= 65 ) && ( firstChunk <= 90 )){
               firstChunk += 32;
          }
          if(( secondChunk >= 65 ) && ( secondChunk <= 90 )){
               secondChunk += 32;
          }
          if( firstChunk < secondChunk ){
               found = -1;

          } else if( firstChunk > secondChunk ){
               found = 1;
          } else {
               found = 0;
               firstChunk = firstString[ firstIndex ];
               secondChunk = secondString[ secondIndex ];
               ++firstIndex;
               ++secondIndex;
          }
     }
     if( found == 0 ){
          if(( firstChunk == '\0' ) && ( secondChunk == '\0' )){
               found = 0;
          } else if( firstChunk == '\0' ){
               found = -1;
          } else {
               found = 1;
          }
     }
     return( found );
}
