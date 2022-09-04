/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : main.h
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include <pthread.h>

typedef struct{
    pthread_t pid_observer, pid_display, pid_event_loop;

    int *space_i, *space_o;
    unsigned int space_w, space_h, space_d, size_plane, size_space;
    int dw, dh;

    int a_sync, motion, mx, my;
    float ax, ay, c_ax, c_ay;

    int main, mapping, display;
    unsigned int iter;
}SYS_TABLE;

