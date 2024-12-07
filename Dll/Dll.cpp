#include <iostream>
#include <windows.h>
#include <string>

// basically start this with arguments "injector.exe <ProcessId> <DllPath>"
// This Was made by @privateuserttv (discord), Please give credits to https://github.com/PrivateUserSource (or this repository) when using this.

bool InjectDLL(DWORD processID, const wchar_t* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processID);
    if (!hProcess) {
        std::cerr << "Could not open process: " << GetLastError() << std::endl;
        return false;
    }

    SIZE_T dllPathSize = (wcslen(dllPath) + 1) * sizeof(wchar_t);
    void* pRemoteMemory = VirtualAllocEx(hProcess, nullptr, dllPathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!pRemoteMemory) {
        std::cerr << "Could not allocate memory in target process: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, pRemoteMemory, (void*)dllPath, dllPathSize, nullptr)) {
        std::cerr << "Could not write to process memory: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        std::cerr << "Could not get handle for kernel32.dll: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    FARPROC loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryW");
    if (!loadLibraryAddr) {
        std::cerr << "Could not get address for LoadLibraryW: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, pRemoteMemory, 0, nullptr);
    if (!hThread) {
        std::cerr << "Could not create remote thread: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return true;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: injector <process_id> <dll_path>" << std::endl;
        return 1;
    }

    DWORD processID = std::stoul(argv[1]);
    const wchar_t* dllPath = argv[2];

    if (InjectDLL(processID, dllPath)) {
        std::wcout << L"DLL injected successfully!" << std::endl;
    }
    else {
        std::wcerr << L"DLL injection failed!" << std::endl;
    }

    return 0;
}
