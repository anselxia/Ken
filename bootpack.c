#include<stdio.h>
void io_hlt(void);
void io_cli(void);
void io_out8(int port,int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start,int end,unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram,int xsize,int x,int y,char c,char *font);
void putfonts8_asc(char *vram,int xsize,int x,int y,char c,unsigned char *s);
void init_mouse_cursor8(char *mouse,char bc);
void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize);

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/*该结构体与P58的BOOT_INFO数据结构相对应*/
struct BOOTINFO{	
	char cyls,leds,vomde,reserve;
	short scrnx,scrny;
	char *vram;
};

void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;	/*将0x0ff0内存附近的启动信息提取出来*/
	char s[40],mcursor[256];
	int mx,my;
	
	init_palette();	/*设定调色板*/
	init_screen(binfo->vram,binfo->scrnx,binfo->scrny);	/*显示画面背景*/
	
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	
	init_mouse_cursor8(mcursor,COL8_008484);	/*描画鼠标指针*/
	putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mcursor,16);	/*显示鼠标指针*/
	
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
				
	for(;;){
		io_hlt();
	}
}

void init_palette(void)	/*初始化调色板*/
{
	static unsigned char table_rgb[16*3]={
		0x00,0x00,0x00,
		0xff,0x00,0x00,
		0x00,0xff,0x00,
		0xff,0xff,0x00,
		0x00,0x00,0xff,
		0xff,0x00,0xff,
		0x00,0xff,0xff,
		0xff,0xff,0xff,
		0xc6,0xc6,0xc6,
		0x84,0x00,0x00,
		0x00,0x84,0x00,
		0x84,0x84,0x00,
		0x00,0x00,0x84,
		0x84,0x00,0x84,
		0x00,0x84,0x84,
		0x84,0x84,0x84
	};
	set_palette(0,15,table_rgb);
	return;
}

void set_palette(int start,int end,unsigned char *rgb)	/*设置调色板*/
{
	int i,eflags;
	eflags=io_load_eflags();
	io_cli();
	io_out8(0x03c8,start);
	
	for(i=start;i<=end;i++)
	{
		io_out8(0x03c9,rgb[0]/4);
		io_out8(0x03c9,rgb[1]/4);
		io_out8(0x03c9,rgb[2]/4);
		rgb+=3;
	}
	
	io_store_eflags(eflags);
	return;
}

void boxfill8(unsigned char *vram,int xsize,unsigned char c,int x0,int y0,int x1,int y1)	/*显示框图，显示背景画面用*/
{
	int x,y;
	for(y=y0;y<=y1;y++){
		for(x=x0;x<=x1;x++)
			vram[y*xsize+x]=c;
	}
	return;	
}

void init_screen(char *vram,int x,int y)	/*显示背景画面*/
{
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;	
}

void putfont8(char *vram,int xsize,int x,int y,char c,char *font)	/*字体为16行8列，输出某个字体font*/
{
	int i;
	char *p,d;	
	
	for(i=0;i<16;i++){
		p=vram+(y+i)*xsize+x;
		d=font[i];
		if( (d&0x80)!=0	)	p[0]=c;		
		if( (d&0x40)!=0 )	p[1]=c;		
		if( (d&0x20)!=0 )	p[2]=c;		
		if( (d&0x10)!=0 )	p[3]=c;		
		if( (d&0x08)!=0 )	p[4]=c;		
		if( (d&0x04)!=0 )	p[5]=c;	
		if( (d&0x02)!=0 )	p[6]=c;		
		if( (d&0x01)!=0 )	p[7]=c;	
	}
	return;
}

void putfonts8_asc(char *vram,int xsize,int x,int y,char c,unsigned char *s)	/*显示字符串*/
{
	extern char hankaku[4096];
	for(;*s!=0x00;s++){
		putfont8(vram,xsize,x,y,c,hankaku+*s*16);
		x+=8;
	}
	return;
}

void init_mouse_cursor8(char *mouse,char bc)	/*显示鼠标指针*/
{
	static char cursor[16][16]={	/*描画鼠标指针*/
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	
	int x,y;	/*根据所描画的鼠标指针，在对应位置填充相应的颜色，并存入*mouse中*/
	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			if(cursor[y][x]=='*'){
				mouse[y*16+x]=COL8_000000;
			}
			if(cursor[y][x]=='O'){
				mouse[y*16+x]=COL8_FFFFFF;
			}
			if(cursor[y][x]=='.'){
				mouse[y*16+x]=bc;
			}
		}
	}
	return;
}

void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize)	/*将*buf（实参为*mouse）中描画的鼠标指针显示在屏幕相应位置*/
{
	int	x,y;
	for(y=0;y<pysize;y++){
		for(x=0;x<pxsize;x++){
			vram[(py0+y)*vxsize+(px0+x)]=buf[y*bxsize+x];
		}
	}
	return;
}
		