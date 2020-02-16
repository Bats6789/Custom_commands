/*
 * Name: fileSearch.c
 * Auth: Blake Wingard
 * Desc: Searches for a file.
 * Vers: 1.0.0 02/15/2020 CBW - Original code.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "../library/CBWstring.h"

#ifdef WIN32
#define SLASH '\\'
#else
#define SLASH '/'
#endif

typedef struct fileList {
	char *path;
	struct fileList *nextFile;
	struct fileList *prevFile;
} fileList;


fileList *fileSearch( const char *currentDirectory, const char *fileName );
int printFiles( fileList *headFile );

int main( int argc, char **argv ){
	fileList *headFile;

	if( argc < 2 ){
		fprintf( stderr, "ERROR: Not enough arguments\n" );
	} else {
		headFile = fileSearch( getcwd( NULL, 0 ), argv[ 1 ] );
		if( headFile == NULL ){
			printf( "File not found\n" );
		} else {
			printFiles( headFile );
		}
	}

	return( EXIT_SUCCESS );
}

fileList *fileSearch( const char *currentDirectory, const char *fileName ){
	DIR *dir;
	struct dirent *dirStruct;
	char *tmpString;
	char *allocString;
	fileList *headFile;
	fileList *currentFile;
	fileList *tmpFile;

	dir = opendir( currentDirectory );

	if( dir == NULL ){
		// file
		if( strcmp( currentDirectory, fileName ) == 0 ){
			headFile = malloc( sizeof( fileList *));
			headFile->path = malloc( sizeof( char ) * ( strlen( currentDirectory ) + 1 ));
			sprintf( headFile->path, "%s", currentDirectory );
			headFile->nextFile = NULL;
			headFile->prevFile = NULL;
			return( headFile );
		} else {
			return( NULL );
		}
	} else {
		headFile = malloc( sizeof( fileList * ));
		headFile->nextFile = headFile;
		headFile->prevFile = headFile;
		currentFile = headFile;
		while(( dirStruct = readdir( dir )) != NULL ){
			if( strcmp( dirStruct->d_name, "." ) != 0 && strcmp( dirStruct->d_name, ".." ) != 0 ){
				//printf("%s\n", dirStruct->d_name );
				chdir( currentDirectory );
				tmpFile = fileSearch( dirStruct->d_name, fileName );
				if( tmpFile != NULL ){
					if( currentFile->nextFile == currentFile ){
						currentFile->path = malloc( sizeof( char ) * ( strlen( currentDirectory ) + 1 + strlen( tmpFile->path ) + 1 ));
						sprintf( currentFile->path, "%s%c%s", currentDirectory, SLASH, tmpFile->path );
						currentFile->nextFile = tmpFile->nextFile;
						currentFile->prevFile = NULL;
						if( currentFile->nextFile != NULL ){
							currentFile->nextFile->prevFile = currentFile;
							currentFile = currentFile->nextFile;
							free( tmpFile );
							tmpFile = currentFile;
						}
					} else {
						currentFile->nextFile = tmpFile;
						currentFile->nextFile->prevFile = currentFile;
						currentFile = currentFile->nextFile;
					}
					while( currentFile->nextFile != NULL ){
						allocString = malloc( sizeof( char ) * ( strlen( currentDirectory ) + 1 + strlen( tmpFile->path ) + 1 ));
						sprintf( allocString, "%s%c%s", currentDirectory, SLASH, tmpFile->path );
						tmpString = tmpFile->path;
						currentFile->path = allocString;
						free( tmpString );
						currentFile = currentFile->nextFile;
						tmpFile = tmpFile->nextFile;
					}
					if( currentFile->prevFile != NULL ){
						allocString = malloc( sizeof( char ) * ( strlen( currentDirectory ) + 1 + strlen( tmpFile->path ) + 1 ));
						sprintf( allocString, "%s%c%s", currentDirectory, SLASH, tmpFile->path );
						tmpString = tmpFile->path;
						currentFile->path = allocString;
						free( tmpString );
					}
				}
				if( strcmp( currentDirectory, "." ) != 0 ){
					chdir( ".." );
				}

			}
		}
		if( headFile->nextFile == headFile ){
			free( headFile );
			headFile = NULL;
		}
		free( dirStruct );
		free( dir );
		return( headFile );
	}

}

int printFiles( fileList *headFile ){
	fileList *currentFile;
	int count;
	
	currentFile = headFile;
	count = 0;
	while( currentFile != NULL ){
		printf("%s\n", currentFile->path );
		currentFile = currentFile->nextFile;
		++count;
	}
	return( count );
}
