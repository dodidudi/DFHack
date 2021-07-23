#pragma once
typedef struct _WARP_POINT
{
	DWORD a;
	DWORD b;
	DWORD c;
	DWORD d;
	DWORD e;
	char dest[255];
} WARP_POINT;

typedef struct _MAP
{
	WARP_POINT* warp;
	int count;
	char name[255];
} MAP;