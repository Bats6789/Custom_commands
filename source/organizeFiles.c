/*
 * Name: organizeFiles.c
 * Author: Blake Wingard
 * Desc: organizes the files into a folder for each file type.
 * Vers: 2.0.1 06/09/2019 CBW - Bug fix.
 * Vers: 2.0.0 06/04/2019 CBW - Added list features
 * Vers: 1.0.0 05/26/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "..\library\CBWfile.h"
#include "..\library\CBWstring.h"

typedef struct folderName {
     char *extension;
     char *name;
     struct folderName* nextFolder;
     struct folderName* prevFolder;
} folderNameType;

int readFileToList( char *filename, folderNameType *headNode );
int printFolderList( folderNameType *headNode );
int printListToFile( folderNameType *headNode );
int addToList( folderNameType *headNode, char *extension, char *name );
int sortList( folderNameType **headNode );
int removeFromList( folderNameType *headNode, char *extension );
int printExtName( folderNameType *headNode, char *extension );
char *findFolderName( char *filetype, folderNameType *headNode );
void help( void );

int main( int argc, char** argv ){
     folderNameType *headNode;
     FILE *dir;         // the current directory
     char *dirName;     // the subdirectory for a file
     char *extension;
     char *filename;
     char *buf;         // a buffer for the system function
     char *listDir;     // the directory the list is in
     char chunk;
     // index
     int fileIndex;
     int exIndex;
     // scales memory used
     int fileScale;
     int exScale;
     int bufScale;

     // prepare list of folder names
     headNode = malloc( sizeof( folderNameType ) );
     headNode->nextFolder = headNode;
     listDir = malloc( sizeof( listDir ) * (strlen( getenv( "commands" ) ) + 20 ));
     strcpy( listDir, getenv( "commands" ));
     strcat( listDir, "\\folderList.list" );
     readFileToList( listDir, headNode );
     free( listDir );

     if( argc == 1 ){ // sort the files
          dir = popen( "dir /a-d /b", "r" );
          if( dir == NULL ){
               printf("ERROR: Couldn't view directory.\n");
               return( EXIT_FAILURE );
          }
          filename = malloc( sizeof( filename ) * 100 );
          extension = malloc( sizeof( extension ) * 10 );
          buf = malloc( sizeof( buf ) * 500 );
          exIndex = 0;
          fileIndex = 0;
          exScale = 1;
          fileScale = 1;
          bufScale = 1;
          while( fscanf( dir, "%c", &chunk ) != EOF ){
               filename[ fileIndex ] = chunk;
               if( chunk == '.' ){
                    ++fileIndex;
                    while(( fscanf( dir, "%c", &chunk ) != EOF ) && ( chunk != '\n' )){
                         filename[ fileIndex ] = chunk;
                         extension[ exIndex ] = chunk;
                         ++fileIndex;
                         ++exIndex;
                         if( exIndex == exScale * 10 ){
                              ++exScale;
                              extension = realloc( extension, sizeof( extension ) * 10 * exScale );
                         }
                         if( fileIndex == fileScale * 100 ){
                              ++fileScale;
                              filename = realloc( filename, sizeof( filename ) * fileScale * 100);
                         }
                    }
                    filename[ fileIndex ] = '\0';
                    extension[ exIndex ] = '\0';
                    dirName = findFolderName( extension, headNode );
                    mkdir( dirName );
                    while( strlen( filename ) + strlen( dirName ) + 13 >= 500 * bufScale ){
                         ++bufScale;
                         buf = realloc( buf, sizeof( buf ) * bufScale * 500 );
                    }
                    strcpy( buf, "move ");
                    strcat( buf, filename );
                    strcat( buf, " " );
                    strcat( buf, dirName );
                    strcat( buf, " > NUL");
                    system( buf );
                    fileIndex = 0;
                    exIndex = 0;
               } else if( chunk == '\n' ){
                    filename[ fileIndex ] = '\0';
                    dirName = findFolderName( "[blank]", headNode );
                    mkdir( dirName );
                    while( strlen( filename ) + strlen( dirName ) + 13 >= 500 * bufScale ){
                         ++bufScale;
                         buf = realloc( buf, sizeof( buf ) * bufScale * 500 );
                    }
                    strcpy( buf, "move ");
                    strcat( buf, filename );
                    strcat( buf, " " );
                    strcat( buf, dirName );
                    strcat( buf, " > NUL");
                    system( buf );
                    fileIndex = 0;
                    exIndex = 0;
               } else {
                    ++fileIndex;
               }
          }
          pclose( dir );
          free( filename );
          free( extension );
          free( buf );
     } else { // switches used
          if( strcmpi( "/h", argv[ 1 ] ) == 0 ){ // help
               help();
          } else if(( strcmpi( "/a", argv[ 1 ] ) == 0 ) && (argc == 4 )){ // add extension
               addToList( headNode, argv[ 2 ], argv[ 3 ] );
               sortList( &headNode );
               printListToFile( headNode );
          } else if( strcmpi( "/v", argv[ 1 ] ) == 0 ){ // view list
               printFolderList( headNode );
          } else if(( strcmpi( "/c", argv[ 1 ] ) == 0 ) && ( argc == 3 )){ // check extension
               printExtName( headNode, argv[ 2 ] );
          } else if(( strcmpi( "/r", argv[ 1 ] ) == 0 ) && ( argc == 3 )){ // remove extension
               removeFromList( headNode, argv[ 2 ] );
               printListToFile( headNode );
          } else {
               printf("ERROR: Invalid switch.\n");
               help();
          }
     }
     return( EXIT_SUCCESS );
}

/*
 * Name: readFileToList( char *filename, folderNameType *head_node )
 * Desc: reads a file to a linked list
 * Args:
 *      char *filename - the name of the file
 *      folderNameType *head_node - the start of the linked list
 */
int readFileToList( char *filename, folderNameType *headNode ){
     FILE* file;
     int isHeadNode;
     folderNameType *currentNode;
     char line[ 1000 ];
     char chunks[ CBW_ROW_MAX ][ CBW_COL_MAX ];
     int count;

     file = fopen( filename, "r" );
     if( file == NULL ){
          printf("ERROR: Can't open file.\n");
          return( EXIT_FAILURE );
     }
     if( headNode == NULL ){
          printf("ERROR: Invalid list.\n");
          return( EXIT_FAILURE );
     }
     isHeadNode = 1;
     count = 0;
     while( fileReadLine( file, line ) > 0 ){
          if( stringSplit( line, ',', chunks ) == 2 ){
               if( isHeadNode == 0 ){
                    currentNode->nextFolder = malloc( sizeof( folderNameType ) );
                    currentNode->nextFolder->prevFolder = currentNode;
                    currentNode = currentNode->nextFolder;
                    currentNode->extension = malloc( sizeof( chunks[ 0 ] ) * strlen( chunks[ 0 ] ) + 1);
                    currentNode->name = malloc( sizeof( chunks[ 1 ] ) * strlen( chunks[ 1 ] ) + 1);
                    strcpy( currentNode->extension, chunks[ 0 ] );
                    strcpy( currentNode->name, chunks[ 1 ] );
                    currentNode->nextFolder = NULL;
                    ++count;
               } else {
                    currentNode = headNode;
                    currentNode->extension = malloc( sizeof( chunks[ 0 ] ) * strlen( chunks[ 0 ] ) + 1);
                    currentNode->name = malloc( sizeof( chunks[ 1 ] ) * strlen( chunks[ 1 ] ) + 1);
                    strcpy( currentNode->extension, chunks[ 0 ] );
                    strcpy( currentNode->name, chunks[ 1 ] );
                    currentNode->prevFolder = NULL;
                    currentNode->nextFolder = NULL;
                    ++count;
                    isHeadNode = 0;
               }
          }
     }
     fclose( file );
     return( count );
}

/*
 * Name: printFolderList( folderNameType *headNode )
 * Desc: Prints the list of folders to the command line
 * Args:
 *      folderNameType *headNode - the start of the list
 */
int printFolderList( folderNameType *headNode ){
     folderNameType *currentNode;

     if( headNode == NULL ){
          printf("ERROR: Invalid list.\n");
          return( -1 );
     }
     if( headNode->nextFolder == headNode ){
          printf("ERROR: Empty list.\n");
          return( -1 );
     }
     currentNode = headNode;
     while( currentNode != NULL ){
          printf("extension       %s\n", currentNode->extension);
          printf("name            %s\n", currentNode->name);
          currentNode = currentNode->nextFolder;
     }
     return( EXIT_SUCCESS );
}

/*
 * Name: printListToFile( folderNameType *headNode )
 * Desc: Prints the contents of the linked list to the list file
 * Args:
 *      folderNameType *headNode - the start of the list
 */
int printListToFile( folderNameType *headNode ){
     folderNameType *currentNode;
     FILE *file;

     if( headNode == NULL ){
          printf("ERROR: Invalid list.\n");
          return( -1 );
     }
     if( headNode->nextFolder == headNode ){
          printf("ERROR: Empty list.\n");
          return( -1 );
     }
     chdir( getenv( "commands" ) );
     file = fopen( "folderList.list", "w" );
     currentNode = headNode;
     while( currentNode != NULL ){
          fprintf( file, "%s,%s\n", currentNode->extension, currentNode->name);
          currentNode = currentNode->nextFolder;
     }
     fclose( file );
     return( EXIT_SUCCESS );
}

/*
 * Name: addToList( folderNameType *headNode, char *extension, char *name )
 * Desc: Prints the contents of the linked list to the list file
 * Args:
 *      folderNameType *headNode - the start of the list
 */
int addToList( folderNameType *headNode, char *extension, char *name ){
     folderNameType *currentNode;

     currentNode = headNode;
     if( headNode == NULL ){
          printf("ERROR: Invalid head node.\n");
          return( EXIT_FAILURE );
     }
     if( headNode == headNode->nextFolder ){
          currentNode->nextFolder = NULL;
          currentNode->prevFolder = NULL;
          currentNode->extension = malloc( sizeof( currentNode->extension ) * ( strlen( extension ) + 1 ));
          currentNode->name = malloc( sizeof( currentNode->name ) * ( strlen( name ) + 1 ));
          strcpy( currentNode->extension, extension );
          strcpy( currentNode->name, name );
          return( EXIT_SUCCESS );
     }
     while( currentNode->nextFolder != NULL ){
          if( strcmpi( extension, currentNode->extension ) == 0 ){ // if it already exist, update it.
               if( strlen( name ) > strlen( currentNode->name ) ){
                    currentNode->name = realloc( currentNode->name, sizeof( currentNode->name ) * ( strlen( name ) + 1 ));
               }
               strcpy( currentNode->name, name );
               return( EXIT_SUCCESS );
          }
          currentNode = currentNode->nextFolder;
     }
     // create new node for the addition
     currentNode->nextFolder = malloc( sizeof( folderNameType ) );
     currentNode->nextFolder->prevFolder = currentNode;
     currentNode = currentNode->nextFolder;
     currentNode->nextFolder = NULL;
     currentNode->extension = malloc( sizeof( currentNode->extension ) * ( strlen( extension ) + 1 ));
     currentNode->name = malloc( sizeof( currentNode->name ) * ( strlen( name ) + 1 ));
     strcpy( currentNode->extension, extension );
     strcpy( currentNode->name, name );
     return( EXIT_SUCCESS );
}

/*
 * Name: sortList( folderNameType **headNode )
 * Desc: Sorts the list by extension in alphabetical order
 * Args:
 *      folderNameType *headNode - the start of the list
 */
int sortList( folderNameType **headNode ){
     folderNameType *currentNode; // current node being sorted
     folderNameType *endNode;     // last node sorted
     folderNameType *compareNode; // node being compared to currentNode
     folderNameType *nextNode;    // the next node to be sorted
     folderNameType *head;
     int sort;

     head = *headNode;
     if(( head == NULL ) || ( head == head->nextFolder )){
          printf("ERROR: Invalid list to sort.\n");
          return( EXIT_FAILURE );
     }
     currentNode = head->nextFolder;
     nextNode = currentNode->nextFolder;
     endNode = head;
     compareNode = head;
     sort = 0;
     while( currentNode != NULL ){
          while( sort == 0 ){
               if( strcmp( currentNode->extension, compareNode->extension ) >= 0 ){
                    if( endNode == compareNode ){
                         endNode->nextFolder = currentNode;
                         currentNode->prevFolder = endNode;
                         endNode = currentNode;
                    } else {
                         currentNode->nextFolder = compareNode->nextFolder;
                         currentNode->prevFolder = compareNode;
                         compareNode->nextFolder->prevFolder = currentNode;
                         compareNode->nextFolder = currentNode;
                    }
                    sort = 1;
               } else if( compareNode->prevFolder == NULL ){
                    currentNode->prevFolder = NULL;
                    currentNode->nextFolder = compareNode;
                    compareNode->prevFolder = currentNode;
                    *headNode = currentNode;
                    sort = 1;
               } else {
                    compareNode = compareNode->prevFolder;
               }
          }
          compareNode = endNode;
          currentNode = nextNode;
          if( currentNode != NULL ){
               nextNode = currentNode->nextFolder;
          }
          sort = 0;
     }
     endNode->nextFolder = NULL;
     return( EXIT_SUCCESS );
}

/*
 * Name: removeFromList( folderNameType *headNode, char *extension )
 * Desc: removes a node from the list
 * Args:
 *      folderNameType *headNode - the start of the list
 *      char *extension - the extension being removed
 */
int removeFromList( folderNameType *headNode, char *extension ){
     folderNameType *currentNode;

     currentNode = headNode;
     while( currentNode != NULL ){
          if( strcmpi( currentNode->extension, extension ) == 0 ){
               currentNode->prevFolder->nextFolder = currentNode->nextFolder;
               currentNode->nextFolder->prevFolder = currentNode->prevFolder;
               free( currentNode->extension );
               free( currentNode->name );
               free( currentNode );
               return( EXIT_SUCCESS );
          } else {
               currentNode = currentNode->nextFolder;
          }
     }
     return( EXIT_FAILURE );
}

/*
 * Name: printExtName( folderNameType *headNode, char *extension )
 * Desc: Prints the folder-name of an extension
 * Args:
 *      folderNameType *headNode - the start of the list
 *      char *extension - the extension being printed
 */
int printExtName( folderNameType *headNode, char *extension ){
     folderNameType *currentNode;
     int found;

     if( headNode == NULL ){
          printf("ERROR: Invalid list.\n");
          return( -1 );
     }
     if( headNode->nextFolder == headNode ){
          printf("ERROR: Empty list.\n");
          return( -1 );
     }
     currentNode = headNode;
     found = 0;
     while(( currentNode != NULL ) && ( found == 0 )){
          if( strcmpi( extension, currentNode->extension ) == 0 ){
               printf("extension       %s\n", currentNode->extension);
               printf("name            %s\n", currentNode->name);
               found = 1;
          } else {
               currentNode = currentNode->nextFolder;
          }
     }
     if( found == 0 ){
          printf("Extension not found.\n");
     }
     return( EXIT_SUCCESS );
}

/*
 * Name: findFolderName( char *filetype, folderNameType *headNode )
 * Desc: Obtains the name of the folder to store the file in.
 * Args:
 *      char *filetype - the file extension
 *      folderNameType *headNode - the start of the list
 */
char *findFolderName( char *filetype, folderNameType *headNode ){
     folderNameType *currentNode;

     currentNode = headNode;
     while( currentNode != NULL ){
          if( strcmp( filetype, currentNode->extension ) == 0 ){
               return( currentNode->name );
          } else {
               currentNode = currentNode->nextFolder;
          }
     }
     char *newname;
     newname = malloc( sizeof( newname ) * ( strlen( filetype ) + 8 ));
     strcpy( newname, filetype );
     strcat( newname, "_folder" );
     return( newname );
}

/*
 * Name: help( void )
 * Desc: Provides help to the user
 * Args: (none)
 */
void help( void ){
     chdir( getenv( "commands" ) );
     readFileToSTD( "organizeFiles.txt" );
}
