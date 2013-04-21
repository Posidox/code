#include "stdio.h"
#include "conio.h"
#include "stdlib.h"

#define NAME_MAX_LENGTH 10			//��������󳤶�

#define STATE_FINISHED 0			//�������״̬
#define STATE_WAIT 1				//���̾���״̬
#define STATE_RUNNING 2				//��������״̬

#define TIME_UNIT 1					//��λʱ�䳤��


typedef struct PCB
{
	//�����Ĺ�������
	char name[NAME_MAX_LENGTH];		//��������
	int arrivetime;					//����ʱ��
	int runtime;					//����ʱ��
	int priority;					//�������ȼ�
	//����ȱ仯������
	int nStep;						//���н���
	int state;						//��ǰ״̬
	int waittime;					//�ȴ�ʱ��
	int finishtime;					//���ʱ��		finishtime = arrivetime + waittime + runtime;
	int turnaroundtime;				//��ҵ��תʱ��	turnaroundtime = finishtime - arrivetime;
	struct PCB *link;				//ָ����һ����ҵ
	//��Ա����
	void Init(bool bFCF = true);	//����ʼ��
	void InfoPrinter(bool bFCFS);	//���������Ϣ

}PCB,*PCBQueue;

void QueueInfoPrinter(PCBQueue Q,bool bFCFS);										//����ĳ�ַ�ʽ�������Q�еĸ��������Ϣ
void QueueInsert(PCBQueue &Q,PCB *P,bool bFCFS);									//��ĳ�ַ�ʽ�����������
void GetPCB(PCBQueue &runningP,PCBQueue &Q);										//�Ӿ��������л��һ������
void Run(PCBQueue &runningP);														//���е�ǰ����
void ReFresh(PCBQueue &runningP,PCBQueue &readyQ,PCBQueue &finishedQ,int nTime);	//�Ӿ���������ȡ����,������ɵĽ��̷�����ɶ�����
void WaitingReFresh(PCBQueue &Q);													//�����������е�ÿ�����̵ĵȴ�ʱ���һ����λʱ��
float AverageTurnAroundTime(PCBQueue finishedQ,bool bWeights = false);				//ͳ����ҵƽ����תʱ��ʹ�Ȩƽ����תʱ��
void FreeQueue(PCBQueue &Q);														//�ͷ�һ������

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
	link = NULL;
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

void QueueInfoPrinter(PCBQueue Q,bool bFCFS)
{
	if(NULL == Q || NULL == Q->link)
	{
		printf("��.\n");
		return;
	}
	PCB *p = Q->link;
	while(p)
	{
		p->InfoPrinter(bFCFS);
		p = p->link;
	}
}

void QueueInsert(PCBQueue &Q,PCB *P,bool bFCFS)
{
	if(NULL == Q)
	{
		Q = new PCB;
		Q->link = NULL;
	}
	PCB *tmp = Q;
	if(bFCFS)
	{
		while(tmp->link && (tmp->link->arrivetime <= P->arrivetime))
		{
			tmp = tmp->link;
		}
	}
	else
	{
		while(tmp->link && (tmp->link->priority >= P->priority))
		{
			tmp = tmp->link;
		}
	}

	P->link = tmp->link;
	tmp->link = P;
}

void GetPCB(PCBQueue &runningP,PCBQueue &Q)
{
	if(NULL == Q || NULL == Q->link)
		return;
	runningP = Q->link;
	Q->link = Q->link->link;
}

void Run(PCBQueue &runningP)
{
	if(NULL == runningP)
		return;
	runningP->nStep++;
}

void ReFresh(PCBQueue &runningP,PCBQueue &readyQ,PCBQueue &finishedQ,int nTime)
{
	if(NULL == runningP)
	{
		GetPCB(runningP,readyQ);
		if(runningP)
			runningP->state = STATE_RUNNING;
	}
	else
	{
		if(runningP->nStep == runningP->runtime)
		{
			runningP->finishtime = nTime;
			runningP->turnaroundtime = runningP->finishtime - runningP->arrivetime;
			runningP->state = STATE_FINISHED;
			QueueInsert(finishedQ,runningP,true);
			runningP = NULL;
			GetPCB(runningP,readyQ);
			if(runningP)
				runningP->state = STATE_RUNNING;
		}
	}
}

void WaitingReFresh(PCBQueue &Q)
{
	if(NULL == Q)	return;
	PCB *p = Q->link;
	while(p)
	{
		p->waittime += TIME_UNIT;
		p = p->link;
	}
}

float AverageTurnAroundTime(PCBQueue finishedQ,bool bWeights)
{
	if(NULL == finishedQ)
		return -1;
	PCB *p = finishedQ;
	float fCount = 0;
	float fSum = 0;
	while(p->link)
	{
		if(bWeights)
		{
			fSum += (float)(p->link->turnaroundtime) / (float)(p->link->runtime);
		}
		else
		{
			fSum += (float)(p->link->turnaroundtime);
		}
		fCount++;
		p = p->link;
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

void FreeQueue(PCBQueue &Q)
{
	PCBQueue tmp;
	tmp = Q;
	while(tmp!=NULL)
	{
		Q = Q->link;
		delete tmp;
		tmp = Q;
	}
}

int main()
{
	int nTime = 0;			//��ʱ
	bool bFCFS = true;		//�Ƿ�Ϊ�����ȷ���ʽ
	PCB *tmp = NULL;		//������ʱ������Ľ���
	char ch1,ch2;			//����û�ָ��
	float fAvg,fAvgW;		//��ҵ��ƽ����תʱ��ʹ�Ȩƽ����תʱ��

	PCBQueue readyQ = NULL;		//��������
	PCBQueue finishedQ = NULL;	//�Ѿ���ɵĽ���
	PCBQueue runningP = NULL;	//�������еĽ���

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
		if(runningP)
		{
			printf("��ǰ�������н���%s,��������%d����λʱ��,���е���%d����λʱ��...\n\n",runningP->name,runningP->runtime,runningP->nStep);
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
			tmp = new PCB;
			tmp->Init(bFCFS);
			tmp->arrivetime = nTime;
			printf("���̵���ʱ��Ϊ��ǰʱ��: %d.\n",nTime);
			QueueInsert(readyQ,tmp,bFCFS);
			printf("����ɹ���\n");
			printf("\n�����������...\n");
			getch();
			break;
		case '3':
			printf("���������еĽ���:\n");
			QueueInfoPrinter(readyQ,bFCFS);
			printf("\n�����������...\n");
			getch();
			break;
		case '4':
			printf("�Ѿ���ɵĽ���:\n");
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
			FreeQueue(readyQ);
			ch1 = getch();
			switch(ch1)
			{
			case '1':
				printf("����վ�������,�����ø����ȼ����ȷ�ʽ��\n");
				bFCFS = false;
				break;
			default:
				bFCFS = true;
				printf("����վ�������,�����������ȷ�����ȷ�ʽ��\n");
				break;
			}
			printf("\n�����������...\n");
			getch();
			break;
		case '6':
			FreeQueue(readyQ);
			FreeQueue(finishedQ);
			FreeQueue(runningP);
			return 0;
			break;
		default:
			break;
		}
	}
}