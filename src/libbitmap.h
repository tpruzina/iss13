/******************************************************************************
 *
 * Pseudo-implementacia bmp kniznice
 *
 * Author: Tomas Pruzina <pruzinat@gmail.com>
 *
 * 		mmap je pouzity namapovanie suboru do pamate,
 * 		struct bmp len poskytuje ukazatele do namapovaneho suboru
 *		subor po bmp_load() je dost mozna R-only -> treba vytvorit kopiu a do nej pisat (viz main)
 *
 * EXTERNAL FEATURES:
 * 		- load file 		bmp_load()
 * 		- copy file			bmp_cpy()
 * 		- save file			bmp_save() -- works?
 * 		- print bmp stats	bmp_print()
 *
 * 		- get pixel x,y		get_pixel()
 * 		- set pixel x,y,val set_pixel()
 *
 * 		- copare two bmps	bmp_compare()
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/

#ifndef __BITMAP_H
#define __BITMAP_H


#define _POSIX_C_SOURCE 200809L	// bez tohto by gcc panikarilo

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>//fstat()
#include <fcntl.h>
#include <unistd.h> //ftruncate
#include <sys/types.h>
#include <assert.h> //assert()
#include <string.h> //memset(),memunset()
#include <stdint.h>


struct bmp
{
	char *bmp_file;	//mmapped file
	struct stat sb;
	int fd;

	// bitmap file header - 14 bytes
	char	*header;
	uint32_t *size;	//can be larger (4 extra bytes of padding)
	uint32_t *offset;


	uint32_t *width;
	uint32_t *height;
	uint32_t *bpp;
	uint32_t row_size;


	// Extra bit masks (optional)

	// color table
	
	// GAP1 (optional)

	// BMP DATA START HERE
	uint8_t *pixel_array;
	
	
	// GAP2 (optional)
	
	//ICC color profile (optional)
};

//struct pix_rgb
//{
//	unsigned char r:3;
//	unsigned char g:3;
//	unsigned char b:2;
//};


struct	bmp	*bmp_load(char *filename);
struct	bmp 	*bmp_cpy(struct bmp *src, char *filename);
void	bmp_free_save(struct bmp **pp);

void		bmp_parse(struct bmp *p);

void bmp_print(struct bmp *p);
unsigned char get_pixel(struct bmp *p, int x, int y);
void set_pixel(struct bmp *p, int x, int y, unsigned char pix_data);

int bmp_compare(struct bmp *a, struct bmp *b);

#endif
