/*
 * Name: helpC.c
 * Author: Blake Wingard
 * Desc: Provides help for custom made functions
 * Vers: 1.0.1 06/16/2019 CBW - Fixed SIGTRAP
 * Vers: 1.0.0 05/22/2019 CBW - Original code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../library/CBWfile.h"
#include "../library/CBWstring.h"

void read_help( char *filename );
int read_list( char *command );

int main( int argc, char **argv ){
     char *filename;
     char *commandName;
     int scaling;            // scales the memory allocation
     int index;

     if( chdir( getenv( "commands" ) ) == -1 ){
	     printf("ERROR: Couldn't access enviromental variable.\n");
	     return( EXIT_FAILURE );
     }
     // if no input is provided. Print basic help. If a command is specified, print its general help
     if( argc < 2 ){
          readFileToSTD( "help.txt" );
     } else {
          commandName = malloc( sizeof( commandName ) * 10 );
          index = 0;
          scaling = 1;
          // obtain the command name without extension (if it was provided)
          while(( argv[ 1 ][ index ] != '.' ) && ( argv[ 1 ][ index ] != '\0' )){
               commandName[ index ] = argv[ 1 ][ index ];
               ++index;
               if(( index % 10 ) == 0 ){
                    ++scaling;
                    commandName = realloc( commandName, sizeof( commandName ) *  scaling * 10 );
               }
          }
          commandName[ index ] = '\0';
          if( read_list( commandName ) == 1 ){
               filename = malloc(( sizeof( filename ) * scaling * 10 ) + 4 );
               strcpy( filename, commandName );
               strcat( filename, ".txt" );
               readFileToSTD( filename );
               free( commandName );
               free( filename );
          } else {
               printf( "ERROR: No command named %s found.\n", argv[ 1 ] );
               return( EXIT_FAILURE );
          }
     }

     return( EXIT_SUCCESS );
}


/*
 * Name: int read_list( char *command )
 * Desc: Reads a list to verify a valid command exist
 * Args:
 *      char *command - the name of the command being verified
 */
int read_list( char *command ){
     FILE *checkfile;
     char chunk;
     char *CMPcommand;
     int scaling;           // scales the memory location
     int index;

     checkfile = fopen( "command.list", "r" );
     if( checkfile == NULL ){
          return( EXIT_FAILURE );
     }
     index = 0;
     scaling = 1;
     CMPcommand = malloc( sizeof( CMPcommand ) * 10 );
     while( fscanf( checkfile, "%c", &chunk ) != EOF ){
          // comparing line with command
          if( chunk == '\n' ){
               CMPcommand[ index ] = '\0';
               if( stringCmpI( CMPcommand, command ) == 0 ){
                    strcpy( command, CMPcommand );
                    fclose( checkfile );
                    free( CMPcommand );
                    return( 1 );
               } else {
                    index = 0;
               }
          } else {
               CMPcommand[ index ] = chunk;
               ++index;
          }
          // memory management
          if(( index - ( scaling * 10 )) == 0 ){
               ++scaling;
               CMPcommand = realloc( CMPcommand, sizeof( CMPcommand ) * scaling * 10 );
          }
     }
     fclose( checkfile );
     free( CMPcommand );

     return( 0 );
}
