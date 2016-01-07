#include <windows.h>
#include <clocale>

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
		break;

    case DLL_THREAD_ATTACH:
		break;

    case DLL_THREAD_DETACH:
		break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
