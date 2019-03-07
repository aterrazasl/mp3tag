/*
 * ID3v2.c
 *
 *  Created on: Feb 28, 2019
 *      Author: aterrazas
 */
#include "ID3v2.h"

//The ID3v2 tag size is encoded with four bytes where the most significant bit (bit 7)
//is set to zero in every byte, making a total of 28 bits
unsigned int convert7bits(unsigned char* size) {
	unsigned int isize = 0;
//	printf("Size2 \t= %02x%02x%02x%02x\n", size[0], size[1], size[2], size[3]);
	isize = (size[0]  / 2) * 0x01000000;
	isize += (size[1] / 2) * 0x010000;
	isize += (size[2] / 2) * 0x0100;
	isize += size[3];
	return isize;
}

//Format ID3v2 header and prints the first 10bytes
int printID3header(ID3v2 *header) {
	printf("ID tag \t= %c%c%c\n", header->ID[0], header->ID[1], header->ID[2]);
	printf("Ver \t= 0x%02x%02x\n", header->version[0], header->version[1]);
	printf("Flags \t= 0x%02x\n", header->flags);
	printf("Size \t= 0x%02x%02x%02x%02x\n", header->size[0], header->size[1],
			header->size[2], header->size[3]);
	printf("Size \t= %u bytes\n", convert7bits(header->size));

	return 0;
}

//Reads the ID3v2 header from file
//Returns -1 if invalid header found otherwise return the ID3v2 info size
unsigned int readID3header(char * file, char checkVersion){

	ID3v2 *header;
	unsigned int headerSize;
	FILE* f1;

	header = calloc(ID3v2_HEADER_SIZE, 1);

	f1 = fopen(file, "rb");
	fread(header, ID3v2_HEADER_SIZE, 1, f1);
	fclose(f1);

	if(memcmp(header->ID, "ID3", 3))
		return -1;

	if(checkVersion){
		if(memcmp(header->version, ID3v2_VERSION, 2))
			return -1;
	}

	headerSize = convert7bits(header->size);
	//printID3header(header);

	free(header);

	return headerSize;
}

//Convert from char[] size to int
int convertFrameSize(unsigned char * size) {
	int isize = 0;
//	printf("Size2 \t= %02x%02x%02x\n", size[0], size[1], size[2]);

	isize += size[0] * 0x01000000;
	isize += size[1] * 0x010000;
	isize += size[2] * 0x0100;;
	isize += size[3];
	return isize;
}

//prints all frames tags from file
//Frame ID $xx xx xx xx (four characters) Size $xx xx xx xx Flags $xx xx
int printID3frames(char * file, char checkVersion){
	ID3v2Frame *frames;
	char rawData[200];
	FILE* f1;
	unsigned int headerSize = readID3header(file, checkVersion);

	int i;
	for (i=0; i<200; i++){
		rawData[i] =0x00;
	}

	if(headerSize == -1){
		printf("ID3v2 Header not found in file %s\n",file);
		return -1;
	}

	f1 = fopen(file, "rb");
	fseek(f1, ID3v2_HEADER_SIZE, SEEK_CUR);	//set position file after header

	unsigned char rawFrame[ID3v2_FRAME];
	while ((ftell(f1) < (headerSize + ID3v2_HEADER_SIZE))) {
		//    		printf("file position = %i\n",ftell(f1));

		fread(rawFrame, 1, ID3v2_FRAME, f1);
		frames = (ID3v2Frame*) rawFrame;

//		printf("---- frame size = %02x%02x%02x%02x -----\n", rawFrame[0], rawFrame[1],
//				rawFrame[2],rawFrame[3]);

		if ((frames->ID[0] == 0x00) | (frames->ID[0] != 'T'))
		{
			fseek(f1, convertFrameSize(frames->size), SEEK_CUR);	//skip 1 byte of encoding
			continue;
		}

//		printf("frame ID \t= %c%c%c%c, frame size = %u, next pos = %u\n",
//				frames->ID[0], frames->ID[1], frames->ID[2], frames->ID[3],
//				convertFrameSize(frames->size),
//				(unsigned int)ftell(f1) + convertFrameSize(frames->size));

		fseek(f1, ID3v2_ENCODING_SIZE, SEEK_CUR);	//skip 1 byte of encoding
		fread(rawData, 1, convertFrameSize(frames->size) - ID3v2_ENCODING_SIZE, f1);

		rawData[convertFrameSize(frames->size)- ID3v2_ENCODING_SIZE] = 0x00;

		printf("Frame ID = %c%c%c%c, Data = %s, size %i\n",
				frames->ID[0], frames->ID[1], frames->ID[2], frames->ID[3],
				rawData, convertFrameSize(frames->size) );


		//fseek(f1, convertFrameSize(frames->size), SEEK_CUR);



	}
	fclose(f1);
	return 0;
}

//This function change *size engdianess
int reverseBuf(unsigned char *size){
	char tmp[4];

	tmp[3] = *(size+0);
	tmp[2] = *(size+1);
	tmp[1] = *(size+2);
	tmp[0] = *(size+3);

	memcpy(size, tmp, 4);

	return 0;
}

//creates frame based on track data
//Returns the size of the frame including header
int createFrame(char * frameBuffer, track_info * track){

	ID3v2Frame frame;
	int totalSize = 0;
	//frame.enconding[0] =0x00;


	if(strlen(track->TIT2)){
//		printf("TIT2 = %s, lenght = %i\n", track->TIT2,strlen(track->TIT2));
		memcpy(frame.ID,"TIT2",ID3v2_FRAMEID_LENGHT);

		unsigned int size = strlen(track->TIT2) + ID3v2_ENCODING_SIZE;
		memcpy(frame.size,&size,ID3v2_FRAME_SIZE);
		reverseBuf(frame.size);

		memcpy(frame.flags,"\00\00",ID3v2_FLAGS_SIZE);

		memcpy(frameBuffer + totalSize, &frame, ID3v2_FRAME + ID3v2_ENCODING_SIZE);

		memcpy(frameBuffer+ID3v2_FRAME + ID3v2_ENCODING_SIZE +totalSize, track->TIT2, strlen(track->TIT2));

		totalSize += strlen(track->TIT2) + ID3v2_FRAME + ID3v2_ENCODING_SIZE;
	}

	if(strlen(track->TPE1)){
//		printf("TPE1 = %s, lenght = %i\n", track->TPE1,strlen(track->TPE1));
		memcpy(frame.ID,"TPE1",ID3v2_FRAMEID_LENGHT);

		unsigned int size = strlen(track->TPE1) + ID3v2_ENCODING_SIZE;
		memcpy(frame.size,&size,ID3v2_FRAME_SIZE);
		reverseBuf(frame.size);

		memcpy(frame.flags,"\00\00",ID3v2_FLAGS_SIZE);

		memcpy(frameBuffer+totalSize, &frame, ID3v2_FRAME + ID3v2_ENCODING_SIZE);

		memcpy(frameBuffer+ID3v2_FRAME + ID3v2_ENCODING_SIZE+totalSize, track->TPE1, strlen(track->TPE1));

		totalSize += strlen(track->TPE1) + ID3v2_FRAME + ID3v2_ENCODING_SIZE;
	}


	if(strlen(track->TALB)){
//		printf("TPE1 = %s, lenght = %i\n", track->TPE1,strlen(track->TPE1));
		memcpy(frame.ID, "TALB", ID3v2_FRAMEID_LENGHT);

		unsigned int size = strlen(track->TALB) + ID3v2_ENCODING_SIZE;
		memcpy(frame.size, &size, ID3v2_FRAME_SIZE);
		reverseBuf(frame.size);

		memcpy(frame.flags, "\00\00", ID3v2_FLAGS_SIZE);

		memcpy(frameBuffer + totalSize, &frame, ID3v2_FRAME + ID3v2_ENCODING_SIZE);

		memcpy(frameBuffer + ID3v2_FRAME + ID3v2_ENCODING_SIZE + totalSize, track->TALB, strlen(track->TALB));

		totalSize += strlen(track->TALB) + ID3v2_FRAME + ID3v2_ENCODING_SIZE;
	}



	if(strlen(track->TRCK)){
//		printf("TPE1 = %s, lenght = %i\n", track->TPE1,strlen(track->TPE1));
		memcpy(frame.ID,"TRCK",ID3v2_FRAMEID_LENGHT);

		unsigned int size = strlen(track->TRCK) + ID3v2_ENCODING_SIZE;
		memcpy(frame.size,&size,ID3v2_FRAME_SIZE);
		reverseBuf(frame.size);

		memcpy(frame.flags,"\00\00",ID3v2_FLAGS_SIZE);

		memcpy(frameBuffer+totalSize, &frame, ID3v2_FRAME + ID3v2_ENCODING_SIZE);

		memcpy(frameBuffer+ID3v2_FRAME + ID3v2_ENCODING_SIZE+totalSize, track->TRCK, strlen(track->TRCK));

		totalSize += strlen(track->TRCK) + ID3v2_FRAME + ID3v2_ENCODING_SIZE;
	}

	if(strlen(track->TYER)){
//		printf("TPE1 = %s, lenght = %i\n", track->TPE1,strlen(track->TPE1));
		memcpy(frame.ID,"TYER",ID3v2_FRAMEID_LENGHT);

		unsigned int size = strlen(track->TYER) + ID3v2_ENCODING_SIZE;
		memcpy(frame.size,&size,ID3v2_FRAME_SIZE);
		reverseBuf(frame.size);

		memcpy(frame.flags,"\00\00",ID3v2_FLAGS_SIZE);

		memcpy(frameBuffer+totalSize, &frame, ID3v2_FRAME + ID3v2_ENCODING_SIZE);

		memcpy(frameBuffer+ID3v2_FRAME + ID3v2_ENCODING_SIZE+totalSize, track->TYER, strlen(track->TYER));

		totalSize += strlen(track->TYER) + ID3v2_FRAME + ID3v2_ENCODING_SIZE;
	}



	return totalSize;
}

//creates new file with the ID3v2 track info
int createNewID3v2(char * file, track_info * track){
	FILE* f1;
	char rawBuffer[ID3v2_HEADER_SIZE];
	char rawFrame[ID3v2_MAX_FRAME_SIZE];
	int frameSize = 0;

	ID3v2 *header = (ID3v2*)rawBuffer;

	memcpy(header->ID, ID3v2_TAG_ID, ID3v2_TAG_SIZE);				//Create header tag
	header->flags = 0x00;											//Adds flags
	memcpy(header->size, "\00\00\00\00", ID3v2_HEADER_LENGHT);		//sets size temporary to 0x00
	memcpy(header->version, ID3v2_VERSION, ID3v2_VERSION_LENGHT);	//Sets the version of ID3v2 implementation

	frameSize = createFrame(rawFrame, track);						//Create frames for all track_info

	if(frameSize == 0){
		printf("%s","No data to add\n");
		return -1; //Nothing to add
	}
	memcpy(header->size, &frameSize, ID3v2_FRAME_SIZE);				//Sets the ID3v2 correct size
	reverseBuf(header->size);										//Change header size to bigendian


	//now write the data to temp file

	f1 = fopen(file, "wb");

	fwrite(header  , 1, ID3v2_HEADER_SIZE, f1);		//Writes header
	fwrite(rawFrame, 1, frameSize        , f1);		//Writes the frames

	fclose(f1);
	return 0;
}
