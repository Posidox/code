#include "stdio.h"
#include "conio.h"
#include "stdlib.h"

#define NAME_MAX_LENGTH 10			//进程名最大长度

#define STATE_FINISHED 0			//进程完成状态
#define STATE_WAIT 1				//进程就绪状态
#define STATE_RUNNING 2				//进程运行状态

#define TIME_UNIT 1					//单位时间长度


typedef struct PCB
{
	//本来的固有数据
	char name[NAME_MAX_LENGTH];		//进程名字
	int arrivetime;					//到达时间
	int runtime;					//运行时长
	int priority;					//进程优先级
	//随调度变化的数据
	int nStep;						//运行进度
	int state;						//当前状态
	int waittime;					//等待时长
	int finishtime;					//完成时间		finishtime = arrivetime + waittime + runtime;
	int turnaroundtime;				//作业周转时间	turnaroundtime = finishtime - arrivetime;
	struct PCB *link;				//指向下一个作业
	//成员函数
	void Init(bool bFCF = true);	//结点初始化
	void InfoPrinter(bool bFCFS);	//输出进程信息

}PCB,*PCBQueue;

void QueueInfoPrinter(PCBQueue Q,bool bFCFS);										//按照某种方式输出队列Q中的各个结点信息
void QueueInsert(PCBQueue &Q,PCB *P,bool bFCFS);									//按某种方式插入就绪队列
void GetPCB(PCBQueue &runningP,PCBQueue &Q);										//从就绪队列中获得一个进程
void Run(PCBQueue &runningP);														//运行当前进程
void ReFresh(PCBQueue &runningP,PCBQueue &readyQ,PCBQueue &finishedQ,int nTime);	//从就绪队列中取进程,并将完成的进程放入完成队列中
void WaitingReFresh(PCBQueue &Q);													//将就绪队列中的每个进程的等待时间加一个单位时间
float AverageTurnAroundTime(PCBQueue finishedQ,bool bWeights = false);				//统计作业平均周转时间和带权平均周转时间
void FreeQueue(PCBQueue &Q);														//释放一个队列

void PCB::Init(bool bFCFS)
{
	printf("请输入进程名:");
	scanf("%s",name);
	if(!bFCFS)
	{
		printf("请输入进程优先级:");
		scanf(" %d",&priority);
	}
	else
	{
		priority = 0;
	}
	printf("请输入运行时长:");
	scanf(" %d",&runtime);
	while(0 == runtime)
	{
		printf("运行时长不能为0,请重新输入:");
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
		printf("空.\n");
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
	printf("进程名:%s,",name);
	printf("到达时间:%d,",arrivetime);
	if(!bFCFS)
	{
		printf("优先级:%d,",priority);
	}
	printf("运行时长:%d;\n当前状态:%s,等待时长:%d",runtime,a,waittime);
	if(-1 != finishtime)
		printf(",完成时间:%d",finishtime);
	if(-1 != turnaroundtime)
		printf(",作业周转时间:%d",turnaroundtime);
	printf(".\n");
}

void QueueInfoPrinter(PCBQueue Q,bool bFCFS)
{
	if(NULL == Q || NULL == Q->link)
	{
		printf("空.\n");
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
	int nTime = 0;			//计时
	bool bFCFS = true;		//是否为先来先服务方式
	PCB *tmp = NULL;		//保存临时待插入的进程
	char ch1,ch2;			//获得用户指令
	float fAvg,fAvgW;		//作业的平均周转时间和带权平均周转时间

	PCBQueue readyQ = NULL;		//就绪队列
	PCBQueue finishedQ = NULL;	//已经完成的进程
	PCBQueue runningP = NULL;	//正在运行的进程

	while(1)
	{
		system("cls");
		printf("**************当前时间:第%d个单位时间******",nTime);
		if(bFCFS)
		{
			printf("先来先服务调度");
		}
		else
		{
			printf("最高优先级调度");
		}
		printf("************\n\n");
		if(runningP)
		{
			printf("当前正在运行进程%s,共需运行%d个单位时间,运行到第%d个单位时间...\n\n",runningP->name,runningP->runtime,runningP->nStep);
		}
		else
		{
			printf("当前没有正在运行的进程.\n\n");
		}
		printf("1.继续运行一个单位时间;\n");
		printf("2.插入新进程;\n");
		printf("3.显示当前就绪的进程队列;\n");
		printf("4.显示当前已完成的进程;\n");		
		printf("5.修改调度方式;\n");
		printf("6.退出.\n\n");

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
			printf("进程到达时间为当前时间: %d.\n",nTime);
			QueueInsert(readyQ,tmp,bFCFS);
			printf("插入成功！\n");
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '3':
			printf("就绪队列中的进程:\n");
			QueueInfoPrinter(readyQ,bFCFS);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '4':
			printf("已经完成的进程:\n");
			QueueInfoPrinter(finishedQ,bFCFS);
			fAvg = AverageTurnAroundTime(finishedQ);
			fAvgW = AverageTurnAroundTime(finishedQ,true);
			if(fAvg > -1)
			{
				printf("\n以上进程作业的平均周转时间为%f个单位时间.\n",fAvg);
				printf("\n以上进程作业的带权平均周转时间为%f个单位时间.\n",fAvgW);
			}
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '5':
			printf("按数字1采用高优先级调度,按其他用先来先服务调度:\n");
			FreeQueue(readyQ);
			ch1 = getch();
			switch(ch1)
			{
			case '1':
				printf("已清空就绪队列,将采用高优先级调度方式！\n");
				bFCFS = false;
				break;
			default:
				bFCFS = true;
				printf("已清空就绪队列,将采用先来先服务调度方式！\n");
				break;
			}
			printf("\n按任意键继续...\n");
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