#include<windows.h>
#include"menu.h"
#include"ImageProc.h"

HBITMAP hBitmap = NULL;
bool ShowImage = false;
int switch1 = 0;//����������ɫ�����Ŀ���

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

			Sleep(200); // ��ʱ 200 ����
			
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

			// ��ȡͼ��ߴ�
			BITMAP bm;
			GetObject(hBitmap, sizeof(bm), &bm);

			// ��ȡ�˵��ĸ߶�
			int menuHeight = GetSystemMetrics(SM_CYMENU);

			// �������ڴ�С�������˵��ĸ߶�
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
		// lParam �ĵ�λ�� HitTest �����������������ĸ�����
		// LOWORD(lParam) ���ɿ���Ӧ���� wParam �� HIWORD(lParam)

		// �ж��Ƿ��ڿͻ���
		if (LOWORD(lParam) == HTCLIENT)//LOWORD�Ǳ�ʾX����
		{
			SetCursor(LoadCursor(NULL, IDC_CROSS)); // ������ͨ��ͷ
			return TRUE; // ��ʾ���Ǵ����˹������
		}

		// ������ϵͳ��������߿�������ϵͳ��ʾ����ָ��
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
//////////////////////////////////////////////////////////////////////////////////

	case WM_PAINT:
	{

		PAINTSTRUCT ps;//�����ͼ���������BeginPaint��EndPaintʹ��
		HDC hdc = BeginPaint(hwnd, &ps);

		// ��� ps.rcPaint ��Ϣ�����Դ���
		TCHAR debugMsg[128];
		wsprintf(debugMsg, L"ps.rcPaint: left=%d, top=%d, right=%d, bottom=%d/n",
			ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
		OutputDebugString(debugMsg);

		HBRUSH blueBrush = CreateSolidBrush(RGB(173, 216, 230));
		FillRect(hdc, &ps.rcPaint, blueBrush);
		DeleteObject(blueBrush);

		// ����ͼ��
		if (hBitmap)
		{
			HDC memDC = CreateCompatibleDC(hdc); // �����ڴ��豸������
			HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap); // ѡ��ͼ���ڴ��豸������

			// ��ȡͼ��ߴ�
			BITMAP bm;
			GetObject(hBitmap, sizeof(bm), &bm);

			// �ڴ����л���ͼ��
			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);

			// ����
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
	// ע�ᴰ����
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyWindowClass";
	RegisterClass(&wc);

	//Ŀ�꣺���㴰������λ�ã������ڳ�ʼ���ڴ�������
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // ��Ļ���
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); // ��Ļ�߶�


////////////////////////////////////////////////////////////////////////////////////////////
	// ��Ӳ˵�
	HMENU hMenubar = CreateMenu();

	HMENU hFile = CreateMenu();
	AppendMenu(hFile, MF_STRING, OPEN_ORINGIN, L"�򿪳�ʼͼ��");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_PROC, L"����ͼ���ٴε��������ɫ��");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_CHANGECOLOR, L"�Զ����ţ��ٴε��ֹͣ��");
	AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hFile, MF_STRING, ID_EXIT, L"�˳�");

	AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"�ļ�");

	// 3. ��������ʱ���Ӳ˵�
	HWND hwnd = CreateWindowEx(
		0, L"MyWindowClass", L"ͼ�������",
		WS_OVERLAPPEDWINDOW,
		screenWidth/2-320, screenHeight/2-300, 522, 570,
		NULL, hMenubar, hInstance, NULL);

	if (!hwnd) return 0;
////////////////////////////////////////////////////////////////////////////////////////////
	ShowWindow(hwnd, nCmdShow);  // ��ʾ����
	UpdateWindow(hwnd);  // ���´���

	// ��Ϣѭ��
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//TranslateMessage(&msg);���̲���
		DispatchMessage(&msg);//��Ϣ�ַ������̺���
	}

	return 0;
}