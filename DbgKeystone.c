#include <windows.h>

// Some code borrowed from the sample DbgEng style debugger extension

//
// Define KDEXT_64BIT to make all wdbgexts APIs recognize 64 bit addresses
// It is recommended for extensions to use 64 bit headers from wdbgexts so
// the extensions could support 64 bit targets.
//
#define KDEXT_64BIT
#include <wdbgexts.h>
#include "keystone.h"
#pragma comment (lib, "keystone.lib")

//
// globals
//
EXT_API_VERSION         ApiVersion = { 1, 0, EXT_API_VERSION_NUMBER64, 0 };
WINDBG_EXTENSION_APIS   ExtensionApis;
ULONG SavedMajorVersion;
ULONG SavedMinorVersion;

DllInit(HANDLE hModule, DWORD  dwReason, DWORD  dwReserved)
{
    switch (dwReason) {
    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;

    case DLL_PROCESS_ATTACH:
        break;
    }

    return TRUE;
}

__declspec(dllexport) VOID WinDbgExtensionDllInit(PWINDBG_EXTENSION_APIS lpExtensionApis, USHORT MajorVersion, USHORT MinorVersion)
{
    ExtensionApis = *lpExtensionApis;
    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;

    return;
}

__declspec(dllexport) LPEXT_API_VERSION ExtensionApiVersion()
{
    //
    // ExtensionApiVersion should return EXT_API_VERSION_NUMBER64 in order for APIs
    // to recognize 64 bit addresses.  KDEXT_64BIT also has to be defined before including
    // wdbgexts.h to get 64 bit headers for WINDBG_EXTENSION_APIS
    //
    return &ApiVersion;
}

//
// Routine called by debugger after load
//
__declspec(dllexport) VOID CheckVersion()
{
    return;
}

__declspec(dllexport) DECLARE_API(a)
{
    ULONG64 Address;
    SIZE_T  nCount;
    SIZE_T  nSize;
    SIZE_T  i;
    ULONG   cb;
    PVOID   pInput;
    ULONG   Value;
    PBYTE   outBytes;

    ks_engine* ks;
    ks_err err;

    if (GetExpressionEx(args, &Address, &args)) {
        Value = (ULONG)GetExpression(args);
    }
    else {
        dprintf("Usage: !a <address>\n");
        return;
    }

    pInput = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x1000);
    if (!pInput)
    {
        dprintf("Unable to allocate heap!\n");
        return;
    }
    else 
    {
        err = ks_open(KS_ARCH_X86, KS_MODE_64, &ks);
        if (err != KS_ERR_OK) {
            dprintf("ERROR: failed on ks_open(), quit\n");
            return;
        }

        while (GetInputLine("Keystone > ", (PSTR)pInput, 0x1000) > 1)
        {
            dprintf("%I64lx\n", Address);

            if (ks_asm(ks, pInput, 0, &outBytes, &nSize, &nCount) != KS_ERR_OK)
            {
                dprintf("ERROR: ks_asm() failed & count = %lu, error = %u\n", nCount, ks_errno(ks));
                break;
            }
            for (i = 0; i < nSize; i++) 
            {
                WriteMemory(Address + i, &outBytes[i], 1, &cb);
            }
            Address += nSize;
            ZeroMemory(pInput, 0x1000);
            // NOTE: free encode after usage to avoid leaking memory
            ks_free(outBytes);
        }
        // close Keystone instance when done
        ks_close(ks);
        HeapFree(GetProcessHeap(), 0, pInput);
    }
    return;
}
