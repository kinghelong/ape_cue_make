// cue_ape.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "cue_ape.h"
//#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#define MAX_LOADSTRING 100
#define ONE_SECOND_DATA 176400
#define IDM_OPEN 564
#define IDM_PLAY 456
#define IDM_STOP 5465
#define IDM_CONVERT 5642
#define IDM_CUE 4882

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

 TCHAR fName[1024], outName[1024], CueArtist[255], CueAlbum[255], cueSong[128][255];//
int nPercentageDone = 0, nKillFlag = 0, MusicTime[255], *temp = NULL, array_len = 0;
TCHAR *CueList[128];
char* UnicodeToAnsi(wchar_t *sourceString);

//TICK_COUNT_TYPE g_nInitialTickCount = 0;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Album(HWND, UINT, WPARAM, LPARAM);
TIME_STRUCT       * cue_time(HWND hWnd, TCHAR *fileName);
TCHAR             * OpenFile(HWND hWnd);
TCHAR             * OpenCue(HWND hWnd);
LRESULT             CALLBACK mySubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
BOOL                cue_txt_make(int *time, TCHAR **music, TCHAR *artist, TCHAR *album, TCHAR *fileName, int soundNum);
VOID                SetOwner(HWND hWnd);
BOOL                WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR  lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CUEAPE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CUEAPE));
    MSG msg;
    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CUEAPE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CUEAPE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR CUE_NAME[1024];
    switch (message)
    {
	case WM_CREATE:
	{
		CreateWindowEx(0, L"BUTTON", L"OPEN", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 10,  10, 50, 50, hWnd, (HMENU)IDM_OPEN,    hInst, NULL);
		CreateWindowEx(0, L"BUTTON", L"CUE",  WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 80,  10, 50, 50, hWnd, (HMENU)IDM_CUE,     hInst, NULL);
		CreateWindowEx(0, L"BUTTON", L"PLAY", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 150, 10, 50, 50, hWnd, (HMENU)IDM_PLAY,    hInst, NULL);
		CreateWindowEx(0, L"BUTTON", L"转换", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 220, 10, 50, 50, hWnd, (HMENU)IDM_CONVERT, hInst, NULL);
		
	}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
			case IDM_OPEN:
			{
				TCHAR KL[255];
				TCHAR *F=OpenCue( hWnd);
				if (F != NULL)
				{
					lstrcpy(CUE_NAME, F);
				//	GetCurrentDirectoryW(200, CUE_NAME);//获得当前文件的目录名
				//	GetShortPathNameW(CUE_NAME, KL, 255);//路径缩减形式
					//ShellExecuteW(NULL,L"open", L"notepad.exe", CUE_NAME,  NULL, SW_SHOW);
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ALBUM), hWnd, Album);
				}
			}
				break;
			case IDM_CUE:
			{
				TCHAR *U = OpenFile(hWnd);
				if(U!=NULL)
					lstrcpy(fName,U );
				else
				{
					return 0;
				}
				wchar_t *spInputFilename;
				wchar_t *spOutputFilename;
				TIME_STRUCT *song_time_list;

				spInputFilename = fName;
				int NameLen = lstrlenW(fName);
				lstrcpynW(outName, fName, NameLen - 3);
				lstrcatW(outName, L".wav");//获得wav文件名
				spOutputFilename = outName;
				int result = DecompressFileW(spInputFilename, spOutputFilename, &nPercentageDone, ProgressCallback, &nKillFlag);
				if (result)
					return 0;
				
				song_time_list = cue_time(hWnd, fName);

				array_len = song_time_list->itemNum;
				temp = song_time_list->Array;
				memcpy(MusicTime, temp, array_len * sizeof(int));//把局部变量复制进数组里，得到时间表
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ALBUM), hWnd, Album);

			}
			break;
			case IDM_CONVERT:
			{				
			}
			break;
            case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case VK_RETURN:
			MessageBox(hWnd, L"df", L"fd", MB_OK);		
			break;
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		}
    case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



static void CALLBACK ProgressCallback(int nPercentageDone)
{
	// get the current tick count
	TICK_COUNT_TYPE  nTickCount;
	TICK_COUNT_READ(nTickCount);

	// calculate the progress
	double dProgress = nPercentageDone / 1.e5;                                            // [0...1]
	double dElapsed = (double)(nTickCount - g_nInitialTickCount) / TICK_COUNT_FREQ;    // seconds
	double dRemaining = dElapsed * ((1.0 / dProgress) - 1.0);                            // seconds

																						 // output the progress
	_ftprintf(stderr, _T("Progress: %.1f%% (%.1f seconds remaining, %.1f seconds total)          \r"),
		dProgress * 100, dRemaining, dElapsed);

	// don't forget to flush!
	fflush(stderr);
}

static void DisplayProperUsage(FILE * pFile)
{
	_ftprintf(pFile, _T("Proper Usage: [EXE] [Input File] [Output File] [Mode]\n\n"));

	_ftprintf(pFile, _T("Modes: \n"));
	_ftprintf(pFile, _T("    Compress (fast): '-c1000'\n"));
	_ftprintf(pFile, _T("    Compress (normal): '-c2000'\n"));
	_ftprintf(pFile, _T("    Compress (high): '-c3000'\n"));
	_ftprintf(pFile, _T("    Compress (extra high): '-c4000'\n"));
	_ftprintf(pFile, _T("    Compress (insane): '-c5000'\n"));
	_ftprintf(pFile, _T("    Decompress: '-d'\n"));
	_ftprintf(pFile, _T("    Verify: '-v'\n"));
	_ftprintf(pFile, _T("    Convert: '-nXXXX'\n\n"));

	_ftprintf(pFile, _T("Examples:\n"));
	_ftprintf(pFile, _T("    Compress: mac.exe \"Metallica - One.wav\" \"Metallica - One.ape\" -c2000\n"));
	_ftprintf(pFile, _T("    Decompress: mac.exe \"Metallica - One.ape\" \"Metallica - One.wav\" -d\n"));
	_ftprintf(pFile, _T("    Decompress: mac.exe \"Metallica - One.ape\" auto -d\n"));
	_ftprintf(pFile, _T("    Verify: mac.exe \"Metallica - One.ape\" -v\n"));
	_ftprintf(pFile, _T("    (note: int filenames must be put inside of quotations)\n"));
}


static BOOL CALLBACK CtrlHandlerCallback(DWORD dwCtrlTyp)
{
	switch (dwCtrlTyp)
	{
	case CTRL_C_EVENT:
		_fputts(_T("\n\nCtrl+C: MAC has been interrupted !!!\n"), stderr);
		break;
	case CTRL_BREAK_EVENT:
		_fputts(_T("\n\nBreak: MAC has been interrupted !!!\n"), stderr);
		break;
	default:
		return FALSE;
	}

	fflush(stderr);
	ExitProcess(666);
	return TRUE;
}

int DecompressMain(TCHAR * argv[])
{
	// variable declares
	CSmartPtr<wchar_t> spInputFilename; CSmartPtr<wchar_t> spOutputFilename;
	int nRetVal = ERROR_UNDEFINED;
	int nMode = UNDEFINED_MODE;
	int nCompressionLevel = 0;
	int nPercentageDone;
	const wchar_t* AUTO; const wchar_t* WAV;

	// initialize
#ifdef PLATFORM_WINDOWS
	SetErrorMode(SetErrorMode(0x0003) | 0x0003);
	SetConsoleCtrlHandler(CtrlHandlerCallback, TRUE);
#endif

	// output the header
	_ftprintf(stderr, CONSOLE_NAME);

	// make sure there are at least four arguments (could be more for EAC compatibility)


	// store the constants
#ifdef PLATFORM_WINDOWS
#ifdef _UNICODE
	AUTO = _T("auto");
	WAV = _T(".wav");
#else
	AUTO = CAPECharacterHelper::GetUTF16FromANSI("auto");
	WAV = CAPECharacterHelper::GetUTF16FromANSI(".wav");
#endif
#else
	AUTO = CAPECharacterHelper::GetUTF16FromUTF8((str_utf8*) "auto");
	WAV = CAPECharacterHelper::GetUTF16FromUTF8((str_utf8*) ".wav");
#endif

	// store the filenames
#ifdef PLATFORM_WINDOWS
#ifdef _UNICODE
	spInputFilename.Assign(argv[1], TRUE, FALSE);
	spOutputFilename.Assign(argv[2], TRUE, FALSE);
#else
	spInputFilename.Assign(CAPECharacterHelper::GetUTF16FromANSI(argv[1]), TRUE);
	spOutputFilename.Assign(CAPECharacterHelper::GetUTF16FromANSI(argv[2]), TRUE);
#endif
#else
	spInputFilename.Assign(CAPECharacterHelper::GetUTF16FromUTF8((str_utf8*)argv[1]), TRUE);
	spOutputFilename.Assign(CAPECharacterHelper::GetUTF16FromUTF8((str_utf8*)argv[2]), TRUE);
#endif

	// verify that the input file exists
	/*
	if (!FileExists(spInputFilename))
	{
	_ftprintf(stderr, _T("Input File Not Found...\n\n"));
	exit(-1);
	}
	*/
	// if the output file equals '-v', then use this as the next argument
	TCHAR cMode[256];
	_tcsncpy_s(cMode, 256, argv[2], 255);

	if (_tcsnicmp(cMode, _T("-v"), 2) != 0)
	{
		// check for and skip if necessary the -b XXXXXX arguments (3,4)
		_tcsncpy_s(cMode, 256, argv[3], 255);
	}

	// get the mode
	nMode = UNDEFINED_MODE;
	if (_tcsnicmp(cMode, _T("-c"), 2) == 0)
		nMode = COMPRESS_MODE;    //压缩模式
	else if (_tcsnicmp(cMode, _T("-d"), 2) == 0)
		nMode = DECOMPRESS_MODE;  //解压模式
	else if (_tcsnicmp(cMode, _T("-v"), 2) == 0)
		nMode = VERIFY_MODE;
	else if (_tcsnicmp(cMode, _T("-n"), 2) == 0)
		nMode = CONVERT_MODE;

	// error check the mode
	if (nMode == UNDEFINED_MODE)
	{
		DisplayProperUsage(stderr);
		exit(-1);
	}

	// get and error check the compression level
	if (nMode == COMPRESS_MODE || nMode == CONVERT_MODE)
	{
		nCompressionLevel = _ttoi(&cMode[2]);
		if (nCompressionLevel != 1000 && nCompressionLevel != 2000 &&
			nCompressionLevel != 3000 && nCompressionLevel != 4000 &&
			nCompressionLevel != 5000)
		{
			DisplayProperUsage(stderr);
			return -1;
		}
	}

	// set the initial tick count
	TICK_COUNT_READ(g_nInitialTickCount);

	// process
	int nKillFlag = 0;
	if (nMode == COMPRESS_MODE)
	{
		//压缩模式，转换成APE 文件
		TCHAR cCompressionLevel[16];
		if (nCompressionLevel == 1000) { _tcscpy_s(cCompressionLevel, 16, _T("fast")); }
		if (nCompressionLevel == 2000) { _tcscpy_s(cCompressionLevel, 16, _T("normal")); }
		if (nCompressionLevel == 3000) { _tcscpy_s(cCompressionLevel, 16, _T("high")); }
		if (nCompressionLevel == 4000) { _tcscpy_s(cCompressionLevel, 16, _T("extra high")); }
		if (nCompressionLevel == 5000) { _tcscpy_s(cCompressionLevel, 16, _T("insane")); }

		_ftprintf(stderr, _T("Compressing (%s)...\n"), cCompressionLevel);
		nRetVal = CompressFileW(spInputFilename, spOutputFilename, nCompressionLevel, &nPercentageDone, ProgressCallback, &nKillFlag);
	}
	else if (nMode == DECOMPRESS_MODE)
	{
		//解压文件
		_ftprintf(stderr, _T("Decompressing...\n"));
		if (_tcsicmp(spOutputFilename, AUTO) == 0)
		{
			wchar_t cOutput[MAX_PATH];
			wchar_t * pExtension = wcschr(spInputFilename, '.');
			if (pExtension != NULL)
				*pExtension = 0;
			wcscpy(cOutput, spInputFilename);
			if (pExtension != NULL)
				*pExtension = '.';
			wcscat(cOutput, WAV);
			spOutputFilename.Assign(new wchar_t[wcslen(cOutput) + 1], true);
			wcscpy(spOutputFilename, cOutput);
		}
		nRetVal = DecompressFileW(spInputFilename, spOutputFilename, &nPercentageDone, ProgressCallback, &nKillFlag);
	}
	else if (nMode == VERIFY_MODE)
	{
		_ftprintf(stderr, _T("Verifying...\n"));
		nRetVal = VerifyFileW(spInputFilename, &nPercentageDone, ProgressCallback, &nKillFlag);
	}
	else if (nMode == CONVERT_MODE)
	{
		_ftprintf(stderr, _T("Converting...\n"));
		nRetVal = ConvertFileW(spInputFilename, spOutputFilename, nCompressionLevel, &nPercentageDone, ProgressCallback, &nKillFlag);
	}

	if (nRetVal == ERROR_SUCCESS)
		_ftprintf(stderr, _T("\nSuccess...\n"));
	else
		_ftprintf(stderr, _T("\nError: %i\n"), nRetVal);

	return nRetVal;
}

TIME_STRUCT  *cue_time(HWND hWnd,TCHAR *fileName)
{
	HANDLE hFile, hTxt;
	int db = 0xFEFF;
	int low_count = 0, judge = 0, val = 0, ten = 0, ok = 0, minute = 0, second = 0, fNameLen = 0;
	int16_t musicBuffer[2048];
	DWORD num = 0, fSize = 0;
	int fLong = 0, ring = 0, sample = 0;
	BOOL MUTE_ZONE = FALSE;
	LONG   temp = 0;
	TCHAR report[1255],cue_name[1255],raw_name[1255],wav_name[1255];
	double DB_sin = 0.00, DB_mul = 0.00, DB_sample = 0.00, DB_max = 0.00, mute_second = 0.00;
	int time[255];
	TIME_STRUCT music_info;

	fNameLen = lstrlenW(fileName);
	lstrcpynW(wav_name, fileName, fNameLen  - 3);
	lstrcpynW(raw_name, fileName, fNameLen - 3);
	lstrcpynW(cue_name, fileName, fNameLen - 3);
	lstrcatW(raw_name, L".raw");
	lstrcatW(wav_name, L".wav");
	lstrcatW(cue_name, L".txt");
	//读WAVE文件
	hFile = CreateFile(wav_name, GENERIC_READ,  0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	hTxt = CreateFile(cue_name,  GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hTxt, &db, 2, &num, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		CloseHandle(hTxt);
		MessageBox(hWnd, L"创建文件失败", L"错误", MB_OK);
		return 0;
	}
	fSize = GetFileSize(hFile, &num);
	
	SetFilePointer(hFile, 44, &temp, FILE_BEGIN);
	fSize -= 44;
	for (unsigned int i = 0; i < fSize; i += 4096)
	{
		ReadFile(hFile, musicBuffer, 4096, &num, NULL);
		int db = 0;
		short int value = 0;
		double dou_sum = 0.00, dbms = 0.00;
		LONG64 sum = 0;

		for (int x = 0; x < 2048; x++)
		{
			sum += abs(musicBuffer[x]); //绝对值求和
		}
		dou_sum = ((double)sum) / 1024; //求平均值（2个字节表示一个振幅，所以振幅个数为：size/2个）

		if (dou_sum > 0)
		{
			//db = (int)(20.0*log10(dou_sum));
			DB_mul = 20 * log10(dou_sum / 32767);
		}
		//if (i > 174106800)
	//		MessageBox(hWnd, L"第二段", L"D", MB_OK);
		BOOL act = false;
		if ((DB_mul < -55) || (sum<100))//前置零区不计，因为dou_sum=0。这是第一个小于-55分贝的样本
		{
			act = true;
			low_count++;//低分贝计数
			if (low_count == 1)//如果是第一个低分贝，则把值赋给ok，用于计算下一个低分贝值是不是连续性
			{
				ok = i;//由于是4096个字节取一个平均值，ok也是4096整数倍。
			}
			else
			{
				judge = i - ok;
				if (judge > 4096)//判断是不是连续低分贝区域
				{
					if (low_count > 70)
					{
						val++;//静音区号
						ok = (i - low_count * 2048) / ONE_SECOND_DATA;
						minute = ok / 60;
						second = ok % 60;
						if ((second + 2) > 60)
						{
							second = second - 59;
							minute++;
						}
							swprintf_s(report, 1255, L"第%d段静音区%d分%d秒\r\n", val, minute, second );
							ten = lstrlenW(report);
							WriteFile(hTxt, report, ten * 2, &num, NULL);
						
					}
					low_count = 0;
				}
				else
					ok = i;
			}
		}
		else if (low_count > 70)
		{
			time[val] = ok;
			val++;
			ok = (i - low_count * 2048) / ONE_SECOND_DATA;
			minute = ok / 60;
			second = ok % 60;
			
				swprintf_s(report, 1255, L"第%d段静音区%d分%d秒\r\n", val, minute, second);
				ten = lstrlenW(report);
				WriteFile(hTxt, report, ten * 2, &num, NULL);
				low_count = 0;
			
		}

	}
	CloseHandle(hTxt);
	CloseHandle(hFile);
	music_info.Array = time;
	music_info.itemNum = val;
	return &music_info;
}

TCHAR * OpenFile(HWND hWnd)
{
	/*执行打开对话框选择文件主要代码*/
	TCHAR szBuffer[1024] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"APE\0*.ape\0WAV\0*.wav\0All\0*.*\0";//要选择的文件后缀
	ofn.lpstrInitialDir = L"D:\\";//默认的文件路径
	ofn.lpstrFile = szBuffer;//存放文件的缓冲区
	ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志如果是多选要加上OFN_ALLOWMULTISELECT
	BOOL bSel =	GetOpenFileName(&ofn);
	if (bSel)
	{
//		MessageBox(hWnd, szBuffer, L"!", MB_OK);//在息框显示所选文件路径
		return szBuffer;
	}
	else
		return NULL;
}
TCHAR * OpenCue(HWND hWnd)
{
	/*执行打开对话框选择文件主要代码*/
	TCHAR szBuffer[1024] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"CUE\0*.cue\0TXT\0*.txt\0All\0*.*\0";//要选择的文件后缀
	ofn.lpstrInitialDir = L"D:\\";//默认的文件路径
	ofn.lpstrFile = szBuffer;//存放文件的缓冲区
	ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志如果是多选要加上OFN_ALLOWMULTISELECT
	BOOL bSel = GetOpenFileName(&ofn);
	if (bSel)
	{
		//		MessageBox(hWnd, szBuffer, L"!", MB_OK);//在息框显示所选文件路径
		return szBuffer;
	}
	else
		return NULL;
}
// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:

		return (INT_PTR)TRUE;
	case IDC_LIST:

		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Album(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hList = GetDlgItem(hDlg, IDC_LIST);
	HWND hEdit1, hEdit2, hEdit3, hEdit4, hButton;
	TCHAR Artist[255]=L"sdfsadfsaedfasdfasdf";
	static int pos;
	//int x = 0, y = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowSubclass(hList, &mySubClassProc, 1, 0);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_ADD:
		{			
			hEdit4 = GetDlgItem(hDlg, IDC_EDIT4);
			int a = GetDlgItemTextW(hDlg, IDC_EDIT4, Artist, 255);//我操，竟然需要填父窗口句柄			
			pos = (int)SendMessage(hList, LB_ADDSTRING, 0,(LPARAM)Artist);//获得列表项索引
			SetDlgItemInt(hDlg, IDC_EDIT3, pos + 1, TRUE);//曲目号
			SetDlgItemTextW(hDlg, IDC_EDIT4, NULL);
		}
		break;
		case IDC_EMPTY:
		{			
			SendMessage(hList, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
			pos = -1;
			SetDlgItemInt(hDlg, IDC_EDIT3, pos + 1, TRUE);			
		}
			break;
		case IDC_DELETE:
		{
			int i = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_DELETESTRING, (WPARAM)i, (LPARAM)0);
			pos--;
			SetDlgItemInt(hDlg, IDC_EDIT3, pos + 1, TRUE);
		}
		break;
		case IDC_LIST:
		{
			switch (HIWORD(wParam))
			{
			case LBN_DBLCLK:
			{
				//int max= (int)SendMessage(hList, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				int itemIndex = (int) SendMessage(hList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);//返回当前选择单元的索引
				if (itemIndex == LB_ERR)
				{
					return 0;
				}				
				int textLen = (int) SendMessage(hList, LB_GETTEXTLEN, (WPARAM)itemIndex, 0);
				TCHAR * textBuffer = new TCHAR[textLen + 1];
				SendMessage(hList, LB_GETTEXT, (WPARAM)itemIndex, (LPARAM)textBuffer);
				SetDlgItemInt(hDlg, IDC_EDIT3, itemIndex + 1, TRUE);
			//	MessageBox(NULL, textBuffer, _T("Selected Text from Listbox:"), MB_OK);				
				SetDlgItemTextW(hDlg, IDC_EDIT4, textBuffer);
				delete[] textBuffer;
				textBuffer = NULL;
				return TRUE;
			}
			}
		}
		break;
		case IDOK:
		{			
			HWND OK = GetDlgItem(hDlg, IDOK);
			hButton = GetParent(OK);
			hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
			hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
			hEdit3 = GetDlgItem(hDlg, IDC_EDIT3);
			int track_num = 0;			

			GetDlgItemTextW(hDlg, IDC_EDIT1, CueArtist, 255);//专辑作者
			GetDlgItemTextW(hDlg, IDC_EDIT2, CueAlbum,  255);//专辑名
			track_num = (int)SendMessage(hList, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);//列表框歌曲数
			for (int x = 0; x < track_num ; x++)
			{
				SendMessage(hList, LB_GETTEXT, (WPARAM)x, (LPARAM)((TCHAR**)cueSong[x]));
				CueList[x] = cueSong[x];				
			}
			
			cue_txt_make(MusicTime, (TCHAR **)cueSong, CueArtist, CueAlbum, fName, track_num);
			if ((CueArtist == NULL) && (CueAlbum == NULL))
			{
				MessageBox(NULL, L"没有添写专辑作者名和专辑名", L"注意", MB_OK);
			}
		}
		break;
		case IDCANCEL:
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
		
		default:
			(INT_PTR)FALSE;
			break;
		}
		
	}
	return (INT_PTR)FALSE;
}
LRESULT CALLBACK mySubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{

	RECT  rect;	
	HANDLE      hMem;
	TCHAR  MusicName[4096],singleMusic[4096];
	static HWND hNextViewer;

	GetWindowRect(hWnd, &rect);
	switch (uMsg)
	{
	case WM_CREATE:
		hNextViewer = SetClipboardViewer(hWnd);
		SetOwner(hWnd);
		break;
	case WM_CONTEXTMENU:
	{
		HMENU hMenu = CreatePopupMenu();

		AppendMenu(hMenu, MFT_STRING, ID_COPY, L"复制");
		AppendMenu(hMenu, MFT_STRING, ID_PASTE, L"粘贴");
		AppendMenu(hMenu, MFT_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MFT_STRING, IDM_EXIT, L"退出");

		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON |
			TPM_TOPALIGN |
			TPM_LEFTALIGN,
			LOWORD(lParam),
			HIWORD(lParam), 0, hWnd, NULL);
		DestroyMenu(hMenu);
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_COPY:
			break;
		case ID_PASTE:
		{
			OpenClipboard(hWnd);
			if (hMem = GetClipboardData(CF_UNICODETEXT))
			{
				HANDLE hFile = CreateFile(L"E:\\unicode.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				DWORD num;
				LPWSTR lpMem = (TCHAR *)GlobalLock(hMem);
				int len = lstrlenW(lpMem);
				if (len < 4096)
				{
					lstrcpyW(MusicName, lpMem);
					lstrcatW(MusicName, L"\0");
					int i = 0, n = 0;
					while (i<len)
					{
						while ((MusicName[i] != '\r') && (MusicName[i] != '\0'))
						{
							singleMusic[n] = MusicName[i];
							n++;
							i++;
						}
						singleMusic[n] = '\0';											
						i += 2;
						n = 0;
						SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)singleMusic);
					}
				}				
				GlobalUnlock(hMem);
				int ucode = 0xFEFF;
				WriteFile(hFile, &ucode, 2, &num, NULL);
				WriteFile(hFile, MusicName, len*2 + 1, &num, NULL);
				CloseHandle(hFile);
			}		
			CloseClipboard();
			
		}
			break;
		case IDM_EXIT:
			HWND hDlg = GetParent(hWnd);
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
	}
		break;
	case WM_MOUSEMOVE:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		break;

	case WM_RBUTTONDOWN:
	{
		
	}
		break;
	
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, &mySubClassProc, 1);
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

VOID SetOwner(HWND hWnd)
{
	TCHAR cName[4096];
	TCHAR cTitle[4096];
	HWND  hCBOwner = GetClipboardOwner();

	if (hCBOwner == hWnd)
		lstrcpyW(cName, L"Local");
	else
		lstrcpyW(cName, L"Another Application");

	swprintf_s(cTitle,4096, L"%s - [%s]", szWindowClass, cName);

	SetWindowText(hWnd, cTitle);
}

BOOL cue_txt_make(int *time, TCHAR **music, TCHAR *artist, TCHAR *album, TCHAR *fileName, int soundNum)
{
	TCHAR   CUE[2048] = L"PERFORMER \"", temp[255], apeName[255],cueName[255],songName[1024];
	HANDLE  hFile,hTxt;
	DWORD LastError;
	HANDLE hHeap;
	TCHAR *ItemData;
	char  ansi[255] ;
	int i = 0, len = 0, str_len = 0, minute = 0, second = 0, time_con = 0;
	int uni = 0xFEFF;
	DWORD num = 0;
	
	lstrcpynW(temp, fileName, lstrlenW(fileName) - 3);
	lstrcpyW(songName, temp);
	lstrcatW(songName, L"--歌曲名");
	lstrcatW(songName, L".txt");
	lstrcatW(temp, L".cue");
	lstrcpy(cueName, temp);
	hTxt = CreateFile(songName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hFile = CreateFile(temp, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	

	WriteFile(hTxt, &uni, 2, &num, NULL);//不需要UNICODE版
	lstrcatW(CUE, artist);
	lstrcatW(CUE, L"\"\r\n");//第一行艺术家
	lstrcatW(CUE, L"TITLE \"");
	lstrcatW(CUE, album);
	lstrcatW(CUE, L"\"\r\n");//第二行专辑名
	lstrcatW(CUE, L"FILE \"");
	len = GetCurrentDirectoryW(255, temp);
	lstrcpy(apeName, fileName + len + 1);
	lstrcatW(CUE, apeName);
	lstrcatW(CUE, L"\" WAVE\r\n");//第三行文件名

	str_len = lstrlenW(CUE);
	if (str_len < 4096)
	{
		//ansi = UnicodeToAnsi(CUE);
		WCharToMByte(CUE, ansi, str_len * 2);
	}
		WriteFile(hFile, ansi, lstrlenA(ansi) , &num, NULL);

	hHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024 * 4, 0);
	
	if (hHeap == NULL)
	{
		LastError = GetLastError();
		swprintf_s(CUE, TEXT("Failed to create a new heap with LastError %d.\n"), LastError);
		MessageBox(NULL, CUE, L"Memory Error", MB_OK);
		return 1;
	}
	if (HeapLock(hHeap) == FALSE)
	{
		LastError = GetLastError();
		swprintf_s(CUE, TEXT("Failed to lock heap with LastError %d.\n"), LastError);
		MessageBox(NULL, CUE, L"Memory Error", MB_OK);
		return 1;
	}
	

	for (int n = 0; n < soundNum ; n++)
	{
		TCHAR par[1024] ;
		time_con = *time / ONE_SECOND_DATA;
		time_con -= 2;
		ItemData = (TCHAR *)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 4096);
		swprintf_s(temp, 255, L"  TRACK %02d AUDIO\r\n", n + 1);//TRACK N AUDIO
		lstrcatW(ItemData, temp);//第一行轨道结束
		lstrcatW(ItemData, L"     TITLE \"");//TITLE "
		lstrcpyW(CUE, CueList[n]);           //CUE是歌曲名
		lstrcpyW(par, CueList[n]);
		lstrcatW(par, L"\r\n");
		WriteFile(hTxt, par, lstrlenW(par) * 2 , &num, NULL);
		//WriteFile(hTxt, par, 5, &num, NULL);
		lstrcatW(ItemData, CUE);             //连接成TRACK N AUDIO \r\n TITLE "歌曲
		lstrcatW(ItemData, L"\"\r\n");      //第二行歌曲，补上双引号
		if (n > 0)
		{
			minute = time_con / 60;
			second = time_con % 60 - 1;      //计算第一首歌的结束时间
		}
		if (second < 0)
		{
			second = 60 + second;           //不足0秒，进制借位
			minute--;
		}
		if (n == 0)
		{
			swprintf_s(temp, 255, L"     INDEX 01 %02d:%02d:00\r\n", 0, 0);//第一首歌的时间永远是00:00:00
		}
		else
		{
			swprintf_s(temp, 255, L"     INDEX 00 %02d:%02d:00\r\n", minute, second);//第一首歌的时间格式化输出INDEX MM:SS:00
		}
		lstrcatW(ItemData, temp);//连接上前面的字符串，第三行
		if (n > 0)
		{
			minute = time_con / 60;
			second = time_con % 60 + 1;
			if (second > 60)
			{
				second = second - 60;
				minute++;
			}

			swprintf_s(temp, 255, L"     INDEX 01 %02d:%02d:00\r\n", minute, second);//第二首歌开始时间
			lstrcatW(ItemData, temp);
		}
		str_len = lstrlenW(ItemData);
		if (str_len)
		{
			WCharToMByte(ItemData, ansi, str_len * 2);//UNICODE转成ANSI
		}

		WriteFile(hFile, ansi, lstrlenA(ansi), &num, NULL);
		if (n > 0)
			time++;
		HeapFree(hHeap, HEAP_NO_SERIALIZE, NULL);
		
	}
	
	CloseHandle(hFile);
	CloseHandle(hTxt);
	ShellExecuteW(NULL, L"open", L"notepad.exe", cueName, NULL, SW_SHOW);
	return TRUE;
}

char* UnicodeToAnsi(wchar_t *sourceString)
{
	char szAnsi[4096];
	int ansiLen = WideCharToMultiByte(CP_ACP, NULL, sourceString, wcslen(sourceString), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, sourceString, wcslen(sourceString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';
	return szAnsi;
}



BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (dwSize < dwMinSize)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return TRUE;
}
