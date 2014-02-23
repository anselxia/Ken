; haribote-ipl
; TAB=4

CYLS	EQU		10
		ORG		0x7c00		;����װ�ص�ַ
		        
		JMP		entry
		DB		0x90
		
;��׼FAT12��ʽ���̵�����

		DB		"HARIBOTE"	;������������
		DW		512			;ÿ�������Ĵ�С������512��
		DB		1			;�صĴ�С��1��������
		DW		1			;FAT����ʼλ��
		DB		2			;FAT�ĸ���������2��
		DW		224			;��Ŀ¼�Ĵ�С��һ��224�
		DW 		2880		;���̵Ĵ�С
		DB		0xf0		;���̵�����
		DW		9			;FAT�ĳ��ȣ�����9��
		DW		18			;1���ŵ��м�������������18��
		DW		2			;��ͷ��������2��
		DD		0			;��ʹ�÷���
		DD		2880		;��дһ�δ��̴�С
		DB		0,0,0x29	;��֪��������
		DD		0xffffffff	;������
		DB		"HARIBOTEOS "	;��������
		DB		"FAT12   "		;���̸�ʽ����
		RESB	18			;�ճ�18�ֽ�
		
;�������

entry:
		MOV		AX,0		;��ʼ���Ĵ���
		MOV		SS,AX
		MOV		SP,0x7c00	;ջָ��Ĵ���
		MOV		DS,AX
		
;������,��������
		MOV		AX,0x0820	;0x8000~0x81ff��������������ʣ�µĳ������0x8200��ĵط�
		MOV		ES,AX		;ES:BXΪ��������ַ������Ҫ��BX��0
		MOV		CH,0		;����0
		MOV		DH,0		;��ͷ0
		MOV		CL,2		;����2
readloop:
		MOV		SI,0		;��¼ʧ�ܴ����ļĴ���
retry:
		MOV		AH,0x02		;�������
		MOV		AL,1		;1������
		MOV		BX,0
		MOV		DL,0x00		;A������
		INT		0x13		;���ô���BIOS
		JNC		next		;��λ��־Ϊ0��û�д���ת��next
		;����Ϊ�����
		ADD		SI,1		
		CMP		SI,5
		JAE		error
		MOV		AH,0x00
		MOV		DL,0x00		;A������
		INT		0x13		;����������
		JMP		retry

next:
		MOV		AX,ES		;���ڴ��ַ����0x200���ֽڣ���Ϊû��ADD	ES,0x020ָ���΢�Ƹ���
		ADD		AX,0x0020		
		MOV		ES,AX
		ADD		CL,1		;CL��1������18����������CL<=18��ת��readloop
		CMP		CL,18
		JBE		readloop
		
		MOV		CL,1
		ADD		DH,1		;��ͷ1
		CMP		DH,2
		JB		readloop
		
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop
		
		MOV		[0x0ff0],CH		
		JMP		0xc200		;����ϵͳ�������ݴ�0xc200����ʼ
		
error:
		MOV 	SI,msg

;���Կ��йصĲ���,��������ʾ�ַ�
putloop:
		MOV		AL,[SI]		;ALΪ��Ҫ��ʾ���ַ�����,P38
		ADD		SI,1
		CMP		AL,0
		JE		fin
		
		MOV		AH,0x0e		
		MOV		BX,15
		INT		0x10
		JMP		putloop
		
fin:
		HLT
		JMP		fin
		
msg:
		DB		0x0a,0x0a
		DB		"load error"
		DB		0x0a			
		DB		0
		
		RESB	0x7dfe-$		;ȷ������������������ֽ�Ϊ55AA��P26������

		DB		0x55, 0xaa
		
		