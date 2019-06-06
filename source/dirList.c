/*
 * File: dirList.c
 * Author: Blake Wingard
 * Desc: Prints a list of every file and sub-directory in the current directory.
 * Vers: 1.2.0 05/21/19 CBW - Added directory change
 * Vers: 1.1.0 05/19/19 CBW - Added help
 * Vers: 1.0.0 05/18/19 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void help( void );
int dirRead( FILE *fOutput, int level, char *cd );

int main( int argc, char **argv ){
	FILE *fOutput;		// the file the directory list is printed to.
	char dir[100];		// the name of the current directory.

	// if no file name is provided, use a default name.
	if( argc == 1 ){
		fOutput = fopen( "directoryList.txt", "w" );
		dirRead( fOutput, 0, getcwd( dir, 100 ) ); // read the contents of the directory and sub-directories into fOutput.
	} else if(( argc == 2 ) && ( strcmp( argv[ 1 ], "/h" ) != 0 )){
		fOutput = fopen( argv[ 1 ], "w" );
		dirRead( fOutput, 0, getcwd( dir, 100 ) ); // read the contents of the directory and sub-directories into fOutput.
	} else if( argc == 3 ){
		fOutput = fopen( argv[ 1 ], "w" );
		if( chdir( argv[ 2 ] ) == -1 ){ // change to user specified directory
			printf("ERROR opening %s\n", argv[ 2 ] );
			help();
			return( EXIT_FAILURE );
		}
		dirRead( fOutput, 0, getcwd( dir, 100 ) ); // read the contents of the directory and sub-directories into fOutput.
	} else {
		help();
		return( EXIT_FAILURE );
	}


	// close the output file
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
 * 	cd	- current directory when first called, and the name of the sub-directory when called recursively.
 */
int dirRead( FILE *fOutput, int level, char *cd ){
	// print the current directory or sub-directory to the file.
	fprintf( fOutput, "%s\n", cd );
	// if in a level > 0, change dir using cd
	if( level > 0 ){
		// check the directory change was successful.
		if( chdir( cd ) == -1 ){
			printf("ERROR opening %s\n", cd );
			return( EXIT_FAILURE );
		}
	}

	// variable declaration.
	FILE *dirList;		// the process of dir on current directory
	char chunk;		// current character
	int skipLine = 5;	// skips the first five lines which is directory information (not needed)
	int skip = 1;		// skips characters while true.
	int waitTillSpace = 1;	// wait until current character is a space. (needed for skipping the minutes on the time, or to skip the size of a file)
	int waitTillNum = 1;	// wait until current character is a number. (needed for skipping over the blank-space in a file line for dir.)
	int isDir = 0;		// flag for if current line is a sub-directory line.
	int index;		// index variable

	dirList = popen( "dir /o-gn", "r" ); // read the directory using popen ( /o-gn sorts the output of dir. 'g' sorts by directory first, '-g' is the reverse. 'n' sorts it alphabetically.)
	while( fscanf( dirList, "%c", &chunk ) != EOF ){
		// skip 5 information lines.
		if(( skipLine > 0 ) && ( chunk == '\n' )){
			--skipLine;
		} else if( skipLine == 0 ){
			if(( chunk == '\n' ) && ( skip != 1 )){ // just in case I missed a newline
				skip = 1;
				fprintf( fOutput, "%c", chunk );
			} else if(( skip == 1 ) && ( chunk == ':' )){ // skips the information I don't care about. (misses the minutes because I thought looking for ':' and skipping those two numbers later was easier.)
				skip = 0;
				waitTillSpace = 1;	// skips the minutes
				waitTillNum = 1;	// waits till the size of the file is reached (if it's a file)
			} else if( skip == 0 ){
				if(( waitTillSpace ==  1 ) && ( chunk == ' ' )){ // does two things. 1: skips the minutes I didn't handle earlier. 2: skips the size of a file.
					waitTillSpace = 0;
				} else if(( waitTillNum == 1 ) && (( chunk >= 48 ) && ( chunk <= 57 )) && ( waitTillSpace == 0 )){ // skips all the blank-space between the size of a file and after the time.
					waitTillNum = 0;
					waitTillSpace = 1;
				} else if(( isDir == 1 ) || ( chunk == '<' )){ // the '<' tag appears in dir only when it's a directory.
					while(( fscanf( dirList, "%c", &chunk ) != EOF ) && ( chunk != '>' )){ // skips the <dir> tag.
						// do nothing else
					}
					while(( fscanf( dirList, "%c", &chunk ) != EOF ) && ( chunk == ' ' )){ // skips the remaining blank-space between '>' and the directory name.
						// do nothing else
					}
					char newDir[150];	// new directory name
					index = 0;
					newDir[ index ] = chunk;// current chunk is a character do to the last while loop
					++index;
					// collect the directory name
					while(( fscanf( dirList, "%c", &chunk ) != EOF ) && ( chunk != '\n' ) && ( index < 149 )){
						newDir[ index ] = chunk;
						++index;
					}
					newDir[ index ] = '\0'; // always end a string with EOS
					// this skips the '.' and '..' directories. It's crucial to skip '..' otherwise an infinite recursive loop will occur.
					if(( strcmp( newDir, ".." ) != 0 ) && ( strcmp( newDir, "." ) != 0 )){
						// indent sub-directories.
						for( index = 0; index < level + 1; ++index ){
							fprintf( fOutput, "\t" );
						}
						fprintf( fOutput, "\\" ); // add a '\' to represent a sub-directory
						dirRead( fOutput, level + 1, newDir ); // call dirRead function specifying it's one level deeper.
					}
					isDir = 0; // no longer on the directory line
					skip = 1;  // done reading current line
				} else if(( waitTillSpace == 0 ) && ( waitTillNum == 0 )){ // after skipping over the file size, the name of the file can be printed.
					fprintf( fOutput, "- " ); // adds a tag to identify a file
					// indent the file name if currently reading the files in a sub-directory
					for( index = 0; index < level + 1; ++index ){ 
						fprintf( fOutput, "\t" );
					}
					fprintf( fOutput, "%c", chunk ); // current chunk is a character.
					// read the rest of the filename to the output file.
					while(( fscanf( dirList, "%c", &chunk ) != EOF ) && ( chunk != '\n' )){
						fprintf( fOutput, "%c", chunk );
					}
					fprintf( fOutput, "%c", chunk ); // print the newline character.
					skip = 1; // finished with current line
				}
			}
		}
	}

	if( level > 0 ){ // if in a sub-directory, return the parent directory
		chdir( ".." );
	}
	pclose( dirList ); // close process

	return( EXIT_SUCCESS );
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
