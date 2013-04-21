#include "stdio.h"
#include "conio.h"
#include "stdlib.h"

#define NAME_MAX_LENGTH 10			//��������󳤶�

#define LQLIST_MAX_LENTH 50			//������󳤶�

#define STATE_FINISHED 0			//�������״̬
#define STATE_WAIT 1				//���̾���״̬
#define STATE_RUNNING 2				//��������״̬

#define TIME_UNIT 1					//��λʱ�䳤��


typedef struct PCB
{
	//�����Ĺ�������
	char name[NAME_MAX_LENGTH];		//��������
	int arrivetime;					//����ʱ��(Ϊ-1��ʾ�ս���)
	int runtime;					//����ʱ��
	int priority;					//�������ȼ�
	//����ȱ仯������
	int nStep;						//���н���
	int state;						//��ǰ״̬
	int waittime;					//�ȴ�ʱ��
	int finishtime;					//���ʱ��		finishtime = arrivetime + waittime + runtime;
	int turnaroundtime;				//��ҵ��תʱ��	turnaroundtime = finishtime - arrivetime;
	//��Ա����
	void Init(bool bFCF = true);	//����ʼ��
	void InfoPrinter(bool bFCFS);	//���������Ϣ
	void operator=(PCB &P2);		//���������
}PCB;

typedef struct SqQueue
{
	PCB *data;
	int length;

	void Init();							//��ʼ��˳�����
	void QueueInsert(PCB P,bool bFCFS);		//��ĳ�ַ�ʽ����˳�����
}SqQueue;

void QueueInfoPrinter(SqQueue Q,bool bFCFS);										//����ĳ�ַ�ʽ�������Q�еĸ��������Ϣ
void GetPCB(SqQueue &runningP,SqQueue &Q);											//�Ӿ��������л��һ������
void Run(SqQueue &runningP);														//���е�ǰ����(������һ����λʱ����)
void ReFresh(SqQueue &runningP,SqQueue &readyQ,SqQueue &finishedQ,int nTime);		//�Ӿ���������ȡ����,������ɵĽ��̷�����ɶ�����(������һ����λʱ������)
void WaitingReFresh(SqQueue &Q);													//�����������е�ÿ�����̵ĵȴ�ʱ���һ����λʱ��(������һ����λʱ����)
float AverageTurnAroundTime(SqQueue finishedQ,bool bWeights = false);				//ͳ����ҵƽ����תʱ��ʹ�Ȩƽ����תʱ��
void FreeQueue(SqQueue &Q);															//�ͷ�һ������
void ReSort(SqQueue &Q,bool bFCFS);													//��������������

void PCB::Init(bool bFCFS)
{
	printf("�����������:");
	scanf("%s",name);
	if(!bFCFS)
	{
		printf("������������ȼ�:");
		scanf(" %d",&priority);
	}
	else
	{
		priority = 0;
	}
	printf("����������ʱ��:");
	scanf(" %d",&runtime);
	while(0 == runtime)
	{
		printf("����ʱ������Ϊ0,����������:");
		scanf(" %d",&runtime);
	}

	state = STATE_WAIT;
	nStep = 0;
	waittime = 0;
	finishtime = -1;
	turnaroundtime = -1;
}

void PCB::InfoPrinter(bool bFCFS)
{
	if(!this)
	{
		printf("��.\n");
		return;
	}
	char *a;
	switch(state)
	{
	case STATE_FINISHED:
		a = "Finished";
		break;
	case STATE_RUNNING:
		a = "Running";
		break;
	case STATE_WAIT:
		a = "Waiting";
		break;
	default:
		a = NULL;
		break;
	}
	printf("������:%s,",name);
	printf("����ʱ��:%d,",arrivetime);
	if(!bFCFS)
	{
		printf("���ȼ�:%d,",priority);
	}
	printf("����ʱ��:%d;\n��ǰ״̬:%s,�ȴ�ʱ��:%d",runtime,a,waittime);
	if(-1 != finishtime)
		printf(",���ʱ��:%d",finishtime);
	if(-1 != turnaroundtime)
		printf(",��ҵ��תʱ��:%d",turnaroundtime);
	printf(".\n");
}

void PCB::operator=(PCB &P2)
{
	arrivetime = P2.arrivetime;
	finishtime = P2.finishtime;
	for(int i=0;i<NAME_MAX_LENGTH;i++)
	{
		name[i] = P2.name[i];
	}
	nStep = P2.nStep;
	priority = P2.priority;
	runtime = P2.runtime;
	state = P2.state;
	turnaroundtime = P2.turnaroundtime;
	waittime = P2.waittime;
}

void SqQueue::Init()
{
	data = new PCB[LQLIST_MAX_LENTH];
	length = 0;
}

void QueueInfoPrinter(SqQueue Q,bool bFCFS)
{
	if(Q.length == 0 || NULL == Q.data)
	{
		printf("��.\n");
		return;
	}
	for(int i=0;i<Q.length;i++)
	{
		Q.data[i].InfoPrinter(bFCFS);
	}
}

void SqQueue::QueueInsert(PCB P,bool bFCFS)
{
	if(data == NULL)
		Init();
	int i;
	if(bFCFS)
	{
		data[length] = P;
		length++;
	}
	else
	{
		for(i=0;i<length;i++)
		{
			if(data[i].priority < P.priority)
				break;
		}
		length++;
		for(int j=length;j>i;j--)
		{
			data[j] = data[j-1];
		}
		data[i] = P;
	}	
}

void GetPCB(PCB &runningP,SqQueue &Q)
{
	if(Q.data == NULL || Q.length == 0)
		return;
	runningP = Q.data[0];
	for(int i=0;i<Q.length-1;i++)
	{
		Q.data[i] = Q.data[i+1];
	}
	Q.length--;
}

void Run(PCB &runningP)
{
	if(-1 == runningP.arrivetime)
		return;
	runningP.nStep++;
}

void ReFresh(PCB &runningP,SqQueue &readyQ,SqQueue &finishedQ,int nTime)
{
	if(-1 == runningP.arrivetime)
	{
		GetPCB(runningP,readyQ);
		if(-1 != runningP.arrivetime)
			runningP.state = STATE_RUNNING;
	}
	else
	{
		if(runningP.nStep == runningP.runtime)
		{
			runningP.finishtime = nTime;
			runningP.turnaroundtime = runningP.finishtime - runningP.arrivetime;
			runningP.state = STATE_FINISHED;
			finishedQ.QueueInsert(runningP,true);
			runningP.arrivetime = -1;
			GetPCB(runningP,readyQ);
			if(runningP.arrivetime != -1)
				runningP.state = STATE_RUNNING;
		}
	}
}

void WaitingReFresh(SqQueue &Q)
{
	if(0 == Q.length||Q.data == NULL)	return;
	for(int i=0;i<Q.length;i++)
	{
		Q.data[i].waittime += TIME_UNIT;
	}
}

float AverageTurnAroundTime(SqQueue finishedQ,bool bWeights)
{
	if(NULL == finishedQ.data||0 == finishedQ.length)
		return -1;
	float fCount = (float)finishedQ.length;
	float fSum = 0;
	for(int i=0;i<finishedQ.length;i++)
	{
		if(bWeights)
		{
			fSum += (float)finishedQ.data[i].turnaroundtime / (float)finishedQ.data[i].runtime;
		}
		else
		{
			fSum += (float)finishedQ.data[i].turnaroundtime;
		}
	}
	if(0 != fCount)
	{
		return (fSum/fCount);
	}
	else
	{
		return -1;
	}
}

void FreeQueue(SqQueue &Q)
{
	delete Q.data;
	Q.length = 0;
}

void ReSort(SqQueue &Q,bool bFCFS)
{
	PCB tmp;
	int nCount = Q.length;
	bool bFlag = true;
	int m = nCount-1;
	int i;
	while((m>0)&&(bFlag))
	{
		bFlag = false;
		for(i=0;i<m;i++)
		{
			if(bFCFS)
			{
				if(Q.data[i].arrivetime > Q.data[i+1].arrivetime)
				{
					bFlag = true;
					tmp = Q.data[i];
					Q.data[i] = Q.data[i+1];
					Q.data[i+1] = tmp;
				}
			}
			else
			{
				if(Q.data[i].priority < Q.data[i+1].priority)
				{
					bFlag = true;
					tmp = Q.data[i];
					Q.data[i] = Q.data[i+1];
					Q.data[i+1] = tmp;
				}
			}

		}
		m--;
	}
}

int main()
{
	int nTime = 0;			//��ʱ
	bool bFCFS = true;		//�Ƿ�Ϊ�����ȷ���ʽ
	PCB tmp;				//������ʱ������Ľ���
	char ch1,ch2;			//����û�ָ��
	float fAvg,fAvgW;		//��ҵ��ƽ����תʱ��ʹ�Ȩƽ����תʱ��

	SqQueue readyQ;				//��������
	SqQueue finishedQ;			//�Ѿ���ɵĽ���
	readyQ.Init();
	finishedQ.Init();
	PCB runningP;				//�������еĽ���
	runningP.arrivetime = -1;	//arrivetime����-1��ʾ����Ϊ��
		
	while(1)
	{
		system("cls");
		printf("**************��ǰʱ��:��%d����λʱ��******",nTime);
		if(bFCFS)
		{
			printf("�����ȷ������");
		}
		else
		{
			printf("������ȼ�����");
		}
		printf("************\n\n");
		if(runningP.arrivetime != -1)
		{
			printf("��ǰ�������н���%s,��������%d����λʱ��,���е���%d����λʱ��...\n\n",runningP.name,runningP.runtime,runningP.nStep);
		}
		else
		{
			printf("��ǰû���������еĽ���.\n\n");
		}
		printf("1.��������һ����λʱ��;\n");
		printf("2.�����½���;\n");
		printf("3.��ʾ��ǰ�����Ľ��̶���;\n");
		printf("4.��ʾ��ǰ����ɵĽ���;\n");		
		printf("5.�޸ĵ��ȷ�ʽ;\n");
		printf("6.�˳�.\n\n");

		ch2 = getch();
		switch (ch2)
		{
		case '1':
			ReFresh(runningP,readyQ,finishedQ,nTime);
			WaitingReFresh(readyQ);
			Run(runningP);
			nTime += TIME_UNIT;						
			break;
		case '2':
			tmp.Init(bFCFS);
			tmp.arrivetime = nTime;
			printf("���̵���ʱ��Ϊ��ǰʱ��: %d.\n",nTime);
			readyQ.QueueInsert(tmp,bFCFS);
			printf("����ɹ���\n");
			printf("\n�����������...\n");
			getch();
			break;
		case '3':
			printf("���������еĽ���:\n\n");
			QueueInfoPrinter(readyQ,bFCFS);
			printf("\n�����������...\n");
			getch();
			break;
		case '4':
			printf("�Ѿ���ɵĽ���:\n\n");
			QueueInfoPrinter(finishedQ,bFCFS);
			fAvg = AverageTurnAroundTime(finishedQ);
			fAvgW = AverageTurnAroundTime(finishedQ,true);
			if(fAvg > -1)
			{
				printf("\n���Ͻ�����ҵ��ƽ����תʱ��Ϊ%f����λʱ��.\n",fAvg);
				printf("\n���Ͻ�����ҵ�Ĵ�Ȩƽ����תʱ��Ϊ%f����λʱ��.\n",fAvgW);
			}
			printf("\n�����������...\n");
			getch();
			break;
		case '5':
			printf("������1���ø����ȼ�����,�������������ȷ������:\n");
			ch1 = getch();
			switch(ch1)
			{
			case '1':
				printf("�ѽ�������������,�����ø����ȼ����ȷ�ʽ��\n");
				bFCFS = false;
				break;
			default:
				bFCFS = true;
				printf("�ѽ�������������,�����������ȷ�����ȷ�ʽ��\n");
				break;
			}
			ReSort(readyQ,bFCFS);
			printf("\n�����������...\n");
			getch();
			break;
		case '6':
 			FreeQueue(readyQ);
 			FreeQueue(finishedQ);
			return 0;
			break;
		default:
			break;
		}
	}
}