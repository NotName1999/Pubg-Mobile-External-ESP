#include "Functions.h"
#include "Overlay.h"
#include <vector>
#include <d3d9types.h>
#include <thread>
#include "offset.hpp"
#include "Tools.h"
#include "SDKGame.h"
D3DMATRIX ViewMatrix;
using namespace std;

namespace DirectX9Interface {
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}


void Draw() {
	char FpsInfo[64];
	sprintf(FpsInfo, "Overlay FPS: %0.f", ImGui::GetIO().Framerate);
	RGBA White = { 255,255,255,255 };
	DrawStrokeText(30, 44, &White, FpsInfo);
}

void thread_aa()
{
	while (true)
	{

		Data::Ue4Base = ReadMemoryEx<UINT>(Offset::PointerUe4);
		Data::Gnames = ReadMemoryEx<DWORD>(Data::Ue4Base + Offset::Gnames) + 0x88;
		Data::Gworld = ReadMemoryEx<DWORD>(Data::Ue4Base + Offset::Gword) + 0x3C;
		Data::ViewMatrixBase = Data::Ue4Base + Offset::VIEWMATRIX;
		DWORD temp1 = ReadMemoryEx<DWORD>(Data::ViewMatrixBase);
		DWORD temp2 = ReadMemoryEx<DWORD>(temp1 + 0x20);
		ViewMatrix = ReadMemoryEx<D3DMATRIX>(temp2 + 0x200);
		Data::Uworld = ReadMemoryEx<DWORD>(Data::Gworld);
	}
}

void ThreadESPMain()
{


	if (Data::Uworld != 0)
	{
		Data::PersistentLevel = ReadMemoryEx<DWORD>(Data::Uworld + Offset::PersistentLevel);
		std::cout << "[Debug] PersistentLevel: 0x" << std::hex << Data::PersistentLevel << std::dec << std::endl;

		Data::NetDriver = ReadMemoryEx<DWORD>(Data::Uworld + Offset::NetDriver);
		std::cout << "[Debug] NetDriver: 0x" << std::hex << Data::NetDriver << std::dec << std::endl;

		if (Data::NetDriver <= 0)
		{
			std::cout << "[Info] In lobby (NetDriver <= 0)" << std::endl;
			return;
		}

		if (Data::PersistentLevel != 0)
		{
			std::cout << "[Debug] Decrypting actor array..." << std::endl;
			auto ActorsPointerAddress = DecryptActorsArrayZakaria(Data::PersistentLevel, Offset::EntityList, Offset::ActorsClass);
			std::cout << "[Debug] ActorArray Pointer Addr: 0x" << std::hex << ActorsPointerAddress << std::dec << std::endl;

			DWORD ActorArray = ReadMemoryEx<uint32_t>(ActorsPointerAddress);
			int ActorArrayCount = ReadMemoryEx<int>(ActorsPointerAddress + 0x4);

			std::cout << "[Debug] ActorArray: 0x" << std::hex << ActorArray << std::dec << std::endl;
			std::cout << "[Debug] ActorArrayCount: " << ActorArrayCount << std::endl;

			if (ActorArrayCount <= 0)
			{
				std::cout << "[Warning] ActorArrayCount <= 0" << std::endl;
				return;
			}

			for (int i = 0; i < ActorArrayCount; i++)
			{
				DWORD EntityAddress = ReadMemoryEx<DWORD>(ActorArray + i * 4);
				if (!EntityAddress || EntityAddress == (DWORD)nullptr || EntityAddress == -1)
				{
					std::cout << "[Debug] Invalid EntityAddress at index " << i << std::endl;
					continue;
				}
			
				DWORD EntityStruct = ReadMemoryEx<DWORD>(EntityAddress + 0x10);
				if (EntityStruct == 0 || EntityStruct < 0)
				{
					std::cout << "[Debug] Invalid EntityStruct at index " << i << std::endl;
					continue;
				}
			
				 std::string EntityType = GetEntities(Data::Gnames, EntityStruct);
				if (EntityType.empty() || EntityType == " " || EntityType == "None")
				{
					std::cout << "[Debug] Skipped EntityType (empty/none) at index " << i << std::endl;
					continue;
				}
			
			    	//std::cout << "[Entity] Type: " << EntityType << std::endl;
			
				if (IsPlayer(EntityType))
				{
					cout << " TRUE TRUE TRUE " << endl;
					bool isDead = ReadMemoryEx<bool>(EntityAddress + Offset::isDead);
					if (isDead)
					{
						std::cout << "[Debug] Skipped dead player at index " << i << std::endl;
						continue;
					}
			
					Data::RootComponent = ReadMemoryEx<DWORD>(EntityAddress + Offset::RootComponent);
					VECTOR3 Postion = ReadMemoryEx<VECTOR3>(Data::RootComponent + Offset::Position);
					cout << "Postion: " << Postion.X << ", Y: " << Postion.Y << " Z: " << Postion.Z << " AT EntityAddress: " << EntityAddress << endl;
					

					
					VECTOR3 ThisScreen;
					cout << "Process::Width: " << Process::Width << endl;
					cout << "Process::Height" << Process::Height << endl;
					if (WorldToScreen(Postion, ThisScreen, ViewMatrix))
					{
						cout << "ThisScreen: " << ThisScreen.X << ", " << ThisScreen.Y;
						float height = 50.f; 
						float boxHeight = height / (ThisScreen.Z + 1); 

						float boxWidth = boxHeight * 0.5f;
						float yOffset = 50.f;

						ImVec2 topLeft(ThisScreen.X - boxWidth / 2, ThisScreen.Y - boxHeight + yOffset);
						ImVec2 bottomRight(ThisScreen.X + boxWidth / 2, ThisScreen.Y + yOffset);

						ImGui::GetBackgroundDrawList()->AddRect(topLeft,bottomRight,IM_COL32(0, 255, 0, 255),0.0f,0,2.0f);
			
					
					
					}
					else
					{
						std::cout << "[Debug] WorldToScreen failed at index " << i << std::endl;
					}
				}

			}
		}
	}
	else
	{
		std::cout << "[Error] UWorld is NULL" << std::endl;
	}
}


void Render() {
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	Draw();
	if (EnableESP)
	{
		ThreadESPMain();
	}
//	ImGui::GetIO().MouseDrawCursor = ShowMenu;
	if (ShowMenu)
	{
		ImGui::Begin(u8"Viet Hung DEV Trùm Copy Paste");
		ImGui::Checkbox("ESP", &EnableESP);

		ImGui::End();
	}
	//if (ShowMenu == true) {
	//	ImGui::ShowDemoWindow();
	//}
	ImGui::EndFrame();

	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop()
{
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9Interface::Message, OverlayWindow::Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == Process::Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);

		Process::Width = TempRect.right;
		Process::Height = TempRect.bottom;

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = Process::Hwnd;

		POINT TempPoint2;
		GetCursorPos(&TempPoint2);
		io.MousePos.x = TempPoint2.x - TempPoint.x;
		io.MousePos.y = TempPoint2.y - TempPoint.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process::WindowWidth = TempRect.right;
			Process::WindowHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = Process::WindowWidth;
			DirectX9Interface::pParams.BackBufferHeight = Process::WindowHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process::WindowWidth, Process::WindowHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWindow::Hwnd);
	UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process::WindowWidth;
	Params.BackBufferHeight = Process::WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&OverlayWindow::WindowClass);
	if (Process::Hwnd){
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process::WindowWidth = TempRect.right;
		Process::WindowHeight = TempRect.bottom;
	}

	OverlayWindow::Hwnd = CreateWindowEx(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, Process::WindowLeft, Process::WindowTop, Process::WindowWidth, Process::WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

DWORD WINAPI ProcessCheck(LPVOID lpParameter) {
	while (true) {
		if (Process::Hwnd != NULL) {
			if (GetProcessId(TargetProcess) == 0) {
				exit(0);
			}
		}
	}
}


int main() 
{
	DWORD pidgame = GetTrueProcessId("aow_exe.exe");

	if (pidgame != 0)
	{
		cout << "pid game: " << pidgame << endl;
		Process::GameHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidgame);
		setupchitchit(Process::GameHandle);
		if (Process::GameHandle)
		{
			cout << "Open Memory Sucess" << endl; 
		}
		else
		{
			goto sigmaboy;
		}
	}
	else
	{
	sigmaboy:
		cout << "failed find||open mem" << endl;
	}

	if (CreateConsole == false)
		ShowWindow(GetConsoleWindow(), SW_HIDE); 

	bool WindowFocus = false;
	while (WindowFocus == false) {
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (GetProcessId(TargetProcess) == ForegroundWindowProcessID) {
			Process::ID = GetCurrentProcessId();
			Process::Handle = GetCurrentProcess();
			Process::Hwnd = GetForegroundWindow();

			RECT TempRect;
			GetWindowRect(Process::Hwnd, &TempRect);
			Process::WindowWidth = TempRect.right - TempRect.left;
			Process::WindowHeight = TempRect.bottom - TempRect.top;
			Process::WindowLeft = TempRect.left;
			Process::WindowRight = TempRect.right;
			Process::WindowTop = TempRect.top;
			Process::WindowBottom = TempRect.bottom;

			char TempTitle[MAX_PATH];
			GetWindowText(Process::Hwnd, TempTitle, sizeof(TempTitle));
			Process::Title = TempTitle;

			char TempClassName[MAX_PATH];
			GetClassName(Process::Hwnd, TempClassName, sizeof(TempClassName));
			Process::ClassName = TempClassName;

			char TempPath[MAX_PATH];
			GetModuleFileNameEx(Process::Handle, NULL, TempPath, sizeof(TempPath));
			Process::Path = TempPath;

			WindowFocus = true;
		}
	}

	OverlayWindow::Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	thread t(thread_aa);
	t.detach();
	while (TRUE) {
		MainLoop();
	}
}