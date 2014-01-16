/******************************************************************************
 *
 * ISS
 *
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "libbitmap.h"	// moja pseudokniznica na narabanie s bmp obrazkami
#include <math.h>

///////////////////////////////////////////////// PRIKLAD 1 ///////////////////////////////////////////////
void
sharpen(struct bmp *p)
{
	double filter[3][3]={
				{-0.5,	-0.5,	-0.5},
				{-0.5,	5.0,	-0.5},
				{-0.5,	-0.5,	-0.5}
	};

	// toto by chcelo alokovat dynamicky ale preco sa s tym hrat ked netreba (viz FAQ)
	double buffer[512][512] = {0};

	for(int y=0; y < *p->height; y++)
		for(int x=0; x < *p->width; x++)	// pre kazdy pixel
			for(int i = 0; i < 3; i++)
				for(int j = 0; j < 3; j++)	// pre kazde okolie
					buffer[y][x] = buffer[y][x] + (filter[i][j] * (double)get_pixel(p,x+j-1,y+i-1));

		//urezanie <0, >255
		for(int y=0; y < *p->height; y++)
			for(int x=0; x < *p->width; x++)
			{
				if(round(buffer[y][x]) > (double)255)
					set_pixel(p,x,y,(unsigned char)255);
				else if(round(buffer[y][x]) < (double)0)
					set_pixel(p,x,y,(unsigned char)0);
				else
					set_pixel(p,x,y, round(buffer[y][x]));
			}
}

///////////////////////////////////////////////// PRIKLAD 2 ///////////////////////////////////////////////
//todo: prejmenovat, nerotujeme
void rotate(struct bmp *p)
{
	int x1,x2;
	for(int y=0; y < *p->height; y++)
		for(int x=0; x < *p->width/2; x++)
		{
			x1 = get_pixel(p,x,y);
			x2 = get_pixel(p,*p->width-x-1,y);
			set_pixel(p,x,y,x2);
			set_pixel(p,*p->width-x-1,y,x1);
		}
}

///////////////////////////////////////////////// PRIKLAD 3 ///////////////////////////////////////////////
// pomocna funkcia pre qsort
int
int_cmp(void *a, void *b)
{
	return *(unsigned char *)a - *(unsigned char *)b;
}

void
median(struct bmp *src, struct bmp *res)
{
	for(int y=0; y < *src->height; y++)
		for(int x=0; x < *src->width; x++)
		{
			int k = 0;
			unsigned char window[25] = {0};

			// napln okno okolnymi bodmi
			for(int i = y-2; i <= y+2; i++)
				for(int j = x-2; j <= x+2; j++)
					window[k++] = get_pixel(src,j,i);

			k = 0; // reset

			qsort(window, 25, sizeof(unsigned char), int_cmp);
			
			set_pixel(res,x,y,window[12]);
		}
}

///////////////////////////////////////////////// PRIKLAD 4 ///////////////////////////////////////////////
void blur(struct bmp *p)
{
	double filter[5][5] = { {1, 1, 1, 1, 1},
							{1, 3, 3, 3, 1},
							{1, 3, 9, 3, 1},
							{1, 3, 3, 3, 1},
							{1, 1, 1, 1, 1}
	};
	// toto by chcelo alokovat dynamicky ale preco sa s tym hrat ked netreba
	double buffer[512][512] = {0};

	for(int y=0; y < *p->height; y++)
		for(int x=0; x < *p->width; x++)
		{
			for(int i = 0; i < 5; i++)
				for(int j = 0; j < 5; j++)
				{
					double step =  filter[i][j]/49 * (double)get_pixel(p,x+j-2,y+i-2);
					buffer[y][x] = buffer[y][x] + step;
				}
		}

	for(int y=0; y < *p->height; y++)
		for(int x=0; x < *p->width; x++)
			set_pixel(p,x,y,round(buffer[y][x]));
}

///////////////////////////////////////////////// PRIKLAD 4.5 - CHYBY ///////////////////////////////////////////////
void
error_avg(struct bmp *dirty, struct bmp *clean)
{
	double d[512][512]={0},c[512][512]={0};
	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
		{
			d[y][x] = get_pixel(dirty,x,y);
			c[y][x] = get_pixel(clean,x,y);
		}

	double error = 0;

	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
			error += fabs(d[y][x]-c[y][x]);

	error /= 512*512;
	printf("chyba=%.4f\n",error);
}

///////////////////////////////////////////////// PRIKLAD 5 ///////////////////////////////////////////////
void
stretch_histogram(struct bmp *p)
{
	double buffer[512][512] = {0};
	uint8_t min=255,max=0;
	uint8_t pixel;

	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
		{
			pixel = get_pixel(p,x,y);
			buffer[y][x] = pixel;
			if(pixel > max)
				max = pixel;
			if(pixel < min)
				min = pixel;
		}

	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
			buffer[y][x]= (buffer[y][x] - min) * (((double)255) / (max-min));

	for(int y=0; y < *p->height; y++)
		for(int x=0; x < *p->width; x++)
		{
			//kontrola
			if(buffer[y][x] > max)
				max = buffer[y][x];
			else if(buffer[y][x] < min)
				min = buffer[y][x];
			set_pixel(p,x,y, round(buffer[y][x]));
		}
	assert(min == 0 && max == 255);
}


///////////////////////////////////////////////// PRIKLAD 5.5 DEVIACIA ///////////////////////////////////////////////
void
deviation(struct bmp *before, struct bmp *after)
{
	double a[512][512] = {0};
	double b[512][512] = {0};

	double sum_no_hist=0;
	double sum_no_hist_squared=0;
	double variance_no_hist=0;

	double sum_hist=0;
	double sum_hist_squared=0;
	double variance_hist=0;


	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
		{
			b[y][x] = get_pixel(before,x,y);
			sum_no_hist += b[y][x];
			sum_no_hist_squared += b[y][x] * b[y][x];

			a[y][x] = get_pixel(after,x,y);
			sum_hist += a[y][x];
			sum_hist_squared += a[y][x] * a[y][x];
		}

	double mean_no_hist= sum_no_hist / (512*512);
	double mean_hist= sum_hist / (512*512);


        for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
		{
			const double difference_from_mean_hist = ((double)a[y][x]) - mean_hist;
			const double difference_from_mean_no_hist = ((double)b[y][x]) - mean_no_hist;
        		
			variance_no_hist += pow(difference_from_mean_no_hist,2);
			variance_hist += pow(difference_from_mean_hist,2);
    		}
	
	double std_no_hist = sqrt(variance_no_hist / (512*512));
	double std_hist = sqrt(variance_hist / (512*512));

	printf(	"mean_no_hist=%.4f\n"
		"std_no_hist=%.4f\n"
		"mean_hist=%.4f\n"
		"std_hist=%.4f\n",
		mean_no_hist,std_no_hist,
		mean_hist, std_hist
	);



}

///////////////////////////////////////////////// PRIKLAD 6 ///////////////////////////////////////////////
void
quantizate(struct bmp *p)
{
	double buffer[512][512] = {0};

	const uint8_t max = 255;
	const uint8_t min = 0;
	const uint8_t N = 2;


	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
			buffer[y][x] = round((pow(2,N)-1)*((double)get_pixel(p,x,y)-min)/(max-min))*(max-min)/(pow(2,N)-1)+min;

	for(int y=0; y < 512; y++)
		for(int x=0; x < 512; x++)
			set_pixel(p,x,y,(uint8_t)buffer[y][x]);
}


/////////////////////////////////////////////////    MAIN   ///////////////////////////////////////////////
void
main()
{
	// todo smazat, pouzite na testovanie
	struct bmp *ref0 = bmp_load("../ref/step0.bmp");
	struct bmp *ref1 = bmp_load("../ref/step1.bmp");
	struct bmp *ref2 = bmp_load("../ref/step2.bmp");
	struct bmp *ref3 = bmp_load("../ref/step3.bmp");
	struct bmp *ref4 = bmp_load("../ref/step4.bmp");
	struct bmp *ref5 = bmp_load("../ref/step5.bmp");
	struct bmp *ref6 = bmp_load("../ref/step6.bmp");

	struct bmp *xpruzi01 = bmp_load("../xpruzi01.bmp");

	// STEP 1
	struct bmp *step1 = bmp_cpy(ref0, "../step1.bmp");
	sharpen(step1);
//	assert(bmp_compare(step1,ref1) == 0);
	error_avg(step1,ref1);								//DONE (round() pouzite)

	// STEP 2
	struct bmp *step2 = bmp_cpy(step1, "../step2.bmp");
	rotate(step2);
//	assert(bmp_compare(step2,ref2) == 0);
	error_avg(step2,ref2);								//DONE
	
	// STEP 3
	struct bmp *step3 = bmp_cpy(step2, "../step3.bmp");
	median(step2,step3);
//	assert(bmp_compare(step3,ref3) == 0);
	error_avg(step3,ref3);								//DONE


	// STEP 4
	struct bmp *step4 = bmp_cpy(step3, "../step4.bmp");
	blur(step4);
//	assert(bmp_compare(step4,ref4) == 0);
	error_avg(ref4,step4);								//DONE round()

	// STEP 4.5 - Chyba v obraze
	struct bmp *invert4 = bmp_cpy(step4, "invert4.bmp");
	rotate(invert4);
	error_avg(invert4,ref0);

	// STEP 5
	struct bmp *step5 = bmp_cpy(step4, "../step5.bmp");
	stretch_histogram(step5);
	error_avg(ref5,step5);								//DONE round()

	// STEP 5.5
	deviation(step4,step5);

	
	// STEP 6
	struct bmp *step6 = bmp_cpy(step5, "../step6.bmp");
	quantizate(step6);
	error_avg(ref6,step6);								// DONE

	// odmapuje bmp vytvorene bmp subory (ulozi ich)
	bmp_free_save(&xpruzi01);
	bmp_free_save(&step1);
	bmp_free_save(&step2);
	bmp_free_save(&step3);
	bmp_free_save(&step4);
	bmp_free_save(&invert4);
	bmp_free_save(&step5);
	bmp_free_save(&step6);
}



