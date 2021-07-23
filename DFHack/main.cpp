#include <windows.h>
#include <shellapi.h>
#include <locale.h>
#include "detours\detours.h"
#include "log.h"
#include "main.h"
#include "ddrawhook.h"


extern _TrueDirectDrawCreateEx TrueDirectDrawCreateEx;

MAP* map;

BOOL bSpeedHack;
BOOL bTeleportHack;
BOOL bView;
BOOL bHackDesc;
BOOL bazmo;
BOOL defmacro;
BOOL attmacro;

int nSpeed;
int nMapIndex;
int nMapCount;
int nWarpPosition;

void PrintLogo()
{
	LOG(14, "뎁스판타지아 핵\r\n");
	LOG(12, "(원본)https://github.com/codetronik\r\n");
	LOG(12, "(수정)https://github.com/codetronik\r\n");
	LOG(13, "F1 - 설명 온 오프 시 하단 문구 함께 온오프\r\n");
	LOG(13, "F2 - 스피드핵 자동속도 조절(기존F7누를필요X)\r\n");
	LOG(13, "F8 - 섬 전생루트 던전들 오토등반 단축키 추가\r\n");
	LOG(13, "   - ex)말듀크 입장 후 좌표 맞춘 후 F8\r\n");
	LOG(13, "F7 - 섬 전생 오토등반 시 왕성 바즈모 설정\r\n");
	LOG(13, "F11 / F12 - 기존 한방향에서 양방향으로 좌표 변경가능\r\n");
	LOG(13, "pause / scroll lock - 간단하게 매크로가 필요할때 사용\r\n");
	LOG(13, "매크로 종료는 END 키\r\n");
	

}
void SetPoint(char* szFilePath);

void LoadFile()
{
	nMapCount = 0;
	map = (MAP*)malloc(sizeof(MAP) * 50);
	for (int i = 0; i < 50; i++)
	{
		map[i].warp = (WARP_POINT*)malloc(sizeof(WARP_POINT) * 500);
		ZeroMemory(map[i].warp, sizeof(WARP_POINT) * 500);
	}
		
	BOOL bResult = TRUE;
	WIN32_FIND_DATA w32FindData = { 0, };
	char szCurrentDirectory[MAX_PATH] = { 0, };
	GetCurrentDirectoryA(MAX_PATH, szCurrentDirectory);
	strcat(szCurrentDirectory, "\\warp\\*.txt");
	
	HANDLE hFile = FindFirstFile(szCurrentDirectory, &w32FindData);

	while (bResult)
	{
		// 디렉토리라면,  
		if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			
		}
		else
		{			
			char szFilePath[MAX_PATH] = { 0, };
			GetCurrentDirectoryA(MAX_PATH, szCurrentDirectory);
			strcat(szFilePath, szCurrentDirectory);
			strcat(szFilePath, "\\warp\\");
			strcat(szFilePath, w32FindData.cFileName);

			char imsi[255] = { 0, };
			strcpy_s(imsi, 255, w32FindData.cFileName);
			char* p;
			if (p = strstr(imsi, ".txt"))
			{			
				*p = 0x00;
			}
			
			strcpy_s(map[nMapCount].name, 255, imsi);
			
			SetPoint(szFilePath);					
		}

		bResult = FindNextFile(hFile, &w32FindData);
	}
	CloseHandle(hFile);
}

void SetPoint(char* szFilePath)
{
	int nWarpTotal = 0;

	FILE* fp = fopen(szFilePath, "r");
	if (!fp)
	{		
		return;
	}

	char buf[512] = { 0, };
	while (!feof(fp))
	{
		fgets(buf, sizeof(buf), fp);
		if (strstr(buf, "$$$$")) continue;
		if (strstr(buf, "%%%%")) break;
		if (strstr(buf, "#")) continue;
		// 주석처리
		char* comment = strstr(buf, " //");
		if (comment)
		{
			comment = 0x0;
		}

		char* context = NULL;
		char* token = strtok_s(buf, "=", &context);
		char* token2 = strtok_s(NULL, "=", &context);
		char* token3 = strtok_s(NULL, "=", &context);
		char* token4 = strtok_s(NULL, "=", &context);
		char* token5 = strtok_s(NULL, "=", &context);
		char* token6 = strtok_s(NULL, "=", &context);
	
		token6[strlen(token6) - 1] = 0; // \n 제거
		map[nMapCount].warp[nWarpTotal].a = strtoul(token, NULL, 16);
		map[nMapCount].warp[nWarpTotal].b = strtoul(token2, NULL, 16);
		map[nMapCount].warp[nWarpTotal].c = strtoul(token3, NULL, 16);
		map[nMapCount].warp[nWarpTotal].d = strtoul(token4, NULL, 16);
		map[nMapCount].warp[nWarpTotal].e = strtoul(token5, NULL, 16);
		
		strcpy_s(map[nMapCount].warp[nWarpTotal].dest, 255, token6);

		/*
		LOG(13, "%d %d %d %d %d : %s\r\n", 
			map[nMapCount].warp[nWarpTotal].a, map[nMapCount].warp[nWarpTotal].b,
			map[nMapCount].warp[nWarpTotal].c, map[nMapCount].warp[nWarpTotal].d,
			map[nMapCount].warp[nWarpTotal].e, map[nMapCount].warp[nWarpTotal].dest
		);
		*/
		nWarpTotal++;
	}
	fclose(fp);
	
	map[nMapCount].count = nWarpTotal;

	nMapCount++;
}

void _stdcall Teleport(DWORD* a, DWORD* b)
{
	DWORD* c = (DWORD*)0x6409C0;
	DWORD* d = (DWORD*)0x6409C4;
	DWORD* e = (DWORD*)0x6409D0;
	if (bView == TRUE)
	{
		char temp[100] = { 0, };
		sprintf_s(temp, 100, "%x=%x=%x=%x=%x\r\n", *a, *b, *c, *d, *e);
		HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, strlen(temp) + 1);
		memcpy(GlobalLock(handle), temp, strlen(temp));
		GlobalUnlock(handle);
		OpenClipboard(NULL);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, handle);
		CloseClipboard();
	}
	if (TRUE == bTeleportHack)
	{	
		
		LOG(12, "teleport to %x %x %x %x %x\r\n", map[nMapIndex].warp[nWarpPosition].a,
			map[nMapIndex].warp[nWarpPosition].b,
			map[nMapIndex].warp[nWarpPosition].c,
			map[nMapIndex].warp[nWarpPosition].d,
			map[nMapIndex].warp[nWarpPosition].e
		);
		*a = map[nMapIndex].warp[nWarpPosition].a;
		*b = map[nMapIndex].warp[nWarpPosition].b;
		*c = map[nMapIndex].warp[nWarpPosition].c;
		*d = map[nMapIndex].warp[nWarpPosition].d;
		*e = map[nMapIndex].warp[nWarpPosition].e;
		
		bTeleportHack = FALSE;
	}
}

DWORD* g_x;
DWORD* g_y;
DWORD g_esi;
DWORD g_ebx;

PDETOUR_TRAMPOLINE pTrampoline = NULL;
__declspec(naked) void HOOK_Teleport()
{
	__asm {
		pushad
		pushfd
			
		mov g_esi, esi;
		mov g_ebx, ebx;
	}
	g_x = (DWORD*)((DWORD)g_esi + 0x218);
	g_y = (DWORD*)g_ebx;

	__asm
	{
		push g_x
		push g_y
		call Teleport
		popfd
		popad
		jmp[pTrampoline]
	}
}

// 스피드핵
typedef int(__cdecl* _S_55910)(int a1, int a2, int a3, float a4);
_S_55910 S_55910;
int __cdecl Hook_S_55910(int a1, int a2, int a3, float a4)
{
	if (TRUE == bSpeedHack)
	{
		a4 = a4 + nSpeed; // 스핵 이동거리

	}
	return S_55910(a1, a2, a3, a4);
}
/*
F2 스핵 온오프
F3 이동좌표 복사 온오프
F7 바즈모 온 오프로 변경
F8 등반 시작
F9 등반 정지
scroollock 매크로 방어
pause 매크로 통타
end 매크로 정지
F1 설명 온오프
F11좌표 파일 변경 증가하는방향만
F12 좌표 감소하는 방향
페이지업 페이지다운 딜리트
*/


void Key()
{
	while (1)
	{
		if (GetAsyncKeyState(VK_F1) & 0x0001)
		{
			if (bHackDesc == TRUE)
			{
				bHackDesc = FALSE;
			}
			else
			{
				bHackDesc = TRUE;
			}
		}
		if (GetAsyncKeyState(VK_F2) & 0x0001)
		{
			nSpeed = 0; // 속도 초기화
			if (bSpeedHack == TRUE)
			{				
				bSpeedHack = FALSE;				
			}
			else
			{				
				bSpeedHack = TRUE;
				nSpeed = nSpeed + 20; // 스피드핵 속도 증가
			}
		}
		if (GetAsyncKeyState(VK_F7) & 0x0001)	
		{			
			if (bazmo == TRUE)
			{
				bazmo = FALSE;

			}
			else
			{
				bazmo = TRUE;				
			}
		}	

		//0x32306466 말듀크
		//0x33306466 나가
		//0x34306466 비아레스
		//0x35306466 빙궁
		//0x36306466 별궁
		//0x37306466 왕성
		if (GetAsyncKeyState(VK_F8) & 0x0001)
		{
			DWORD a = 0x814514;
			DWORD* map_address;
			map_address = &a;
			if (*(DWORD*)*map_address == 0x32306466)
			{
				ShellExecute(NULL, "open", "hack\\autoclimb\\1.exe", NULL, NULL, NULL);
			}
			if (*(DWORD*)*map_address == 0x33306466)
			{
				ShellExecute(NULL, "open", "hack\\autoclimb\\2.exe", NULL, NULL, NULL);
			}
			if (*(DWORD*)*map_address == 0x34306466)
			{
				ShellExecute(NULL, "open", "hack\\autoclimb\\3.exe", NULL, NULL, NULL);
			}
			if (*(DWORD*)*map_address == 0x35306466)
			{
				ShellExecute(NULL, "open", "hack\\autoclimb\\4.exe", NULL, NULL, NULL);
			}
			if (*(DWORD*)*map_address == 0x36306466)
			{
				ShellExecute(NULL, "open", "hack\\autoclimb\\5.exe", NULL, NULL, NULL);
			}
			if (*(DWORD*)*map_address == 0x37306466)
			{
				if (bazmo == TRUE)
				{
					ShellExecute(NULL, "open", "hack\\autoclimb\\7.exe", NULL, NULL, NULL);
				}
				else
				{
					ShellExecute(NULL, "open", "hack\\autoclimb\\6.exe", NULL, NULL, NULL);
				}
			}
		}

		if (GetAsyncKeyState(VK_F11) & 0x0001)
		{
			if (nMapIndex == nMapCount -1)
			{
				nMapIndex = 0;
			}
			else
			{
				nMapIndex++;
			}
			nWarpPosition = -1;
		}

		if (GetAsyncKeyState(VK_F12) & 0x0001)
		{
			if (nMapIndex == 0)
			{
				nMapIndex = nMapCount - 1;
			}
			else
			{
				nMapIndex--;
			}
			nWarpPosition = -1;
		}

		if (GetAsyncKeyState(VK_DELETE) & 0x0001)
		{
			bTeleportHack = FALSE;
		}
		if (GetAsyncKeyState(VK_F3) & 0x0001)
		{
			if (bView == TRUE)
			{				
				bView = FALSE;
			}
			else
			{
			
				bView = TRUE;
			}
		}
		
		if (GetAsyncKeyState(VK_PRIOR) & 0x0001)
		{				
			if (nWarpPosition < map[nMapIndex].count - 1)
			{
				nWarpPosition++;
			}
		
			bTeleportHack = TRUE;
			
		}
		if (GetAsyncKeyState(VK_NEXT) & 0x0001)
		{		
			if (nWarpPosition >= 0)
			{
				nWarpPosition--;
				
				if (nWarpPosition == -1)
					bTeleportHack = FALSE;
				else
				{
					bTeleportHack = TRUE;
				}
			}		
		}

		if (GetAsyncKeyState(VK_PAUSE) & 0x0001)
		{
			if (attmacro == FALSE)
			{
				if (defmacro == FALSE)
				{
					attmacro = TRUE;
					ShellExecute(NULL, "open", "hack\\macro\\attmacro.exe", NULL, NULL, NULL);
				}				
			}
			else
			{
				//attmacro = FALSE;
			}
		}
		
		if (GetAsyncKeyState(VK_SCROLL) & 0x0001)
		{
			if (defmacro == FALSE)
			{
				if (attmacro == FALSE)
				{
					defmacro = TRUE;
					ShellExecute(NULL, "open", "hack\\macro\\defmacro.exe", NULL, NULL, NULL);
				}
			}
			else
			{
				//defmacro = FALSE;
			}
		}
		if (GetAsyncKeyState(VK_F10) & 0x0001)
		{
			if (defmacro || attmacro)
			{
				defmacro = FALSE;
				attmacro = FALSE;
			}

		}


	}
}

void Hooking()
{
	if (DetourIsHelperProcess())
	{
		return;
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	HMODULE hMod = LoadLibrary("ddraw.dll");

	TrueDirectDrawCreateEx = (_TrueDirectDrawCreateEx)GetProcAddress(hMod, "DirectDrawCreateEx");
	DetourAttach(&(PVOID&)TrueDirectDrawCreateEx, HookDirectDrawCreateEx);
	
	S_55910 = (_S_55910)(DWORD)0x455910;
	DetourAttach(&(PVOID&)S_55910, Hook_S_55910); // 스피드핵

	DWORD dwTeleport = 0x418F56;
	DetourAttachEx(&(PVOID&)dwTeleport, HOOK_Teleport, &pTrampoline, NULL, NULL); // 순간이동 핵

	DetourTransactionCommit();
}
void Start()
{
	HMODULE hMod = NULL;

	
	if (AllocConsole())
	{
		FILE* stream = NULL;
		freopen_s(&stream, "CONOUT$", "w", stdout);

		SetConsoleTitle("Hack Log Console");
		setlocale(LC_ALL, "korean");
	}
	
	LOG(7, "process id - %d(%x)\n", GetCurrentProcessId(), GetCurrentProcessId());
	PrintLogo();
	Hooking();
	DWORD dwThread = 0;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Key, 0, 0, &dwThread);

	bSpeedHack = FALSE;
	bTeleportHack = FALSE;
	bView = FALSE;
	bHackDesc = TRUE;
	bazmo = FALSE;
	defmacro = FALSE;
	attmacro = FALSE;
	nWarpPosition = -1;

	LoadFile();

}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DWORD dwThread;
		HANDLE hDlgThread = 0;
		hDlgThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, 0, 0, &dwThread);

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}