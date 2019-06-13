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

#if defined(__WIN32__)
    #define WINDOWS 1
#else
    #define WINDOWS 0
#endif



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
void getFileType( char *filename, char *filetype, int *size );
void help( void );

int main( int argc, char** argv ){
     folderNameType *headNode;
     DIR *directory;
     DIR *checkForDir;
     struct dirent *directoryList;
     char *dirName;     // the subdirectory for a file
     char *extension;
     char *buf;         // a buffer for the system function
     char *listDir;     // the directory the list is in
     char move[ 6 ];
     char pipe[ 7 ];
     int extSize;
     unsigned int bufScale;

     if( WINDOWS ){
          strcpy( move, "move " );
          strcpy( pipe, " > NUL" );
     } else {
          strcpy( move, "mv " );
          pipe[ 1 ] = '\0';
     }


     // prepare list of folder names
     headNode = malloc( sizeof( folderNameType ) );
     headNode->nextFolder = headNode;
     listDir = malloc( sizeof( listDir ) * (strlen( getenv( "commands" ) ) + 20 ));
     strcpy( listDir, getenv( "commands" ));
     strcat( listDir, "\\folderList.list" );
     readFileToList( listDir, headNode );
     free( listDir );

     if( argc == 1 ){ // sort the files
          directory = opendir( "." );
          if( directory == NULL ){
               printf("ERROR: Failed to open directory.\n");
               return( EXIT_FAILURE );
          }
          extension = malloc( sizeof( extension ) * 10 );
          buf = malloc( sizeof( buf ) * 500 );
          extSize = 10;
          bufScale = 1;
          while(( directoryList = readdir( directory )) != NULL ){
               checkForDir = opendir( directoryList->d_name );
               if( checkForDir != NULL ){
                    closedir( checkForDir );
               } else {
                    getFileType( directoryList->d_name, extension, &extSize);
                    dirName = findFolderName( extension, headNode );
                    mkdir( dirName );
                    while(( strlen( directoryList->d_name ) + strlen( dirName ) + 13) >= (500 * bufScale )){
                         ++bufScale;
                         buf = realloc( buf, sizeof( buf ) * bufScale * 500 );
                    }
                    strcpy( buf, move );
                    strcat( buf, directoryList->d_name );
                    strcat( buf, " " );
                    strcat( buf, dirName );
                    strcat( buf, pipe );
                    system( buf );
               }
          }
          closedir( directory );
          free( dirName );
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
 * Name: getFileType( char *filename, int *size )
 * Desc: Obtains the extension of a file.
 * Args:
 *      char *filename - the name of the file
 *      char *filetype - the file extension
 *      int *size - the size of filetype
 */
void getFileType( char *filename, char *filetype, int *size ){
     int indexName;
     int indexType;
     int currentSize;

     indexName = 0;
     indexType = 0;
     currentSize = 0;

     while(( filename[ indexName ] != '.' ) && ( filename[ indexName ] != '\0' )){ // get to extension
          ++indexName;
     }

     if( filename[ indexName ] == '\0' ){
          strcpy( filetype, "[blank]" );
     } else{
          ++indexName;
          while( filename[ indexName ] != '\0' ){
               filetype[ indexType ] = filename[ indexName ];
               ++indexName;
               ++indexType;
               ++currentSize;
               if( currentSize >= *size ){
                    filetype = realloc( filetype, sizeof( filetype ) * ( *size + 10 ));
                    *size = *size + 10;
               }
          }
          filetype[ indexType ] = '\0';
     }
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
