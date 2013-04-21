#include "stdio.h"//�����������printf()
#include "conio.h"//�����������getch()
#include "stdlib.h"//�����������system()

//��������������洢�ṹ�嶨��
typedef struct BNode{
	char data;						//���������
	struct BNode *lchild,*rchild;	//���Һ���ָ��
}BNode,*BiTree;


void CreateBiTreeInPreOrder(BiTree &T);// 1���Զ��������ʾ������������һ�ö�������
void PreOrderTraverse(BiTree T);// 2�������������ǰ����������
void InOrderTraverse(BiTree T);// 3�������������������������
void PostOrderTraverse(BiTree T);// 4������������ĺ�����������
int LeafNodeCount(BiTree T);// 5��ͳ�ƶ�������Ҷ��������
int NodeCount(BiTree T);// 6��ͳ�ƶ������Ľ�������
int Depth(BiTree T);// 7���������������ȣ�
void ExchangeBiTree(BiTree &T);// 8������������ÿ���������Ӻ��Һ��ӣ�

//ѡ��
void DblOrderTraverse(BiTree &T);// 9�������������˫����������

void visit(BiTree T);//���ʽ��ĺ���



void visit(BiTree T)
{
	printf("%2c",T->data);//���ʽ�㼴����ý������
}

void CreateBiTreeInPreOrder(BiTree &T)// 1���Զ��������ʾ������������һ�ö�������
{
	char ch;
	ch = getch();
							
	if(' '== ch)							//�������ո񣬸ö�����Ϊ��
	{
		T = NULL;
		printf(" _");					//�������Ϊ�ո�����»���'_'��ʾ
	}
	else
	{
		printf("%2c",ch);				//������벻Ϊ�ո��������
		T = new BNode;
		T->data = ch;						//�����ڵ㸴��
		CreateBiTreeInPreOrder(T->lchild);	//����������
		CreateBiTreeInPreOrder(T->rchild);	//����������
	}
}

void PreOrderTraverse(BiTree T)// 2�������������ǰ����������
{
	if(NULL == T)	return;			//��������Ϊ�գ���ղ���������
	visit(T);						//���ʸ��ڵ�
	PreOrderTraverse(T->lchild);	//�������������
	PreOrderTraverse(T->rchild);	//�������������
}

void InOrderTraverse(BiTree T)// 3�������������������������
{
	if(NULL == T)	return;		//��������Ϊ�գ���ղ���������
	InOrderTraverse(T->lchild);	//�������������
	visit(T);					//���ʸ��ڵ�
	InOrderTraverse(T->rchild);	//�������������
}

void PostOrderTraverse(BiTree T)// 4������������ĺ�����������
{
	if(NULL == T)	return;			//��������Ϊ�գ���ղ���������
	PostOrderTraverse(T->lchild);	//�������������
	PostOrderTraverse(T->rchild);	//�������������
	visit(T);						//���ʸ��ڵ�
}

int LeafNodeCount(BiTree T)// 5��ͳ�ƶ�������Ҷ��������
{
	if(NULL == T)	return 0;									//��������Ϊ�գ��򷵻�0������
	if(!(T->lchild||T->rchild))	return 1;						//�����������������ڣ�����1������
	return LeafNodeCount(T->lchild)+LeafNodeCount(T->rchild);	//����������Ҷ�ӽڵ�����������Ҷ�ӽڵ���֮��
}

int NodeCount(BiTree T)// 6��ͳ�ƶ������Ľ�������
{
	if(NULL == T) return 0;										//��������Ϊ�գ��򷵻�0������
	return NodeCount(T->lchild)+NodeCount(T->rchild)+1;			//�����������ڵ������������ڵ���֮�ͼ�1
}

int Depth(BiTree T)// 7���������������ȡ�
{
	if(NULL == T) return 0;	//��������Ϊ�գ��򷵻�0������
	int ld,rd;
	ld = Depth(T->lchild);
	rd = Depth(T->rchild);
	return (ld>rd?ld:rd)+1;	//��������������Ƚϴ��߼�1
}

void ExchangeBiTree(BiTree &T)// 8������������ÿ���������Ӻ��Һ��ӣ�
{
	if(NULL == T)	return;		//��������Ϊ�գ���ղ���������
	ExchangeBiTree(T->lchild);	//������������ÿ���������Ӻ��Һ���
	ExchangeBiTree(T->rchild);	//������������ÿ���������Ӻ��Һ���

	BNode *t;
	t = T->lchild;				//������ǰ�����������Ӻ��Һ���
	T->lchild = T->rchild;
	T->rchild = t;
}

void DblOrderTraverse(BiTree &T) //9����ѡ���������������˫����������
{
	if(NULL == T)	return;			//��������Ϊ�գ���ղ��������򷵻�
	visit(T);						//���ʸ��ڵ�
	DblOrderTraverse(T->lchild);	//˫�����������
	visit(T);						//�ٴη��ʸ��ڵ�
	DblOrderTraverse(T->rchild);	//˫�����������
}

int main()
{
	BiTree T = NULL;
	char ch;
	
	while(1)
	{
		system("cls");
		printf("******************�������Ļ�������*************gengteng@gmail.com*************\n\n");
		printf("1�������������У�����һ�ö�������\n");
		printf("2�������������ǰ����������\n");
		printf("3�������������������������\n");
		printf("4������������ĺ�����������\n");
		printf("5��ͳ�ƶ�������Ҷ��������\n");
		printf("6��ͳ�ƶ������Ľ�������\n");
		printf("7���������������ȣ�\n");
		printf("8������������ÿ���������Ӻ��Һ��ӣ�\n");
		printf("9����ѡ���������������˫����������\n");
		printf("0���˳���\n");
		printf("\n");
		printf("�����빦����ţ�\n\n");
		ch = getch();
		switch (ch)
		{
		case '1':
			printf("1.�밴��������������������ݣ��������Կո��ʾ����\n");
			CreateBiTreeInPreOrder(T);
			printf("\n�����������...\n");
			getch();
			break;
		case '2':
			printf("2.��������ǰ����������\n");
			PreOrderTraverse(T);
			printf("\n�����������...\n");
			getch();
			break;
		case '3':
			printf("3.��������������������\n");
			InOrderTraverse(T);
			printf("\n�����������...\n");
			getch();
			break;
		case '4':
			printf("4.�������ĺ�����������\n");
			PostOrderTraverse(T);
			printf("\n�����������...\n");
			getch();
			break;
		case '5':
			printf("5.��������Ҷ��������\n");
			printf("%d",LeafNodeCount(T));
			printf("\n�����������...\n");
			getch();
			break;
		case '6':
			printf("6.�������Ľ�������\n");
			printf("%d",NodeCount(T));
			printf("\n�����������...\n");
			getch();
			break;			
		case '7':
			printf("7������������ȣ�\n");
			printf("%d",Depth(T));
			printf("\n�����������...\n");			
			getch();
			break;
		case '8':
			printf("8������������ÿ���������Ӻ��Һ��ӣ�\n");
			ExchangeBiTree(T);
			printf("����������");
			printf("\n�����������...\n");
			getch();
			break;
		case '9':
			printf("9����������˫����������\n");
			DblOrderTraverse(T);
			printf("\n�����������...\n");
			getch();
			break;
		case '0':
			return 0;
			break;
		default:
			printf("������󣡰�������������롣\n");
			getch();
			break;
		}
	}
}
