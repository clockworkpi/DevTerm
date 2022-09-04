/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : observer.c
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include "main.h"
#include "observer.h"

IMG img;
VERTEX coord[8], center;

void create_window(SYS_TABLE *sys)
{
	XInitThreads();
	create_x (sys->dw, sys->dh, "Volume VIZ");
	create_image (&img, img.w, img.h);

	set_colormap();
	set_graymap();
	set_spectrum();

	clear_buffer(0, 0, sys->dw, sys->dh);
}

void set_env(SYS_TABLE *sys)
{
	sys->dw = 640;
	sys->dh = 480;
	sys->main = 1;

	sys->ax = 1.2;
	sys->ay = -2.7;
	sys->c_ax = sys->ax;
	sys->c_ay = sys->ay;

	sys->a_sync = 0;
	sys->motion = 1;
	
	img.w = sys->dw;
	img.h = sys->dh;

	img.cx = img.w / 2;
	img.cy = img.h / 2;

	center.x = sys->space_w / 2;
	center.y = sys->space_h / 2;
	center.z = sys->space_d / 2;

	coord[0].x = 0;
	coord[0].y = 0;
	coord[0].z = 0;
	coord[1].x = sys->space_w;
	coord[1].y = 0;
	coord[1].z = 0;
	coord[2].x = 0;
	coord[2].y = sys->space_h;
	coord[2].z = 0;
	coord[3].x = sys->space_w;
	coord[3].y = sys->space_h;
	coord[3].z = 0;
	coord[4].x = sys->space_w;
	coord[4].y = 0;
	coord[4].z = sys->space_d; 
	coord[5].x = 0;
	coord[5].y = sys->space_h;
	coord[5].z = sys->space_d;
	coord[6].x = 0;
	coord[6].y = 0;
	coord[6].z = sys->space_d;
	coord[7].x = sys->space_w;
	coord[7].y = sys->space_h;
	coord[7].z = sys->space_d;
}

int event_loop(SYS_TABLE *sys)
{
	while(XPending(dpy) || sys->main)
	{
		XNextEvent(dpy, &e);
		switch(e.type)
		{
			case Expose:
				if (!sys->display) draw_buffer (0, 0, sys->dw, sys->dh);
			break;

			case ClientMessage:
				if (e.xclient.data.l[0] == delWin) sys->main = 0;
			break;

			case KeyPress:
				if(XLookupKeysym(&e.xkey, 0) == XK_Escape) sys->main = 0;
				if(XLookupKeysym(&e.xkey, 0) == XK_q) sys->main = 0;
				if(XLookupKeysym(&e.xkey, 0) == XK_0)
				{
					sys->c_ax = 0; sys->c_ay = 0;
					sys->a_sync = 1;
					sys->display = 1;
					sys->mapping = 1;
				}
				if(XLookupKeysym(&e.xkey, 0) == XK_Left)
				{
					sys->c_ay += .05;
					sys->a_sync = 1;
					sys->display = 1;
					sys->mapping = 1;
				}
				if(XLookupKeysym(&e.xkey, 0) == XK_Right)
				{
					sys->c_ay -= .05;
					sys->a_sync = 1;
					sys->display = 1;
					sys->mapping = 1;
				}
				if(XLookupKeysym(&e.xkey, 0) == XK_Up)
				{
					sys->c_ax -= .05;
					sys->a_sync = 1;
					sys->display = 1;
					sys->mapping = 1;
				}
				if(XLookupKeysym(&e.xkey, 0) == XK_Down)
				{
					sys->c_ax += .05;
					sys->a_sync = 1;
					sys->display = 1;
					sys->mapping = 1;
				}
			break;

			case MotionNotify:
				sys->display = 1;
				sys->a_sync = 0;
				if (sys->motion)
				{
					sys->mx = e.xmotion.x;
					sys->my = e.xmotion.y;
					sys->motion = 0;
				}
				sys->c_ay += (sys->mx - e.xmotion.x)*.01;
				sys->c_ax += (sys->my - e.xmotion.y)*.01;
				sys->mx = e.xmotion.x;
				sys->my = e.xmotion.y;
			break;

			case ButtonRelease:
				sys->a_sync = 1;
				sys->motion = 1;
				sys->mapping = 1;
			break;

			default:
			break;
		}
		usleep(1200);
	}
	pthread_exit(NULL);
	return 0;
}

void draw_status(SYS_TABLE *sys)
{
	char string[64];

	XSetForeground(dpy, gc, gray[200]);

	sprintf(string, "Space size: %d*%d*%d",
		sys->space_w, sys->space_h, sys->space_d);
	XDrawString (dpy, pixmap, gc, 5, 15, string, strlen(string));

	sprintf(string, "Iterations: %d", sys->iter);
	XDrawString (dpy, pixmap, gc, 5, 30, string, strlen(string));
}

void draw_coord(SYS_TABLE *sys)
{
	int i;
	VERTEX out[8];

	XSetForeground(dpy, gc, colors[255]);

	for (i=0; i<8; i++)
	{
		rotate_xy (&coord[i], &out[i], sys->c_ax, sys->c_ay, &center);

		out[i].x += img.cx;
		out[i].y += img.cy;

		XDrawLine
			(dpy, pixmap, gc, out[i].x-3, out[i].y-3, out[i].x+3, out[i].y+3);
		XDrawLine
			(dpy, pixmap, gc, out[i].x+3, out[i].y-3, out[i].x-3, out[i].y+3);
	}

		XDrawLine (dpy, pixmap, gc, out[0].x, out[0].y, out[1].x, out[1].y);
		XDrawLine (dpy, pixmap, gc, out[0].x, out[0].y, out[2].x, out[2].y);
		XDrawLine (dpy, pixmap, gc, out[0].x, out[0].y, out[6].x, out[6].y);
}

void display(SYS_TABLE *sys)
{
	while(sys->main)
	{
		if (sys->display)
		{
			draw_image (&img, 0, 0, sys->dw, sys->dh);

			draw_status (sys);
			draw_coord (sys);

			draw_buffer (0, 0, sys->dw, sys->dh);
			sys->display = 0;
		}
		usleep(1000);
	}
	pthread_exit(NULL);
}

void mapping(SYS_TABLE *sys)
{
	int x, y, z;
	VERTEX in, out;
	float alpha;

	clear_image (&img);

	if (sys->a_sync)
	{
		sys->ax = sys->c_ax;
		sys->ay = sys->c_ay;
		sys->a_sync = 0;
	}

	for(z=0; z<sys->space_d; z++)
	{
		for(y=0; y<sys->space_h; y++)
		{
			for(x=0; x<sys->space_w; x++)
			{
				in.x = x;
				in.y = y;
				in.z = z;

				rotate_xy (&in, &out, sys->ax, sys->ay, &center);

				//put_pixel (&img, out.x + img.cx, out.y + img.cy,
				//	colors[cell_get(sys, x, y, z)]);

				alpha = cell_get(sys, x, y, z) / 4080.0;
				put_apixel (&img, out.x + img.cx, out.y + img.cy,
					colors[cell_get(sys, x, y, z)], alpha);
			}
		}
		if (!(z%4)) sys->display = 1;
		if (!sys->main) return;

		usleep(120);

		//if (!(z%2)) burn_image (&img);
	}
	sys->display = 1;
}

void observer(SYS_TABLE *sys)
{
	while(sys->main)
	{
		if (sys->mapping)
		{
			mapping (sys);
			sys->mapping = 0;
		}
		usleep(1000);
	}
	pthread_exit(NULL);
}

void create_observer(SYS_TABLE *sys)
{
	set_env (sys);
	create_window (sys);

	if(pthread_create(&(sys->pid_observer),   NULL, (void *)observer,   sys)|
		pthread_create(&(sys->pid_display),    NULL, (void *)display,    sys)|
		pthread_create(&(sys->pid_event_loop), NULL, (void *)event_loop, sys))
	{
		perror("pthread_create()");
		exit(1);
	}
}

