/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : x.h
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h> 

extern Display *dpy;
extern GC gc;
extern Pixmap pixmap;
extern Atom delWin;
extern XEvent e;
extern unsigned long gray[], colors[], spectrum[];

typedef struct{
	unsigned short int *image_16;
	long *image_32;
	XImage *screen;
	int w, h, cx, cy;
	unsigned long size;
}IMG;

int create_x(int, int, char*);
int create_image(IMG *, int, int);

void set_graymap();
void set_colormap();
void set_spectrum();

void clear_image(IMG *img);
void burn_image(IMG *img);
void draw_buffer(int, int, int, int);

void put_pixel(IMG *img, int, int, unsigned long);
void put_apixel(IMG *img, int, int, unsigned long, float alpha);
void draw_line (IMG *img, int x0, int y0, int x1, int y1, unsigned long c);

void clear_buffer(int, int, int, int);
void draw_image(IMG *img, int x, int y, int w, int h);

