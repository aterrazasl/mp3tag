/*
 * mp3tag.h
 *
 *  Created on: Feb 28, 2019
 *      Author: aterrazas
 */

#ifndef MP3TAG_H_
#define MP3TAG_H_

#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

extern char *optarg;

static struct option const long_opts[] = {
		{ "debug", no_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ "file", required_argument, NULL, 'f' },
		{ "output", required_argument, NULL, 'o' },
		{ "info", no_argument, NULL, 'i' },
		{ "title", required_argument, NULL, 't' },
		{ "artist", required_argument, NULL, 'a' },
		{ "album", required_argument, NULL, 'b' },
		{ "year", required_argument, NULL, 'y' },
		{ "track", required_argument, NULL, 'k' }
};
#endif /* MP3TAG_H_ */
