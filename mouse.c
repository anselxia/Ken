#include"bootpack.h"

struct FIFO32 *mousefifo;
int mousedata0;

void inthandler2c(int *esp)	/*�����жϴ�������ϰ벿��*/
{
	unsigned char data;
	io_out8(PIC1_OCW2,0x64);	/*֪ͨPIC1"IRQ-12"���ж��ź����յ�*/
	io_out8(PIC0_OCW2,0x62);	/*֪ͨPIC0��IRQ-02�����ж��ź����յ�*/
	data=io_in8(PORT_KEYDAT);
	fifo32_put(mousefifo,data+mousedata0);
	
	return;
}

#define KEYCMD_SENDTO_MOUSE	0xd4 /*��ָ��͸����̿��Ƶ�·�˿�PORT_KEYCMD����һ�����ݾ��Զ����͸����*/
#define MOUSECMD_ENABLE		0xf4 /*�������ָ��*/

void enable_mouse(struct FIFO32 *fifo,int data0,struct MOUSE_DEC *mdec) /*�������*/
{
	/*��FIFO����������Ϣ���浽ȫ�ֱ���*/
	mousefifo=fifo;
	mousedata0=data0;
	/*�����Ч*/
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT,MOUSECMD_ENABLE);
	
	mdec->phase=0;
	
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat) /*�����*/
{
	if(mdec->phase==0){
		/*��0�׶Σ���걻����󣬻᷵��0xfa�����յ�������ݱ�������ѱ�������Դ��н�������*/
		if(dat==0xfa){
			mdec->phase=1;
		}
		return 0;
	}
	if(mdec->phase==1){
		/*�ȴ�����һ�ֽڵĽ׶�*/
		if((dat&0xc8)==0x08){	/*ȷ����һ�׶��յ��������Ƿ���Ч����P149*/
			mdec->buf[0]=dat;
			mdec->phase=2;
		}
		return 0;
	}
	if(mdec->phase==2){
		/*�ȴ����ڶ��ֽڵĽ׶Σ������������ƶ��йأ�*/
		mdec->buf[1]=dat;
		mdec->phase=3;
		return 0;
	}
	if(mdec->phase==3){
		/*�ȴ��������ֽڵĽ׶Σ������������ƶ��йأ�*/
		mdec->buf[2]=dat;
		mdec->phase=1;
		
		/*������յ�������*/
		mdec->btn=mdec->buf[0]&0x07;	/*��갴����״̬����buf[0]�ĵ�3λ*/
		mdec->x=mdec->buf[1];			/*�����ƶ�������*/
		mdec->y=mdec->buf[2];			/*�����ƶ�������*/
		
		/*��x��y�ĵ�8λ����8λ�Ժ�ȫ����Ϊ1����Ϊ0������ȷ���x��y*/
		if((mdec->buf[0]&0x10)!=0){
			mdec->x |= 0xffffff00;
		}
		if((mdec->buf[0]&0x20)!=0){
			mdec->y |= 0xffffff00;
		}
		
		mdec->y=-mdec->y;	/*���y�ķ����뻭������෴*/
		
		return 1;
	}
	return -1;
}
