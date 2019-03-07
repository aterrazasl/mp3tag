/*
 * mp3tag.c
 *
 *  Created on: Feb 28, 2019
 *      Author: aterrazas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mp3tag.h"
#include "ID3v2.h"

//merge file1 and file2 offset into file3
void merge_file(char* file1, char* file2, char* file3, unsigned int offset) {
	FILE *f1, *f2, *f3;
	int ch;
	f1 = fopen(file1, "rb"); // Opening in write mode.
	f2 = fopen(file2, "rb");
	f3 = fopen(file3, "wb");

	if (f1 == NULL || f2 == NULL) {
		exit(EXIT_FAILURE);
	}

	fseek(f2, offset, SEEK_CUR);		// Adds the offset

	while ((ch = fgetc(f1)) != EOF) // Appending tagging info.
		fputc(ch, f3);

	while ((ch = fgetc(f2)) != EOF) // Appending media file info.
		fputc(ch, f3);

	// Closing the files.
	fclose(f1);
	fclose(f2);
	fclose(f3);
}

int printHelp(){
	printf("Usage: mp3tag [OPTION]... -f file.mp3 -o destination.mp3\n");
	printf("\t-h, --help\t\tPrints this help\n");
	printf("\t-i, --info\t\tPrints ID3v2 version 3 information\n");
	printf("\t-t, --title\t\tTitle name\n");
	printf("\t-a, --artist\t\tArtist name\n");
	printf("\t-b, --album\t\tAlbrum name\n");
	printf("\t-k, --track\t\tTrack number example: 05/10\n");
	printf("\t-y, --year\t\tYear\n");

	return 0;
}

int main(int argc, char *argv[]) {
	char tempFile[100] = "media_info.txt", inputFile[100], outputFile[100];
	int inputfileok = 0, outputfileok = 0, info = 0, debug;
	debug=0;
	track_info track;
	strcpy(track.TIT2, "");
	strcpy(track.TPE1, "");

	if (argc > 1) {
		int c = 0;
		while ((c = getopt_long(argc, argv, "dhif:o:t:a:b:k:y:", long_opts, NULL)) != -1) {
			switch (c) {
			case 'f':
				strcpy(inputFile, optarg);
				if (access(inputFile, F_OK)) {
					printf("File %s not found\n", inputFile);
					return ENOENT;
				}
				inputfileok = 1;

				break;
			case 'o':
				strcpy(outputFile, optarg);
				outputfileok = 1;
				break;
			case 'i':
				info = 1;
				break;
			case 'h':
				printHelp();
				break;
			case 't':
				strcpy(track.TIT2, optarg);
				break;
			case 'a':
				strcpy(track.TPE1, optarg);
				break;
			case 'b':
				strcpy(track.TALB, optarg);
				break;
			case 'y':
				strcpy(track.TYER, optarg);
				break;
			case 'k':
				strcpy(track.TRCK, optarg);
				break;
			case 'd':
				debug = 1;
				break;
			default:
				return 0;
				break;
			}
		}
	} else {
		//printf("%s\n", "missing arguments");
		printHelp();
		return 0;
	}

	unsigned int headerSize = readID3header(inputFile, 0);
	if (headerSize == -1) {
		printf("ID3v2 Header not found in file %s or version not 0x0300\n",
				inputFile);
		return 0;
	}

	if (info) {
		printID3frames(inputFile, 1);
		return 0;
	}

	if ((track.TIT2[0] == 0x00) & (track.TPE1[0] == 0x00)) {

		char * token;
		char * start = &inputFile[0];

		token = strchr(inputFile, '-');
		strcpy(track.TPE1, token + 2);
		track.TPE1[strlen(track.TPE1) - 4] = 0x00;

		strncpy(track.TIT2, start, token - start);
		track.TIT2[token - start - 1] = 0x00;

		strcat(outputFile, inputFile);
	}


	// Function to merge media info and media

	if ((createNewID3v2(tempFile, &track) ==0)  & (inputfileok) & (outputfileok))
		merge_file(tempFile, inputFile, outputFile,
				headerSize + ID3v2_HEADER_SIZE);

	return 0;
}
