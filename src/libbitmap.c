/******************************************************************************
 *
 * Pseudoimplementacia bmp kniznice
 *
 * Author: Tomas Pruzina <pruzinat@gmail.com>
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/

#include "libbitmap.h"

void
bmp_parse(struct bmp *p)
{
	assert(p);
	assert(p->bmp_file);
	
	p->header = p->bmp_file;
	p->size = (void*) (p->bmp_file +0x2);
	p->offset = (void*) (p->bmp_file + 0xA);
	
	p->width = (void*) (p->bmp_file + 0x12);
	p->height = (void*) (p->bmp_file + 0x16);
	p->bpp = (void*) (p->bmp_file + 0x1C);

	p->row_size = (((*p->bpp) * (*p->width) + 31) /32) *4;

	// get actual pointer to bmp data
	p->pixel_array = p->bmp_file + (uint32_t) *p->offset;
}

void bmp_print(struct bmp *p)
{
	assert(p);
	assert(p->bmp_file);

	printf("size: %d bytes\n", (int)(*(p->size)));
	printf("height: %d bytes\n", (uint32_t)(*p->height));
	printf("width: %d bytes\n", (uint32_t)(*p->width));
	printf("bpp: %d bytes\n", (uint32_t)(*p->bpp));
	printf("row size: %d bytes\n", p->row_size);
}

uint8_t
get_pixel(struct bmp *p, int x, int y)
{
	assert(p);
	assert(p->bmp_file);
	assert(p->pixel_array);

	if(x < 0 || y < 0 || x >= *p->width || y >= *p->height)
		return 0;

	uint8_t *pp = p->pixel_array + (y * p->row_size) + x;
	return *pp;
}

void
set_pixel(struct bmp *p, int x, int y, uint8_t pix_data)
{
	uint8_t *pp = p->pixel_array + ((y * p->row_size) + x);
	
	assert(pp >= p->pixel_array);
	assert(pp < p->pixel_array + *(p->size)* (*p->bpp));

	*pp = *(char*)(&pix_data);
}

struct bmp *
bmp_load(char *filename)
{
	assert(filename);
	struct bmp *p = malloc(sizeof(struct bmp));
	assert(p);
	
	// todo: bmp init
	memset(p, 0, sizeof(struct bmp));
	
	p->fd = open(filename, O_RDONLY);
	if(fstat(p->fd, &p->sb) == -1) {
		//handle error
		assert(p->sb.st_size > 0);
	}
	p->bmp_file = mmap(NULL, p->sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, p->fd, 0);
	if(p->bmp_file == MAP_FAILED) {
		//handle error
	}

	bmp_parse(p);

	return p;
}

struct bmp *
bmp_cpy(struct bmp *src, char *filename)
{
	assert(src && filename);
	assert(src->bmp_file);

	struct bmp *cpy = malloc(sizeof(struct bmp));

	// assumes initialized an loaded bmp file
	memcpy(cpy,src,sizeof(struct bmp));
	//assert(cpy);

	
	cpy->fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(cpy->fd == -1) {
		assert(0);
	}

	if(ftruncate(cpy->fd, src->sb.st_size) == -1) {
		assert(0);
	}

	cpy->bmp_file = mmap(NULL, src->sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, cpy->fd, 0);
	if(cpy ==  MAP_FAILED) {
		assert(0);
	}
	
	memcpy(cpy->bmp_file, src->bmp_file, src->sb.st_size);
		//todo: handle error
		//

	if(msync(cpy->bmp_file, src->sb.st_size, MS_SYNC) == -1) {
		//todo: handle error
	}
	
	bmp_parse(cpy);

	return cpy;
}

int
bmp_compare(struct bmp *a, struct bmp *b)
{
	assert(a && b);
	assert(*a->height == *b->height);
	assert(*a->width == *b->width);
	assert(*a->size == *b->size);
	
	int res=0;

	for(uint32_t y=0; y < *a->height; y++)
		for(uint32_t x=0; x < *a->width; x++)
			if(0 != (res = get_pixel(a,x,y) - get_pixel(b,x,y)))
				return res;
	return 0;
}

void
bmp_free_save(struct bmp **pp)
{
	munmap((*pp)->bmp_file, (*pp)->sb.st_size);
	close((*pp)->fd);
	free(*pp);
	*pp=NULL;
}
