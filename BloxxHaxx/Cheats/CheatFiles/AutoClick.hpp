#pragma once
#include <Windows.h>
#include <string>
#include <iostream>

bool AutoClick_Enabled = false;
int cps = 20;

void AutoClick_Cheat() {
    while (true) {
        // Exit if auto-clicking is disabled
        if (!AutoClick_Enabled) {
            ExitThread(0);
        }

        // Check if the left mouse button is held down
        if (!GetAsyncKeyState(VK_LBUTTON)) {
            ExitThread(0);
        }
            float delay = (1000 / cps) / 2;
            Sleep(delay);

            // Perform the click
            INPUT Input = { 0 };

            Input.type = INPUT_MOUSE;
            Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &Input, sizeof(INPUT));

            ZeroMemory(&Input, sizeof(INPUT));
            Input.type = INPUT_MOUSE;
            Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &Input, sizeof(INPUT));
        }
               // If the button is not pressed, you can introduce a small sleep to avoid busy-waiting

        
    }

