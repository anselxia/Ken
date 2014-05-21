#include"bootpack.h"
#include<stdio.h>

void init_pic(void)	/*��ʼ��PIC*/
{
	/*��������PIC�������ж�*/
	io_out8(PIC0_IMR,0xff);
	io_out8(PIC1_IMR,0xff);
	
	/*��ʼ��PIC0*/
	io_out8(PIC0_ICW1,0x11);	/*���ش���ģʽ*/
	io_out8(PIC0_ICW2,0x20);	/*IRQ0-7��INT20-27����*/
	io_out8(PIC0_ICW3,1<<2);	/*PIC1��IRQ2����*/
	io_out8(PIC0_ICW4,0x10);	/*�޻�����ģʽ*/
	
	/*��ʼ��PIC1*/
	io_out8(PIC1_ICW1,0x11);	/*���ش���ģʽ*/
	io_out8(PIC1_ICW2,0x28);	/*IRQ8-15��INT28-2f����*/
	io_out8(PIC1_ICW3,2	 );	/*PIC1��IRQ2����*/
	io_out8(PIC1_ICW4,0x10);	/*w�޻�����ģʽ*/
	
	/*��ֹPIC1��������ж�*/
	io_out8(PIC0_IMR,0xfb);	/*1111011*/
	io_out8(PIC1_IMR,0xff);
	
	return;
}

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67); 
	return;
}
