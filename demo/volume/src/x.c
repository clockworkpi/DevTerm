/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : x.c
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include "x.h"

Display *dpy;
GC gc, gc_out;
Colormap xcolors;
Atom delWin;
XEvent e;

int d_depth;
Window d;
Pixmap pixmap;
XSetWindowAttributes window_attributes;
XColor color;
unsigned long gray[0x100], colors[0x100], spectrum[0x400];

int create_x(int scr_w, int scr_h, char title[64])
{
	int s_number;
	if (!(dpy=XOpenDisplay(NULL)))
	{
		perror("XOpenDisplay");
		return -1;
	}

	s_number = DefaultScreen (dpy);
	d_depth = DefaultDepth (dpy, s_number);
	window_attributes.border_pixel = BlackPixel (dpy, s_number);
	window_attributes.background_pixel = BlackPixel (dpy, s_number);
	window_attributes.override_redirect = 0;

	d = XCreateWindow
	(
		dpy, 
		DefaultRootWindow (dpy), 
		0, 0,
		scr_w, scr_h, 
		0, 
		d_depth, 
		InputOutput, 
		CopyFromParent, 
		CWBackPixel | CWBorderPixel,
		&window_attributes
	);

	xcolors = DefaultColormap(dpy, s_number);
	XSetWindowColormap(dpy, d, xcolors);

	gc = XCreateGC (dpy, d, 0, NULL);
	gc_out = XCreateGC (dpy, d, 0, NULL);

	XSetStandardProperties(dpy, d, title, title, None, 0, 0, None);

	XSelectInput
	(
		dpy, d,
		ExposureMask | 
		KeyPressMask | 
		ButtonPressMask | ButtonReleaseMask | 
		Button1MotionMask |Button2MotionMask |
		StructureNotifyMask
	);

	delWin = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, d, &delWin, 1);

	XMapWindow (dpy, d);
	while(!(e.type == MapNotify)) XNextEvent(dpy, &e);

	pixmap = XCreatePixmap(dpy, d, scr_w, scr_h, d_depth);
 
	XSetFillStyle (dpy, gc_out, FillTiled);
	XSetTile (dpy, gc_out, pixmap);

	return 0;
}

void set_graymap()
{
	int i;
	for(i=0;i<0x100;i++)
	{
		color.red = 0x100 * i;
		color.green = 0x100 * i;
		color.blue = 0x100 * i;
			XAllocColor(dpy, xcolors, &color);
		gray[i] = color.pixel;
	}
}

void set_colormap()
{
	int i;
	for(i=0;i<0x100;i++)
	{
		color.red = 0x100 * i;
		color.green = 0x100 * abs(128-i);
		color.blue = 0x100 * (0xff-i);
			XAllocColor(dpy, xcolors, &color);
		colors[i] = color.pixel;
	}
}

void create_image_16(IMG *img, int w, int h)
{
	img->image_16 = (unsigned short int *) malloc (w * h * 2);
	img->screen = XCreateImage (dpy, CopyFromParent, d_depth, ZPixmap, 0,
		(char*)img->image_16, w, h, 16, w * 2);
	memset(img->image_16, 0x00, w * h * 2);
}

void create_image_32(IMG *img, int w, int h)
{
	img->image_32 = (long *) malloc (w * h * 4);
	img->screen = XCreateImage (dpy, CopyFromParent, d_depth, ZPixmap, 0,
		(char*)img->image_32, w, h, 32, w * 4);
	memset(img->image_32, 0x00, w * h * 4);
}

int create_image(IMG *img, int w, int h)
{
	img->w = w; img->h = h;

	if (d_depth == 8 || d_depth == 16)
	{
		create_image_16 (img, w, h);
		img->size = w * h * 2;
	}
	else if(d_depth == 24 || d_depth == 32)
	{
		create_image_32 (img, w, h);
		img->size = w * h * 4;
	}
	else
	{
		fprintf (stderr, "This is not a supported depth. %d\n",d_depth);
		return -1;
	}
	return 0;
}

void clear_image(IMG *img)
{
	if (d_depth == 8 || d_depth == 16)
		memset(img->image_16, 0x00, img->size);

	else if(d_depth == 24 || d_depth == 32)
		memset(img->image_32, 0x00, img->size);
}

void burn_image(IMG *img)
{
	int i, n = img->w * img->h;
	unsigned long c;
	unsigned int r, g, b;

	for (i=0; i<n; i++)
	{
		c = img->image_32[i];

		r = c >> 16;
		g = (c & 0x00ff00) >> 8;
		b = c & 0x0000ff;

		if (r > 0) r--;
		if (g > 0) g--;
		if (b > 0) b--;

		img->image_32[i] = (r << 16) | (g << 8) | b;
	}
}

void clear_buffer(int x, int y, int w, int h)
{
	XSetForeground(dpy, gc, 0);
	XFillRectangle(dpy, pixmap, gc, x, y, w, h);
}

void draw_buffer(int x, int y, int w, int h)
{
	XFillRectangle (dpy, d, gc_out, x, y, w, h);
	XFlush(dpy);
}

void draw_image(IMG *img, int x, int y, int w, int h)
{
	XPutImage(dpy, pixmap, gc, img->screen, 0, 0, x, y, w, h);
}

void put_pixel(IMG *img, int x, int y, unsigned long c)
{
	if (x<0 || y<0) return;
	if (x>=img->w || y>=img->h) return;
	XPutPixel(img->screen, x, y, c);
}

void put_apixel(IMG *img, int x, int y, unsigned long fg, float alpha)
{
	unsigned int fR, fG, fB, bR, bG, bB;
	unsigned long bg, c;
	float v = 1 - alpha;

	if (x<0 || y<0) return;
	if (x>=img->w || y>=img->h) return;

	bg = XGetPixel(img->screen, x, y);

	bR = bg >> 16;
	bG = (bg & 0x00ff00) >> 8;
	bB = bg & 0x0000ff;

	fR = fg >> 16;
	fG = (fg & 0x00ff00) >> 8;
	fB = fg & 0x0000ff;

	c = ((unsigned int)(fR * alpha + bR * v) << 16) |
		 ((unsigned int)(fG * alpha + bG * v) <<  8) |
		  (unsigned int)(fB * alpha + bB * v);

	XPutPixel(img->screen, x, y, c);
}

void draw_line (IMG *img, int x0, int y0, int x1, int y1, unsigned long c)
{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	int fraction;

	if (dy < 0) { dy = -dy;  stepy = -1; } else stepy = 1;
	if (dx < 0) { dx = -dx;  stepx = -1; } else stepx = 1;

	dy <<= 1;
	dx <<= 1;

	put_pixel(img, x0, y0, c);

	if (dx > dy)
	{
		fraction = dy - (dx >> 1);
		while (x0 != x1)
		{
			if (fraction >= 0)
			{
				y0 += stepy;
				fraction -= dx;
			}

			x0 += stepx;
			fraction += dy;
			put_pixel(img, x0, y0, c);
		}
	}
	else
	{
		fraction = dx - (dy >> 1);
		while (y0 != y1)
		{
			if (fraction >= 0)
			{
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			put_pixel(img, x0, y0, c);
		}
	}
}

void set_spectrum()
{
	int i;
	for(i=0; i<0x100; i++)
	{
		color.red   = 0xff00;
		color.green = 0x100 * i;
		color.blue  = 0;
			XAllocColor (dpy, xcolors, &color);
		spectrum[i] = color.pixel;

		color.red   = 0x100 * (0xff-i);
		color.green = 0xff00;
		color.blue  = 0;
			XAllocColor (dpy, xcolors, &color);
		spectrum[i+0x100] = color.pixel;

		color.red   = 0;
		color.green = 0xff00;
		color.blue  = 0x100 * i;
			XAllocColor (dpy, xcolors, &color);
		spectrum[i+0x200] = color.pixel;

		color.red   = 0;
		color.green = 0x100 * (0xff-i);
		color.blue  = 0xff00;
			XAllocColor (dpy, xcolors, &color);
		spectrum[i+0x300] = color.pixel;
	}
}
