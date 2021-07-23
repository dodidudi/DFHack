#pragma once
#include <windows.h>
#include <ddraw.h>

typedef HRESULT(WINAPI* _TrueGetAttachedSurface7)(
	GUID FAR* lpGUID, 
	LPDDSCAPS2 lpddscap,
	LPDIRECTDRAWSURFACE7 FAR* lpdds);

typedef HRESULT(__stdcall* _TrueCreateSurface7)(
	GUID FAR* lpGUID,
	LPDDSURFACEDESC2 lpddsd,
	LPDIRECTDRAWSURFACE7 FAR* lpdds,
	IUnknown FAR* pUnkOuter);

typedef HRESULT(WINAPI* _TrueDirectDrawCreateEx)(
	GUID FAR* lpGuid,
	LPVOID* lplpDD, 
	REFIID  iid, 
	IUnknown FAR* pUnkOuter);

typedef HRESULT(__stdcall* _TrueBlt7)(GUID FAR* lpGUID, 
	LPRECT lprc, 
	LPDIRECTDRAWSURFACE7 lpdds,
	LPRECT lprc1,
	DWORD n, 
	LPDDBLTFX n1);

HRESULT WINAPI HookDirectDrawCreateEx(GUID FAR* lpGuid, LPVOID* lplpDD, REFIID  iid, IUnknown FAR* pUnkOuter);