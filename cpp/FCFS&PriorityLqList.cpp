#include "stdio.h"
#include "conio.h"
#include "stdlib.h"

#define NAME_MAX_LENGTH 10			//进程名最大长度

#define LQLIST_MAX_LENTH 50			//队列最大长度

#define STATE_FINISHED 0			//进程完成状态
#define STATE_WAIT 1				//进程就绪状态
#define STATE_RUNNING 2				//进程运行状态

#define TIME_UNIT 1					//单位时间长度


typedef struct PCB
{
	//本来的固有数据
	char name[NAME_MAX_LENGTH];		//进程名字
	int arrivetime;					//到达时间(为-1表示空进程)
	int runtime;					//运行时长
	int priority;					//进程优先级
	//随调度变化的数据
	int nStep;						//运行进度
	int state;						//当前状态
	int waittime;					//等待时长
	int finishtime;					//完成时间		finishtime = arrivetime + waittime + runtime;
	int turnaroundtime;				//作业周转时间	turnaroundtime = finishtime - arrivetime;
	//成员函数
	void Init(bool bFCF = true);	//结点初始化
	void InfoPrinter(bool bFCFS);	//输出进程信息
	void operator=(PCB &P2);		//运算符重载
}PCB;

typedef struct SqQueue
{
	PCB *data;
	int length;

	void Init();							//初始化顺序队列
	void QueueInsert(PCB P,bool bFCFS);		//按某种方式插入顺序队列
}SqQueue;

void QueueInfoPrinter(SqQueue Q,bool bFCFS);										//按照某种方式输出队列Q中的各个结点信息
void GetPCB(SqQueue &runningP,SqQueue &Q);											//从就绪队列中获得一个进程
void Run(SqQueue &runningP);														//运行当前进程(发生在一个单位时间间隔)
void ReFresh(SqQueue &runningP,SqQueue &readyQ,SqQueue &finishedQ,int nTime);		//从就绪队列中取进程,并将完成的进程放入完成队列中(发生在一个单位时间间隔点)
void WaitingReFresh(SqQueue &Q);													//将就绪队列中的每个进程的等待时间加一个单位时间(发生在一个单位时间间隔)
float AverageTurnAroundTime(SqQueue finishedQ,bool bWeights = false);				//统计作业平均周转时间和带权平均周转时间
void FreeQueue(SqQueue &Q);															//释放一个队列
void ReSort(SqQueue &Q,bool bFCFS);													//就绪队列重排序

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
		printf("空.\n");
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
	int nTime = 0;			//计时
	bool bFCFS = true;		//是否为先来先服务方式
	PCB tmp;				//保存临时待插入的进程
	char ch1,ch2;			//获得用户指令
	float fAvg,fAvgW;		//作业的平均周转时间和带权平均周转时间

	SqQueue readyQ;				//就绪队列
	SqQueue finishedQ;			//已经完成的进程
	readyQ.Init();
	finishedQ.Init();
	PCB runningP;				//正在运行的进程
	runningP.arrivetime = -1;	//arrivetime等于-1表示进程为空
		
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
		if(runningP.arrivetime != -1)
		{
			printf("当前正在运行进程%s,共需运行%d个单位时间,运行到第%d个单位时间...\n\n",runningP.name,runningP.runtime,runningP.nStep);
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
			tmp.Init(bFCFS);
			tmp.arrivetime = nTime;
			printf("进程到达时间为当前时间: %d.\n",nTime);
			readyQ.QueueInsert(tmp,bFCFS);
			printf("插入成功！\n");
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '3':
			printf("就绪队列中的进程:\n\n");
			QueueInfoPrinter(readyQ,bFCFS);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '4':
			printf("已经完成的进程:\n\n");
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
			ch1 = getch();
			switch(ch1)
			{
			case '1':
				printf("已将就绪队列重排,将采用高优先级调度方式！\n");
				bFCFS = false;
				break;
			default:
				bFCFS = true;
				printf("已将就绪队列重排,将采用先来先服务调度方式！\n");
				break;
			}
			ReSort(readyQ,bFCFS);
			printf("\n按任意键继续...\n");
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