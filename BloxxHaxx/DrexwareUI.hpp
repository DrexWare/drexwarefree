#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#include <vector>
#include <map>
#include <fstream>
#include <tchar.h>
#include <sstream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <d3d11.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>

#include "MinHook/MinHook.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl2.h"
#include "ImGUI/imgui_impl_win32.h"

#include "FW1FontWrapper\FW1FontWrapper.h"


#include "Cheats/CheatIncludes.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")


IFW1Factory* Bloxx_FW1FontFactory;
IFW1FontWrapper* Bloxx_FW1FontWrapper;

ID3D11Device* D3D11Device;
ID3D11DeviceContext* D3D11Context;
ID3D11Texture2D* D3D11TextureTarget;
ID3D11RenderTargetView* D3D11RenderView;
IDXGISwapChain* D3D11SwapChain;

DWORD_PTR* D3D11SwapChain_VTABLE;
DWORD_PTR* D3D11Device_VTABLE;
DWORD_PTR* D3D11Context_VTABLE;



bool UIIsCurrentlyOpen;

std::hash<std::string> HashString;

HMODULE DrexwareMod;

HGLRC LastRC = NULL;
std::map<int, HGLRC> PixelFormatLut;

void(*Orig_SwapBuffers)(HDC dc) = nullptr;

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

long ReturnRGBValue(int iR, int iG, int iB, int iA) {

	return ((iA * 256 + iR) * 256 + iG) * 256 + iB;
}

#define RGB_GET(iR, iG, iB, iA) (ReturnRGBValue(iR, iG, iB, iA))
#define RGB_GET(iR, iG, iB) (ReturnRGBValue(iR, iG, iB, 255))




void OpenGLDraw(RECT Rect) {

	auto& IO = ImGui::GetIO();
	IO.DisplaySize.x = Rect.right - Rect.left;
	IO.DisplaySize.y = Rect.bottom - Rect.top;

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplOpenGL2_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 100, 100 });

	if (ImGui::Begin("Drexware - Developed By Drexxy")) {
		ImGui::SetWindowSize(ImVec2(545, 351));

		ImGui::Text("All Cheats Are Listed Below: ");

		ImGui::Checkbox("Enable Autoclicker", &AutoClick_Enabled);














	}

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Render();
	ImGui::EndFrame();
	glClear(256);
	glMatrixMode(5889);
	glOrtho(0.0f, (int)IO.DisplaySize.x, (int)IO.DisplaySize.y, 0.0f, 0.0f, 100.0f);
	glViewport(0, 0, (int)IO.DisplaySize.x, (int)IO.DisplaySize.y);

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}


static void SwapOpenGLBuffers(HDC DC) {
	if (!DC) {
		return Orig_SwapBuffers(DC);
	}

	if (UIIsCurrentlyOpen) {
		HWND const WND = WindowFromDC(DC);
		RECT WNDRect;
		GetClientRect(WND, &WNDRect);

		OpenGLDraw(WNDRect);
	}

	return Orig_SwapBuffers(DC);
}



typedef LRESULT(CALLBACK*OWindowProc) (
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
OWindowProc WNDProc = nullptr;

bool TabBackAutoClick;

static LRESULT WINAPI HWNDProcHandle(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	DWORD Proc;
	auto& IO = ImGui::GetIO();

	if (UIIsCurrentlyOpen) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam)) {
			return TRUE;
		}
	}

	switch (Msg) {
	case WM_KEYUP: {
		if (wParam == VK_INSERT)
		{
			UIIsCurrentlyOpen =! UIIsCurrentlyOpen;
			if (UIIsCurrentlyOpen) {
				TabBackAutoClick = true;
				ReleaseCapture();
			}
			if (!UIIsCurrentlyOpen) {
				if (TabBackAutoClick) {
					//use this to auto click back into game upon UI closing so you can look around again without having to click
					INPUT Input = { 0 };

					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					SendInput(1, &Input, sizeof(INPUT));

					ZeroMemory(&Input, sizeof(INPUT));
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					SendInput(1, &Input, sizeof(INPUT));

					TabBackAutoClick = false;
				}
			}
		}

			if (AutoClick_Enabled && GetAsyncKeyState(VK_LBUTTON)) {
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AutoClick_Cheat, 0, 0, 0);
			
		}
	}
	case WM_SETCURSOR:
	{
		if (UIIsCurrentlyOpen) {
			return 0;
		}
	}
	}
ret:
	return CallWindowProc(WNDProc, hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }



void InitGUI() {
	ImGui::CreateContext();
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	auto MineCraftWindow = FindWindow(_T("LWJGL"), NULL);
	ImGui_ImplWin32_Init(MineCraftWindow);
	ImGui_ImplOpenGL2_Init();


	WNDProc = (OWindowProc)SetWindowLongPtr(MineCraftWindow, GWLP_WNDPROC, (LONG_PTR)HWNDProcHandle);

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	if (MH_Initialize() != MH_OK) {
		MessageBox(NULL, _T("Unable To Initiate Drexware UI! Error Code: 1"), _T("Fatal Error!"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	if (MH_CreateHookApiEx(L"Gdi32.dll", "SwapBuffers", &SwapOpenGLBuffers, &Orig_SwapBuffers) != MH_OK) {
		MessageBox(NULL, _T("Unable To Initiate Drexware UI! Error Code: 2"), _T("Fatal Error!"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	if (MH_EnableHook(MH_ALL_HOOKS)) {
		MessageBox(NULL, _T("Unable To Initiate Drexware UI! Error Code: 3"), _T("Fatal Error!"), MB_OK | MB_ICONERROR);
		exit(0);
	}
}