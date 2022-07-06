#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>

uintptr_t FindDMAAddy(uintptr_t ptr, const std::vector<unsigned int>& offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int offset : offsets)
    {
        addr = *(uintptr_t*)addr;
        addr += offset;
    }
    return addr;
}

namespace offset
{
    constexpr std::ptrdiff_t localPlayer = 0x315420;
    std::vector<unsigned int> jumpStage = {0x5C};
}

void flyCheat(const HMODULE instance) {
    auto client = (DWORD)GetModuleHandleA("BattleBlockTheater.exe");
    const int flyValue = -1104959413;

    // if end is pressed the program exits
    while (!GetAsyncKeyState(VK_END))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // make sure space is pressed
        if (!GetAsyncKeyState(VK_SPACE))
            continue;

        // get local player
        DWORD localPlayer = client + offset::localPlayer; //calculate the full base address

        // check if local player is valid
        if (!localPlayer)
            continue;

        uintptr_t jumpStage = FindDMAAddy(localPlayer, offset::jumpStage);

        if (!jumpStage)
            continue;

        if (*(int*)jumpStage != flyValue)
            *(int*)jumpStage = flyValue;
    }
    FreeLibraryAndExitThread(instance, 0);
}

int __stdcall DllMain(
        const HMODULE instance,
        const std::uintptr_t reason,
        const void* reserved
)
{
    // DLL_PROCESS_ATTACH
    if (reason == 1) {
        DisableThreadLibraryCalls(instance);

        MessageBoxA(NULL, "Injected! Press end key to toggle and space to fly.", "Fly", MB_OK);

        // execute the fly cheat
        const auto thread = CreateThread(
                nullptr,
                0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(flyCheat),
                instance,
                0,
                nullptr
        );

        if (thread)
            CloseHandle(thread);
    }

    return 1;
}
