/*
 * ID3v2.H
 *
 *  Created on: Feb 28, 2019
 *      Author: aterrazas
 */

#ifndef ID3V2_H_
#define ID3V2_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


//ID3v2 version 3
//https://mutagen-specs.readthedocs.io/en/latest/id3/id3v2.3.0.html
#define ID3v2_HEADER_SIZE 10
#define ID3v2_TAG_ID "ID3"
#define ID3v2_TAG_SIZE 3
#define ID3v2_HEADER_LENGHT	4
#define ID3v2_VERSION "\03\00"
#define ID3v2_VERSION_LENGHT 2

#define ID3v2_MAX_FRAME_SIZE 500
#define ID3v2_FRAME 10
#define ID3v2_FRAMEID_LENGHT 4
#define ID3v2_FRAME_SIZE 4
#define ID3v2_FLAGS_SIZE 2
#define ID3v2_ENCODING_SIZE 1

typedef struct {
	char TRCK[10];	//Track number/Position in set
	char TYER[10];	//Year
	char TIT2[100];	//Title song
	char TPE1[100];	//Lead artist(s)/Lead performer(s)/Soloist(s)/Performing group
	char TALB[100];	//Album/Movie/Show title
	char TCON[100];	//Content type
        char TPE2[100];
}track_info;


struct tags // Structure to store tagging information.
{
	char title[100], artist[100], album[100], track_num[100], year[100],
			genre[100];
};

typedef struct {
	char ID[3];
	char version[2];
	char flags;
	unsigned char size[4];

}ID3v2;

typedef struct {
	char			ID[ID3v2_FRAMEID_LENGHT];
	unsigned char 	size[ID3v2_FRAME_SIZE];
	char			flags[ID3v2_FLAGS_SIZE];
	//char			enconding[ID3v2_ENCODING_SIZE]
} ID3v2Frame;


unsigned int readID3header(char * file, char checkVersion);
int printID3frames(char * file, char checkVersion);
int createNewID3v2(char * file, track_info * track);



#endif /* ID3V2_H_ */
