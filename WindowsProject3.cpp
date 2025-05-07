#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <winsock2.h>
#include<windows.h>
#include"menu.h"
#include <ws2tcpip.h>  
#pragma comment(lib, "ws2_32.lib")  // 链接 Winsock 库

int port = 22;//端口

HFONT hFont; // 字体句柄
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//网络部分
DWORD WINAPI NetworkThread(LPVOID lpParam) 
{
	// 用于网络连接的变量
	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in serverAddr;

	// 初始化 Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		MessageBox(NULL, L"请检查WSAStartup", L"错误", MB_OK | MB_ICONERROR);
		return 1;
	}

	// 创建套接字
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		MessageBox(NULL, L"创建套接字失败", L"错误", MB_OK | MB_ICONERROR);
		WSACleanup();
		return 1;
	}

	// 设置服务器地址
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, "116.205.135.236", &(serverAddr.sin_addr));  // 修正：使用 serverAddr

	// 连接服务器
	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		MessageBox(NULL, L"服务器连接失败", L"错误", MB_OK | MB_ICONERROR);
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	MessageBox(NULL, L"服务器连接成功", L"成功", MB_OK | MB_ICONINFORMATION);

	// 发送数据
	const char* sendMsg = "success!";
	send(sock, sendMsg, strlen(sendMsg), 0);

	// 接收数据
	char recvBuf[512];
	int recvLen = recv(sock, recvBuf, sizeof(recvBuf) - 1, 0);
	if (recvLen > 0) {
		recvBuf[recvLen] = '\0'; // 添加终止符
		MessageBoxA(NULL, recvBuf, "收到消息", MB_OK);
	}

	// 清理
	closesocket(sock);
	WSACleanup();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 窗口过程函数，处理窗口消息
	switch (uMsg)
	{
	case WM_CREATE:
	{
		hFont = CreateFont(
			-MulDiv(12, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), // 高度：12号字
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			TEXT("黑体") // 字体名称
		);
	}

	case WM_COMMAND:
	{
		switch(wParam)
		{
		case OPEN_ORINGIN:
		{
			CreateThread(NULL, 0, NetworkThread, NULL, 0, NULL);
			break;
		}

		case ID_EXIT:
		{
			PostQuitMessage(0);
			break;
		}

		case ID_PROC:
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			TCHAR msg[100];
			wsprintf(msg, L"客户区大小：%d x %d", width, height);
			MessageBox(hwnd, msg, L"窗口尺寸", MB_OK);
			break;
		}
			
		case ID_BUTTON1:
		{
			HWND hEdit = GetDlgItem(hwnd, 1);  // 获取输入框句柄

			TCHAR text[256];
			GetWindowText(hEdit, text, 256);  // 获取用户输入的端口号

			SetWindowText(hEdit, L"");

			// 检查是否为空或非数字
			if (wcslen(text) == 0) 
			{
				MessageBox(hwnd, L"输入有误：不能为空", L"错误", MB_OK | MB_ICONERROR);
			}
			else
			{
				BOOL valid = TRUE;
				for (int i = 0; text[i] != L'\0'; ++i) 
				{
					if (!iswdigit(text[i])) 
					{  // 检查是否是宽字符数字
						valid = FALSE;
						break;
					}
				}

				if (!valid) {
					MessageBox(hwnd, L"输入有误：只能输入数字", L"错误", MB_OK | MB_ICONERROR);
				}
				else
				{
					port = _wtoi(text);
					// 输入正确，显示提示
					TCHAR message[300];
					wsprintf(message, L"成功将端口设置为：%s", text);
					MessageBox(hwnd, message, L"提示", MB_OK | MB_ICONINFORMATION);
				}
			}

			break;
		}
		}
		break;
	}

	case WM_SETCURSOR:
	{
		// lParam 的低位是 HitTest 结果，告诉你鼠标在哪个区域
		// LOWORD(lParam) 不可靠，应该用 wParam 和 HIWORD(lParam)

		// 判断是否在客户区
		if (LOWORD(lParam) == HTCLIENT)//LOWORD是表示X坐标
		{
			SetCursor(LoadCursor(NULL, IDC_CROSS)); // 设置普通箭头
			return TRUE; // 表示我们处理了光标设置
		}

		// 否则让系统处理，比如边框区域让系统显示缩放指针
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}


	case WM_PAINT:
	{
		PAINTSTRUCT ps;//保存绘图环境，配合BeginPaint和EndPaint使用
		HDC hdc = BeginPaint(hwnd, &ps);

		HBRUSH blueBrush = CreateSolidBrush(RGB(220, 226, 241));
		FillRect(hdc, &ps.rcPaint, blueBrush);
		DeleteObject(blueBrush);

		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	// 注册窗口类
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyWindowClass";
	RegisterClass(&wc);

	// 目标：计算窗口中心位置，将窗口初始化在窗口中央
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // 屏幕宽度
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度

	////////////////////////////////////////////////////////////////////////////////////////////
	// 添加菜单
	HMENU hMenubar = CreateMenu();

	HMENU hFile = CreateMenu();
	AppendMenu(hFile, MF_STRING, OPEN_ORINGIN, L"连接服务器");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_EXIT, L"退出");
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"开始");

	HMENU hProc = CreateMenu();
	AppendMenu(hProc, MF_STRING, ID_PROC, L"目前窗口大小");
	AppendMenu(hProc, MF_SEPARATOR, 0, NULL);
	AppendMenu(hProc, MF_STRING, ID_CHANGECOLOR, L"");
	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hProc, L"菜单");

	// 3. 创建窗口时附加菜单
	HWND hwnd = CreateWindowEx(
		0, L"MyWindowClass", L"连接",
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
		screenWidth / 2 - 250, screenHeight / 2 - 250, 491, 250,
		NULL, hMenubar, hInstance, NULL);



	//创建按钮
	HWND hButton = CreateWindowEx(
		0, L"BUTTON", L"设置端口",                // 按钮控件，标题
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 风格
		200, 100, 100, 30,                          // 位置和大小
		hwnd, HMENU(ID_BUTTON1), hInstance, NULL);      // 控件ID是1001

	if (!hwnd) return 0;
	
	// 创建编辑框（输入框）
	HWND hEdit = CreateWindowEx(
		0, L"EDIT", L"",                    // 控件类型
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,  // 控件风格：加上ES_AUTOHSCROLL使得输入框支持水平滚动
		50, 50, 400, 25,                  // 位置和大小
		hwnd, (HMENU)1, wc.hInstance, NULL // 控件标识符和实例句柄
	);

	// 更改字体
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	////////////////////////////////////////////////////////////////////////////////////////////

	ShowWindow(hwnd, nCmdShow);  // 显示窗口
	UpdateWindow(hwnd);  // 更新窗口

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//键盘操作
		DispatchMessage(&msg);//消息分发给过程函数
	}

	return 0;
}