//贪吃蛇 数组实现
#include <stdio.h>
#include <graphics.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

/***************************数据部分************************************/
#define LEN 2000	//蛇的最大长度
#define SIZE 10 //一节蛇的宽度
int speed = 70;//蛇移动的速度（越小越快）
/*蛇*/
enum PATH {up=72,down=80,left=75,right=77};//_getch函数对方向键的读取参数，定义蛇的方向
typedef struct coor//存储蛇的坐标
{
	int x;
	int y;
}Coor;
typedef struct snake//存储整条蛇的数据
{
	int len;//蛇的长度(节数)
	PATH path;//蛇的方向
	Coor szb[LEN];
}Snake;
Snake snake;//定义一条蛇
/*食物*/
typedef struct food
{
	int x;
	int y;
	bool iseat;
}Food;
Food food;//定义一个食物

/***************************函数部分************************************/
void FirstPage(void);//首页主函数
/*首页函数*/
void TestSpace(void);//设置按空格键退出
void InputText(void);
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y);

void GameInterface(void);//游戏主函数
/*游戏函数*/
void GameInit(void);//游戏初始化函数
void DrawGame(void);//绘制图形界面
void SnakeMove(void);//蛇的移动
void ChangePath(void);//改变蛇的方向
void CreateFood(void);//产生食物
void EatFood(void);//判断是否吃到食物
bool GameOver(void);//死亡判定
/*用户提示*/
void InputRemind(void);//提示输入

int main()
{
	//初始化一个窗口界面
	initgraph(640, 480);
	//更改窗口显示
	HWND hnd = GetHWnd();
	int flag;
	SetWindowText(hnd, "贪吃蛇1.0");
	//设置背景颜色
	setbkcolor(RGB(30, 30, 30));
	cleardevice();
	//首页部分
		FirstPage();
	while (1)
	{
		//游戏主体
		GameInterface();
		//消息弹窗
		flag=MessageBox(hnd, "		  你挂了", "菜", MB_ABORTRETRYIGNORE);
		if (flag == IDRETRY)
			continue;
		else if (flag == IDIGNORE)
			break;
		else
			return 0;
	}
	InputRemind();
	_getch();//防止程序闪退
	return 0;
}

/*首页部分*/
void FirstPage(void)
{
	//导入png图片
	IMAGE img;
	loadimage(&img, "./01.png");
	drawAlpha(&img,50,200);
	//插入字体
	InputText();
	//播放音乐
	PlaySound("01.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//设置按空格键退出
	TestSpace();
	//清屏
	cleardevice();
	//停止音乐
	PlaySound(NULL, 0, 0);
}

/*游戏主体*/
void GameInterface(void)
{
	bool dead = false;
	//播放音乐
	PlaySound("02.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//初始化
	GameInit();
	while (1)
	{
		while (!_kbhit())//检测到键盘输入退出循环,可将此检测添加至ChangePath()内，则只需一层循环，但那样延迟很高，不适合
		{
			//移动蛇
			SnakeMove();
			//绘制蛇和食物
			DrawGame();
			//产生食物
			CreateFood();
			//判断是否吃到食物
			EatFood();
			if(dead = GameOver())
				break;
			Sleep(speed);
		}
		if (dead)
		{
			PlaySound("04.wav", NULL, SND_FILENAME | SND_ASYNC);//死亡音效
			break;
		}
		//有键盘输入则改变蛇的方向
		ChangePath();
	}
}

/*子函数*/
// 载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

//设置按空格键退出
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

//显示文字
void InputText(void)
{
	settextcolor(RGB(190, 183, 255));
	settextstyle(70, 0, "阿里巴巴普惠体 H");
	outtextxy(65, 50, "欢迎来到贪吃蛇的世界");
	setbkmode(TRANSPARENT);
	settextcolor(RGB(205, 205, 205));
	settextstyle(40, 0, "阿里巴巴普惠体 B");
	outtextxy(280, 250, "按下空格开始游戏");
	outtextxy(360, 320, "用方向键控制方向");
	setbkmode(TRANSPARENT);
}

//初始化游戏
void GameInit(void)
{
	/*初始化蛇*/
	snake.len = 3;
	snake.path = right;
	snake.szb[0].x = 100;
	snake.szb[0].y = 100;
	snake.szb[1].x = 100-SIZE;
	snake.szb[1].y = 100;
	snake.szb[2].x = 100 - SIZE * 2;
	snake.szb[2].y = 100;
	/*初始化食物*/
	food.iseat = true;
}

//绘制蛇和食物
void DrawGame(void)
{
	cleardevice();//清屏
	BeginBatchDraw();
	/*绘制蛇*/
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
	/*绘制食物*/
	setfillcolor(RGB(125, 59, 188));
	solidcircle(food.x + 5, food.y + 5, 5);
	EndBatchDraw();
}

//蛇的移动——每次移动根据蛇头的方向改变整条蛇的坐标一次
void SnakeMove(void)
{
	for (int i = snake.len - 1; i > 0 ; i--)//绘制除蛇身
	{
		snake.szb[i] = snake.szb[i - 1];
	}
	switch (snake.path)//绘制蛇头
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

//改变蛇的方向(检测到输入后开启函数)
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
	case ' '://利用阻塞函数实现暂停功能
		_getch();
		break;
	}
}

//食物产生
void CreateFood(void)
{
	srand((unsigned int)time(NULL));//随机数种子
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

//判断是否吃到食物
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

//结束游戏的判定
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

//提示输入
void InputRemind(void)
{
	setbkmode(TRANSPARENT);
	settextcolor(RGB(122, 192, 255));
	settextstyle(50, 0, "阿里巴巴普惠体 B");
	outtextxy(150, 350, "按任意按键退出游戏");
}