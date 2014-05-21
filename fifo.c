#include"bootpack.h"

#define FLAGS_OVERRUN    0x0001

void fifo32_init(struct FIFO32 *fifo,int size,int *buf,struct TASK *task) /*��ʼ��FIFO������*/
{
	fifo->size=size;
	fifo->buf=buf;
	fifo->free=size;  /*��������С*/
	fifo->p=0;  /*��һ������д��λ��*/
	fifo->q=0;  /*��һ�����ݶ���λ��*/
	fifo->flags=0;
	fifo->task=task;
	
	return;
}

int fifo32_put(struct FIFO32 *fifo,int data) /*��FIFO�������洢����*/
{
	if(fifo->free==0){
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	
	fifo->buf[fifo->p]=data;
	fifo->p++;
	if(fifo->p==fifo->size){
		fifo->p=0;
	}
	fifo->free--;
	
	/*�����ݴ���ʱ���������ߵ�����*/
	if(fifo->task!=0){
		if(fifo->task->flags!=2){
			task_run(fifo->task,-1,0);
		}
	}

	return 0;
}

int fifo32_get(struct FIFO32 *fifo) /*��FIFO������ȡ������*/
{
	int data;
	
	if(fifo->free==fifo->size){
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	
	data=fifo->buf[fifo->q];
	fifo->q++;
	if(fifo->q==fifo->size){
		fifo->q=0;
	}
	fifo->free++;
	
	return data;
}

int fifo32_status(struct FIFO32 *fifo) /*�����ܶ�������*/
{
	return fifo->size - fifo->free;
}
