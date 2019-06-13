/*
 * File: dirList.c
 * Author: Blake Wingard
 * Desc: Prints a list of every file and sub-directory in the current directory.
 * Vers: 2.0.0 06/12/19 CBW - Linux compatible.
 * Vers: 1.2.0 05/21/19 CBW - Added directory change.
 * Vers: 1.1.0 05/19/19 CBW - Added help
 * Vers: 1.0.0 05/18/19 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void help( void );
void dirRead( FILE *fOutput, int level, char *directoryName );
int main( int argc, char **argv ){
	FILE *fOutput;
  char dir[100];

	// if no file name is provided, use a default name.
	if( argc == 1 ){
		fOutput = fopen( "directoryList.txt", "w" );
		dirRead( fOutput, 0, getcwd( dir, 100 ) );
	} else if(( argc == 2 ) && ( strcmp( argv[ 1 ], "/h" ) != 0 )){ // user provided file
		fOutput = fopen( argv[ 1 ], "w" );
		dirRead( fOutput, 0, getcwd( dir, 100 ) );
	} else if( argc == 3 ){ // user provided alternate directory
		fOutput = fopen( argv[ 1 ], "w" );
		if( chdir( argv[ 2 ] ) == -1 ){
			printf("ERROR opening %s\n", argv[ 2 ] );
			help();
			return( EXIT_FAILURE );
		}
		dirRead( fOutput, 0, getcwd( dir, 100 ) );
	} else {
		help();
		return( EXIT_FAILURE );
	}
	fclose( fOutput );

	return( EXIT_SUCCESS );
}

/*
 * Name: int dirRead( FILE *fOutput, int level )
 * Desc: reads the current directory and sub-directories and prints
 * 		that to a file
 * Args:
 * 	fOutput - file pointer that dirRead is printing to
 * 	level	- how deep in the sub-directory the program is
 * 	directoryName	- current directory when first called, and the name of the sub-directory when called recursively.
 */
void dirRead( FILE *fOutput, int level, char *directoryName ){
     DIR *directory;
     struct dirent *directoryList;
     int index;

     if(( directory = opendir( directoryName )) == NULL ){
          fprintf( fOutput, "- " );
          for( index = 0; index < level; ++index ){
               fprintf( fOutput, "\t" );
          }
          fprintf( fOutput, "%s\n", directoryName );
     } else {
          for( index = 0; index < level; ++index ){
               fprintf( fOutput, "\t" );
          }
          fprintf( fOutput, "%s/\n", directoryName );
          while(( directoryList = readdir( directory )) != NULL ){
               if(( strcmp( directoryList->d_name, "." ) != 0 ) && ( strcmp( directoryList->d_name, ".." ) != 0 )){
                    dirRead( fOutput, level + 1, directoryList->d_name );
               }
          }
          closedir( directory );
     }
}

/*
 * Name: int help( void )
 * Desc: Prints information to the user
 * Args: (none)
 */
void help( void ){
	printf("File: dirList.c\n");
	printf("Author: Blake Wingard\n");
	printf("Desc: Prints a list of every file and sub-directory in the current directory.\n");
	printf("Vers: 1.2.0 05/21/19 CBW - Added directory change\n");
	printf("Vers: 1.1.0 05/19/19 CBW - Added help\n");
	printf("Vers: 1.0.0 05/18/19 CBW - Original code.\n");
	printf("===================================================================================================================================\n");
	printf("\n");
	printf("dirList.exe - creates a text file (named directoryList.txt) containing the file names and sub-directories of the current directory\n");
	printf("	/h - help\n");
	printf("	<file name> - the name of the output file\n");
	printf("		<directory> - the directory you want to run the command in. (works with relative and absolute paths)\n");
	printf("\n");
	printf("=================================================================================================================================\n");
	printf("\n");
	printf("(examples)\n");
	printf("dirList.exe - creates a text file (named directoryList.txt) containing the file names and sub-directories of the current directory\n");
	printf("dirList.exe directory.txt - creates a text file (named directory.txt) containing the file names and sub-directories of the current directory\n");
  printf("dirList.exe directory.txt .. - creates a text file (named directory.txt) containing the file names and sub-directories of the parent directory of the current directory	\n");
	printf("dirList.exe /h - help\n");
	printf("\n");
	printf("=================================================================================================================================\n");
}
