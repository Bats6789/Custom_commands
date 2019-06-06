CC = gcc # Compiler to use
OPTIONS = -O2 -g -Wall # -g for debug, -O2 for optimise and -Wall additional messages
INCLUDES = -L library -l library # Directory for header file
FILES = liblibrary.a helpC dirList merge fileList organizeFiles # List of files to be build
LIBRARY = CBWfile.o CBWstring.o # Library files
.PHONY: all clean # To declare all, clean are not files

all: ${FILES}

liblibrary.a: ${LIBRARY}
	ar  -rc library/liblibrary.a library/*.o

CBWfile.o:
	${CC} -c source/CBWfile.c -o library/CBWfile.o

CBWstring.o:
	${CC} -c source/CBWstring.c -o library/CBWstring.o

helpC:
	${CC} source/helpC.c -o commands/helpC ${INCLUDES} ${OPTIONS}

dirLIst:
	${CC} source/dirList.c -o commands/dirList ${INCLUDES} ${OPTIONS}

merge:
	${CC} source/merge.c -o commands/merge ${INCLUDES} ${OPTIONS}

fileList:
	${CC} source/fileList.c -o commands/fileList ${INCLUDES} ${OPTIONS}

organizeFiles:
	${CC} source/organizeFiles.c -o commands/organizeFiles ${INCLUDES} ${OPTIONS}
