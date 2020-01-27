// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commdlg.h>
#include <commctrl.h>
#include <shellapi.h>
#include "All.h"
//#include <stdio.h>
#include "GlobalFunctions.h"
#include "MACLib.h"
#include "CharacterHelper.h"
using namespace APE;
#pragma warning(disable:4996)
#pragma comment(lib,"H:\\MAC_SDK_512\\Source\\Projects\\VS2019\\MACLib\\Debug\\MACLib.lib")
#define VOLUMEMAX   32767


// defines
#define COMPRESS_MODE          0
#define DECOMPRESS_MODE        1
#define VERIFY_MODE            2
#define CONVERT_MODE           3
#define UNDEFINED_MODE        -1
struct MusicInfo 
{
	TCHAR Artist[255];
	TCHAR Album[255];
	int  trackNum;
};
struct TIME_STRUCT
{
	int *Array;
	int itemNum;
};
//RIFF头的宏定义缩写，便于后面的比较 低字节-》高字节
#define ID_RIFF mmioFOURCC('R', 'I', 'F', 'F')
#define ID_WAVE mmioFOURCC('W', 'A', 'V', 'E')
#define ID_data mmioFOURCC('d', 'a', 't', 'a')
#define ID_fmt  mmioFOURCC('f', 'm', 't', '\x20')
#define ID_fact mmioFOURCC('f', 'a', 'c', 't')

//WAVE文件一般有四种块，它们依次是：RIFF块、格式块、附加块（可选），数据块
struct RIFFFORMAT {//长度12
	unsigned long Ckid;	//RIFF标识
	unsigned long FileSize; //文件大小
	unsigned long FccType; //WAVE标志
};

//WAV音频头
struct WAVE_FORMAT
{
	unsigned long Ckid; //fmt
	unsigned long CkSize; //块大小
	unsigned short wFormatTag;//音频格式一般为WAVE_FORMAT_PCM
	unsigned short nChannels;//采样声道数
	unsigned long nSamplesPerSec;//采样率
	unsigned long nAvgBytesPerSec;//每秒字节数  采样率*采样精度/8(字节位数)
	unsigned short nBlockAlign;//块大小 采样字节*声道数
	unsigned short wBitsPerSample;//采样精度 采样精度/8 = 采样字节
	unsigned short cbSize;		//预留字节 一般为0扩展域有的没有

};
//wave数据块
struct WAVE_DATA {
	unsigned long Wdid; //data 标志
	unsigned long WdSize; //块大小
	unsigned char* Wdbuf; //数据指针 有符号
};


#ifndef PLATFORM_WINDOWS
#define _tmain(argc, argv) main(argc, argv)
#define _tcscpy_s(dst, num, src) strcpy(dst, src)
#define _tcsncpy_s(dst, num, src, count) strncpy(dst, src, count)
#define _tcsnicmp(str1, str2, count) strncasecmp(str1, str2, count)
#define _ftprintf fprintf
#define _ttoi(str) atoi(str)
#ifdef _T
#undef _T
#endif
#define TCHAR char
#define _T(x) x
#endif

// global variables
TICK_COUNT_TYPE g_nInitialTickCount = 0;

#ifdef PLATFORM_WINDOWS
static BOOL CALLBACK CtrlHandlerCallback(DWORD dwCtrlTyp);
#endif
static void DisplayProperUsage(FILE * pFile);
static void CALLBACK ProgressCallback(int nPercentageDone);
