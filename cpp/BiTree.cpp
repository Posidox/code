#include "stdio.h"//有这个才能用printf()
#include "conio.h"//有这个才能用getch()
#include "stdlib.h"//有这个才能用system()

//二叉树二叉链表存储结构体定义
typedef struct BNode{
	char data;						//结点数据域
	struct BNode *lchild,*rchild;	//左右孩子指针
}BNode,*BiTree;


void CreateBiTreeInPreOrder(BiTree &T);// 1．以二叉链表表示二叉树，建立一棵二叉树；
void PreOrderTraverse(BiTree T);// 2．输出二叉树的前序遍历结果；
void InOrderTraverse(BiTree T);// 3．输出二叉树的中序遍历结果；
void PostOrderTraverse(BiTree T);// 4．输出二叉树的后序遍历结果；
int LeafNodeCount(BiTree T);// 5．统计二叉树的叶结点个数；
int NodeCount(BiTree T);// 6．统计二叉树的结点个数；
int Depth(BiTree T);// 7．计算二叉树的深度；
void ExchangeBiTree(BiTree &T);// 8．交换二叉树每个结点的左孩子和右孩子；

//选做
void DblOrderTraverse(BiTree &T);// 9．输出二叉树的双序遍历结果；

void visit(BiTree T);//访问结点的函数



void visit(BiTree T)
{
	printf("%2c",T->data);//访问结点即输出该结点数据
}

void CreateBiTreeInPreOrder(BiTree &T)// 1．以二叉链表表示二叉树，建立一棵二叉树；
{
	char ch;
	ch = getch();
							
	if(' '== ch)							//如果输入空格，该二叉树为空
	{
		T = NULL;
		printf(" _");					//如果输入为空格输出下划线'_'提示
	}
	else
	{
		printf("%2c",ch);				//如果输入不为空格输出数据
		T = new BNode;
		T->data = ch;						//给根节点复制
		CreateBiTreeInPreOrder(T->lchild);	//建立左子树
		CreateBiTreeInPreOrder(T->rchild);	//建立右子树
	}
}

void PreOrderTraverse(BiTree T)// 2．输出二叉树的前序遍历结果；
{
	if(NULL == T)	return;			//若二叉树为空，则空操作；否则
	visit(T);						//访问根节点
	PreOrderTraverse(T->lchild);	//先序遍历左子树
	PreOrderTraverse(T->rchild);	//先序遍历右子树
}

void InOrderTraverse(BiTree T)// 3．输出二叉树的中序遍历结果；
{
	if(NULL == T)	return;		//若二叉树为空，则空操作；否则
	InOrderTraverse(T->lchild);	//中序遍历左子树
	visit(T);					//访问根节点
	InOrderTraverse(T->rchild);	//中序遍历右子树
}

void PostOrderTraverse(BiTree T)// 4．输出二叉树的后序遍历结果；
{
	if(NULL == T)	return;			//若二叉树为空，则空操作；否则
	PostOrderTraverse(T->lchild);	//后序遍历左子树
	PostOrderTraverse(T->rchild);	//后序遍历左子树
	visit(T);						//访问根节点
}

int LeafNodeCount(BiTree T)// 5．统计二叉树的叶结点个数；
{
	if(NULL == T)	return 0;									//若二叉树为空，则返回0；否则
	if(!(T->lchild||T->rchild))	return 1;						//若左右子树均不存在，返回1；否则
	return LeafNodeCount(T->lchild)+LeafNodeCount(T->rchild);	//返回左子树叶子节点数与右子树叶子节点数之和
}

int NodeCount(BiTree T)// 6．统计二叉树的结点个数；
{
	if(NULL == T) return 0;										//若二叉树为空，则返回0；否则
	return NodeCount(T->lchild)+NodeCount(T->rchild)+1;			//返回左子树节点数与右子树节点数之和加1
}

int Depth(BiTree T)// 7．计算二叉树的深度。
{
	if(NULL == T) return 0;	//若二叉树为空，则返回0；否则
	int ld,rd;
	ld = Depth(T->lchild);
	rd = Depth(T->rchild);
	return (ld>rd?ld:rd)+1;	//返回左右子树深度较大者加1
}

void ExchangeBiTree(BiTree &T)// 8．交换二叉树每个结点的左孩子和右孩子；
{
	if(NULL == T)	return;		//若二叉树为空，则空操作；否则
	ExchangeBiTree(T->lchild);	//交换左子树的每个结点的左孩子和右孩子
	ExchangeBiTree(T->rchild);	//交换右子树的每个结点的左孩子和右孩子

	BNode *t;
	t = T->lchild;				//交换当前二叉树的左孩子和右孩子
	T->lchild = T->rchild;
	T->rchild = t;
}

void DblOrderTraverse(BiTree &T) //9．（选做）输出二叉树的双序遍历结果；
{
	if(NULL == T)	return;			//若二叉树为空，则空操作；否则返回
	visit(T);						//访问根节点
	DblOrderTraverse(T->lchild);	//双序遍历左子树
	visit(T);						//再次访问根节点
	DblOrderTraverse(T->rchild);	//双序遍历左子树
}

int main()
{
	BiTree T = NULL;
	char ch;
	
	while(1)
	{
		system("cls");
		printf("******************二叉树的基本操作*************gengteng@gmail.com*************\n\n");
		printf("1．输入先序序列，建立一棵二叉树；\n");
		printf("2．输出二叉树的前序遍历结果；\n");
		printf("3．输出二叉树的中序遍历结果；\n");
		printf("4．输出二叉树的后序遍历结果；\n");
		printf("5．统计二叉树的叶结点个数；\n");
		printf("6．统计二叉树的结点个数；\n");
		printf("7．计算二叉树的深度；\n");
		printf("8．交换二叉树每个结点的左孩子和右孩子；\n");
		printf("9．（选做）输出二叉树的双序遍历结果：\n");
		printf("0．退出。\n");
		printf("\n");
		printf("请输入功能序号：\n\n");
		ch = getch();
		switch (ch)
		{
		case '1':
			printf("1.请按先序序列输入二叉树数据（无数据以空格表示）：\n");
			CreateBiTreeInPreOrder(T);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '2':
			printf("2.二叉树的前序遍历结果：\n");
			PreOrderTraverse(T);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '3':
			printf("3.二叉树的中序遍历结果：\n");
			InOrderTraverse(T);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '4':
			printf("4.二叉树的后序遍历结果：\n");
			PostOrderTraverse(T);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '5':
			printf("5.二叉树的叶结点个数：\n");
			printf("%d",LeafNodeCount(T));
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '6':
			printf("6.二叉树的结点个数：\n");
			printf("%d",NodeCount(T));
			printf("\n按任意键继续...\n");
			getch();
			break;			
		case '7':
			printf("7．二叉树的深度；\n");
			printf("%d",Depth(T));
			printf("\n按任意键继续...\n");			
			getch();
			break;
		case '8':
			printf("8．交换二叉树每个结点的左孩子和右孩子：\n");
			ExchangeBiTree(T);
			printf("交换结束！");
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '9':
			printf("9．二叉树的双序遍历结果：\n");
			DblOrderTraverse(T);
			printf("\n按任意键继续...\n");
			getch();
			break;
		case '0':
			return 0;
			break;
		default:
			printf("输入错误！按任意键重新输入。\n");
			getch();
			break;
		}
	}
}
