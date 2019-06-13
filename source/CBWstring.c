/*
 * Name: CBWstring.c
 * Author: Blake Wingard
 * Desc: String function for general use.
 * Vers: 1.0.0 05/26/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
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
