#include<windows.h>
#include"menu.h"
#include"ImageProc.h"

HBITMAP hBitmap = NULL;
bool ShowImage = false;
int switch1 = 0;//用来控制颜色更换的开关

DWORD WINAPI ChangeColorThread(LPVOID param)
{
	HWND hwnd = (HWND)param;

	if (switch1 == 1)
	{
		for (int loop = 0;; loop++)
		{
			if (switch1 == 0)
				break;

			Proc("test.bmp", "love.bmp");

			hBitmap = (HBITMAP)LoadImage(NULL, L"love.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);

			Sleep(200); // 延时 200 毫秒
			
		}
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case OPEN_ORINGIN:
		{
			hBitmap = (HBITMAP)LoadImage(NULL, L"test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (hBitmap == NULL)
			{
				MessageBox(hwnd, L"Failed to load image", L"Error", MB_OK | MB_ICONERROR);
				return 0;
			}

			// 获取图像尺寸
			BITMAP bm;
			GetObject(hBitmap, sizeof(bm), &bm);

			// 获取菜单的高度
			int menuHeight = GetSystemMetrics(SM_CYMENU);

			// 调整窗口大小，包含菜单的高度
			SetWindowPos(hwnd, NULL, 0, 0, bm.bmWidth+10, bm.bmHeight + 58, SWP_NOMOVE);

			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}

		case ID_PROC:
		{
			if (switch1 == 1)
			{
				switch1 = 0;
				Sleep(500);
			}

			Proc("test.bmp", "love.bmp");

			hBitmap = (HBITMAP)LoadImage(NULL, L"love.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			InvalidateRect(hwnd, NULL, TRUE);

			break;
		}

		case ID_CHANGECOLOR:
		{
			if (switch1 == 0)
			{
				switch1 = 1;
				CreateThread(NULL, 0, ChangeColorThread, hwnd, 0, NULL);
				break;
			}

			if (switch1 == 1)
			{
				switch1 = 0;
			}

			break;
		}

		case ID_EXIT:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
	}
//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

	case WM_PAINT:
	{

		PAINTSTRUCT ps;//保存绘图环境，配合BeginPaint和EndPaint使用
		HDC hdc = BeginPaint(hwnd, &ps);

		// 输出 ps.rcPaint 信息到调试窗口
		TCHAR debugMsg[128];
		wsprintf(debugMsg, L"ps.rcPaint: left=%d, top=%d, right=%d, bottom=%d/n",
			ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
		OutputDebugString(debugMsg);

		HBRUSH blueBrush = CreateSolidBrush(RGB(173, 216, 230));
		FillRect(hdc, &ps.rcPaint, blueBrush);
		DeleteObject(blueBrush);

		// 绘制图像
		if (hBitmap)
		{
			HDC memDC = CreateCompatibleDC(hdc); // 创建内存设备上下文
			HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap); // 选择图像到内存设备上下文

			// 获取图像尺寸
			BITMAP bm;
			GetObject(hBitmap, sizeof(bm), &bm);

			// 在窗口中绘制图像
			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);

			// 清理
			SelectObject(memDC, oldBitmap);
			DeleteDC(memDC);
		}

		EndPaint(hwnd, &ps);
		return 0;
	}break;

	case WM_DESTROY:
	
		PostQuitMessage(0);
		return 0;
	break;
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

	//目标：计算窗口中心位置，将窗口初始化在窗口中央
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // 屏幕宽度
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度


////////////////////////////////////////////////////////////////////////////////////////////
	// 添加菜单
	HMENU hMenubar = CreateMenu();

	HMENU hFile = CreateMenu();
	AppendMenu(hFile, MF_STRING, OPEN_ORINGIN, L"打开初始图像");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_PROC, L"处理图像（再次点击更换颜色）");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_CHANGECOLOR, L"自动播放（再次点击停止）");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_EXIT, L"退出");

	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"文件");

	// 3. 创建窗口时附加菜单
	HWND hwnd = CreateWindowEx(
		0, L"MyWindowClass", L"图像处理软件",
		WS_OVERLAPPEDWINDOW,
		screenWidth/2-320, screenHeight/2-300, 522, 570,
		NULL, hMenubar, hInstance, NULL);

	if (!hwnd) return 0;
////////////////////////////////////////////////////////////////////////////////////////////
	ShowWindow(hwnd, nCmdShow);  // 显示窗口
	UpdateWindow(hwnd);  // 更新窗口

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//TranslateMessage(&msg);键盘操作
		DispatchMessage(&msg);//消息分发给过程函数
	}

	return 0;
}