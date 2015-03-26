#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int __fbfd;
struct fb_var_screeninfo __vinfo;
struct fb_fix_screeninfo __finfo;
int __screensize;
unsigned char* __fbp = NULL;
    
void graphics_init()
{
	__fbfd = open("/dev/fb0", O_RDWR);
	ioctl(__fbfd, FBIOGET_FSCREENINFO, &__finfo);
	ioctl(__fbfd, FBIOGET_VSCREENINFO, &__vinfo);
	printf("Framebuffer opened:\n\t%dx%d, %dbpp\n", __vinfo.xres, __vinfo.yres, __vinfo.bits_per_pixel);
	__screensize = __vinfo.yres * __finfo.line_length;
	__fbp = (char*)mmap(0, __screensize, PROT_READ | PROT_WRITE, MAP_SHARED, __fbfd, 0);
	memset(__fbp,0,__screensize);
}

void graphics_clear()
{
	memset(__fbp,0,__screensize);
}

#define graphics_pixel(x,y,bit) \
if (bit) \
	__fbp[(x >> 3) + (y) * __finfo.line_length] |= 1 << (x & 7); \
else \
	__fbp[(x >> 3) + (y) * __finfo.line_length >> 3] ^= 1 << (x & 7);

void graphics_circle(int x,int y,int r)
{
	int a,b;
	int minx = x-r;
	int miny = y-r;
	int maxx = x+r;
	int maxy = y+r;
	if (minx >= (int)__vinfo.xres)
		return;
	if (miny >= (int)__vinfo.yres)
		return;
	if (maxx < 0)
		return;
	if (maxy < 0)
		return;
	if (minx < 0)
		minx = 0;
	if (miny < 0)
		miny = 0;
	if (maxx >= __vinfo.xres)
		maxx = __vinfo.xres-1;
	if (maxy >= __vinfo.yres)
		maxy = __vinfo.yres-1;

	for (a = minx; a <= maxx; a++)
		for (b = miny; b <= maxy; b++)
		{
			if ((a-x)*(a-x) + (b-y)*(b-y) > r*r)
				continue;
			graphics_pixel(a,b,1);
		}
}

void graphics_line(int x0, int y0, int x1, int y1)
{
	int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2;

	for(;;)
	{
		if (x0 >= 0 && x0 < (int)__vinfo.xres && y0 >= 0 && y0 < (int)__vinfo.yres)
			graphics_pixel(x0,y0,1);
		if (x0==x1 && y0==y1) break;
		e2 = 2*err;
		if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

void graphics_quit()
{
	munmap(__fbp, __screensize);
    close(__fbfd);
}
