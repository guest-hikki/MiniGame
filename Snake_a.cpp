//̰���� ����ʵ��
#include <stdio.h>
#include <graphics.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

/***************************���ݲ���************************************/
#define LEN 2000	//�ߵ���󳤶�
#define SIZE 10 //һ���ߵĿ��
int speed = 70;//���ƶ����ٶȣ�ԽСԽ�죩
/*��*/
enum PATH {up=72,down=80,left=75,right=77};//_getch�����Է�����Ķ�ȡ�����������ߵķ���
typedef struct coor//�洢�ߵ�����
{
	int x;
	int y;
}Coor;
typedef struct snake//�洢�����ߵ�����
{
	int len;//�ߵĳ���(����)
	PATH path;//�ߵķ���
	Coor szb[LEN];
}Snake;
Snake snake;//����һ����
/*ʳ��*/
typedef struct food
{
	int x;
	int y;
	bool iseat;
}Food;
Food food;//����һ��ʳ��

/***************************��������************************************/
void FirstPage(void);//��ҳ������
/*��ҳ����*/
void TestSpace(void);//���ð��ո���˳�
void InputText(void);
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y);

void GameInterface(void);//��Ϸ������
/*��Ϸ����*/
void GameInit(void);//��Ϸ��ʼ������
void DrawGame(void);//����ͼ�ν���
void SnakeMove(void);//�ߵ��ƶ�
void ChangePath(void);//�ı��ߵķ���
void CreateFood(void);//����ʳ��
void EatFood(void);//�ж��Ƿ�Ե�ʳ��
bool GameOver(void);//�����ж�
/*�û���ʾ*/
void InputRemind(void);//��ʾ����

int main()
{
	//��ʼ��һ�����ڽ���
	initgraph(640, 480);
	//���Ĵ�����ʾ
	HWND hnd = GetHWnd();
	int flag;
	SetWindowText(hnd, "̰����1.0");
	//���ñ�����ɫ
	setbkcolor(RGB(30, 30, 30));
	cleardevice();
	//��ҳ����
		FirstPage();
	while (1)
	{
		//��Ϸ����
		GameInterface();
		//��Ϣ����
		flag=MessageBox(hnd, "		  �����", "��", MB_ABORTRETRYIGNORE);
		if (flag == IDRETRY)
			continue;
		else if (flag == IDIGNORE)
			break;
		else
			return 0;
	}
	InputRemind();
	_getch();//��ֹ��������
	return 0;
}

/*��ҳ����*/
void FirstPage(void)
{
	//����pngͼƬ
	IMAGE img;
	loadimage(&img, "./01.png");
	drawAlpha(&img,50,200);
	//��������
	InputText();
	//��������
	PlaySound("01.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//���ð��ո���˳�
	TestSpace();
	//����
	cleardevice();
	//ֹͣ����
	PlaySound(NULL, 0, 0);
}

/*��Ϸ����*/
void GameInterface(void)
{
	bool dead = false;
	//��������
	PlaySound("02.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//��ʼ��
	GameInit();
	while (1)
	{
		while (!_kbhit())//��⵽���������˳�ѭ��,�ɽ��˼�������ChangePath()�ڣ���ֻ��һ��ѭ�����������ӳٺܸߣ����ʺ�
		{
			//�ƶ���
			SnakeMove();
			//�����ߺ�ʳ��
			DrawGame();
			//����ʳ��
			CreateFood();
			//�ж��Ƿ�Ե�ʳ��
			EatFood();
			if(dead = GameOver())
				break;
			Sleep(speed);
		}
		if (dead)
		{
			PlaySound("04.wav", NULL, SND_FILENAME | SND_ASYNC);//������Ч
			break;
		}
		//�м���������ı��ߵķ���
		ChangePath();
	}
}

/*�Ӻ���*/
// ����PNGͼ��ȥ͸������
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //xΪ����ͼƬ��X���꣬yΪY����
{

	// ������ʼ��
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}

//���ð��ո���˳�
void TestSpace(void)
{
	char chInput;
	while (1)
	{
		if (_kbhit())
		{
			chInput = _getch();
			if (' ' == chInput)
			{
				break;
			}
		}
	}
}

//��ʾ����
void InputText(void)
{
	settextcolor(RGB(190, 183, 255));
	settextstyle(70, 0, "����Ͱ��ջ��� H");
	outtextxy(65, 50, "��ӭ����̰���ߵ�����");
	setbkmode(TRANSPARENT);
	settextcolor(RGB(205, 205, 205));
	settextstyle(40, 0, "����Ͱ��ջ��� B");
	outtextxy(280, 250, "���¿ո�ʼ��Ϸ");
	outtextxy(360, 320, "�÷�������Ʒ���");
	setbkmode(TRANSPARENT);
}

//��ʼ����Ϸ
void GameInit(void)
{
	/*��ʼ����*/
	snake.len = 3;
	snake.path = right;
	snake.szb[0].x = 100;
	snake.szb[0].y = 100;
	snake.szb[1].x = 100-SIZE;
	snake.szb[1].y = 100;
	snake.szb[2].x = 100 - SIZE * 2;
	snake.szb[2].y = 100;
	/*��ʼ��ʳ��*/
	food.iseat = true;
}

//�����ߺ�ʳ��
void DrawGame(void)
{
	cleardevice();//����
	BeginBatchDraw();
	/*������*/
	for (int i = 0; i < snake.len; i++)
	{
		if (i == 0)
		{
			setfillcolor(RGB(199, 199, 199));
			solidroundrect(snake.szb[i].x, snake.szb[i].y, snake.szb[i].x + SIZE, snake.szb[i].y + SIZE, 5, 5);
		}
		else
		{
			setlinecolor(RGB(199, 199, 199));
			setlinestyle(PS_SOLID | PS_JOIN_ROUND, 1);
			roundrect(snake.szb[i].x, snake.szb[i].y, snake.szb[i].x + SIZE, snake.szb[i].y + SIZE, 5, 5);
		}
	}
	/*����ʳ��*/
	setfillcolor(RGB(125, 59, 188));
	solidcircle(food.x + 5, food.y + 5, 5);
	EndBatchDraw();
}

//�ߵ��ƶ�����ÿ���ƶ�������ͷ�ķ���ı������ߵ�����һ��
void SnakeMove(void)
{
	for (int i = snake.len - 1; i > 0 ; i--)//���Ƴ�����
	{
		snake.szb[i] = snake.szb[i - 1];
	}
	switch (snake.path)//������ͷ
	{
	case up:
		snake.szb[0].y -= SIZE;
		break;
	case down:
		snake.szb[0].y += SIZE;
		break;
	case right:
		snake.szb[0].x += SIZE;
		break;
	case left:
		snake.szb[0].x -= SIZE;
		break;
	}
}

//�ı��ߵķ���(��⵽�����������)
void ChangePath(void)
{
	char key;
	key = _getch();
	switch (key)
	{
	case left:
		if (snake.szb[0].x!=snake.szb[1].x+10)
			snake.path = left;
		break;
	case right:
		if (snake.szb[0].x != snake.szb[1].x - 10)
			snake.path = right;
		break;
	case up:
		if (snake.szb[0].y != snake.szb[1].y + 10)
			snake.path = up;
		break;
	case down:
		if (snake.szb[0].y != snake.szb[1].y - 10)
			snake.path = down;
		break;
	case ' '://������������ʵ����ͣ����
		_getch();
		break;
	}
}

//ʳ�����
void CreateFood(void)
{
	srand((unsigned int)time(NULL));//���������
	bool repeat;
	while (food.iseat)
	{
		repeat = false;
		food.x = rand() % 63 * 10;
		food.y = rand() % 47 * 10;
		for (int i = 0; i < snake.len; i++)
		{
			if (snake.szb[i].x == food.x && snake.szb[i].y == food.y)
			{
				repeat = true;
			}
		}
		if (!repeat)
		{
			food.iseat = false;
		}
	}
}

//�ж��Ƿ�Ե�ʳ��
void EatFood(void)
{
	if (food.x == snake.szb[0].x && food.y == snake.szb[0].y)
	{
		mciSendString("close 05.mp3", 0, 0, 0);
		mciSendString("open 05.mp3",0,0,0);
		mciSendString("play 05.mp3", 0, 0, 0);
		snake.len++;
		food.iseat = true;
	}
}

//������Ϸ���ж�
bool GameOver(void)
{
	if (snake.szb[0].x < 0 || snake.szb[0].x > 640 || snake.szb[0].y < 0 || snake.szb[0].y > 480)
	{
		return true;
	}
	for (int i = 1; i < snake.len - 1; i++)
	{
		if (snake.szb[0].x == snake.szb[i].x && snake.szb[0].y == snake.szb[i].y)
			return true;
	}
	return false;
}

//��ʾ����
void InputRemind(void)
{
	setbkmode(TRANSPARENT);
	settextcolor(RGB(122, 192, 255));
	settextstyle(50, 0, "����Ͱ��ջ��� B");
	outtextxy(150, 350, "�����ⰴ���˳���Ϸ");
}