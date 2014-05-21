#include"bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

void inthandler21(int *esp)	/*���̵��жϴ�������ϰ벿��*/
{
	unsigned char data;
	io_out8(PIC0_OCW2,0x61);	/*֪ͨPIC0"IRQ-01"���ж��ź����յ�*/
	data=io_in8(PORT_KEYDAT);	/*�Ӽ��̶�ȡ��������*/
	fifo32_put(keyfifo,data+keydata0);
	
	return;
}

#define PORT_KEYSTA					0x0064 /*���̿��Ƶ�·״̬��ȡ�˿�*/
#define KEYSTA_SEND_NOTREADY		0x02
#define KEYCMD_WRITE_MODE			0x60 /*����ģʽ�趨ָ��*/
#define KBC_MODE					0x47 /*��������趨ָ��*/

void wait_KBC_sendready(void) /*�����̿��Ƶ�·�Ƿ�׼���ý���CPUָ��*/
{
	for(;;){
		if( (io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0 ){ /*�����̿��Ƶ�·���Խ���CPUָ�0x0064���ĵ����ڶ�λΪ0*/
			break;
		}
	}
}

void init_keyboard(struct FIFO32 *fifo,int data0) /*��ʼ�����̿��Ƶ�·��ʹ����Է�������ж�*/
{
	/*��FIFO����������Ϣ���浽ȫ�ֱ���*/
	keyfifo=fifo;
	keydata0=data0;
	/*���̿������ĳ�ʼ��*/
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT,KBC_MODE);
	
	return;
}
