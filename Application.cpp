#include "Application.h"

#include <cassert>


#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Application* app = nullptr;

		switch (message)
		{
		case WM_CREATE:
			(void)0;
			{
				auto data = reinterpret_cast<CREATESTRUCT*>(lParam);
				auto app = reinterpret_cast<Application*>(data->lpCreateParams);
				assert(nullptr != app);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
			}
			break;
		default:
			break;
		}

		app = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (nullptr != app)
		{
			return app->wnd_proc(hWnd, message, wParam, lParam);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

int32_t Application::init()
{
	int32_t ret = create_window(1600, 900);
	if (ret)
		return ret;

	ret = init_direct3d();
	if (ret)
		return ret;

	ret = init_assets();
	if (ret)
		return ret;

	init_timer();

	running = true;
	return 0;
}

int32_t	Application::run()
{
	MSG msg;
	while (running)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		on_tick();
	}

	cleanup_assets();

	return cleanup();
}

LRESULT Application::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool imgui_processed = ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		running = false;
		break;
	case WM_SIZE:
		on_resize(static_cast<uint32_t>(lParam & 0xffffu), static_cast<uint32_t>(lParam >> 16u));
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int32_t Application::create_window(uint32_t width, uint32_t height)
{
	static wchar_t class_name[] = L"ModelViewerWindow";

	HINSTANCE hInstance = GetModuleHandle(nullptr);

	WNDCLASS wc = {};
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = class_name;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wc))
		return -1;

	RECT r = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) }, screenRect = {};
	if (!AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE))
		return -2;

	GetWindowRect(GetDesktopWindow(), &screenRect);

	hWnd = CreateWindow(class_name, L"Model Viewer", WS_OVERLAPPEDWINDOW,
		((screenRect.right - screenRect.left) - (r.right - r.left)) / 2,
		((screenRect.bottom - screenRect.top) - (r.bottom - r.top)) / 2,
		r.right - r.left,
		r.bottom - r.top,
		nullptr, nullptr, hInstance, this);

	if (nullptr == hWnd)
		return -3;

	bufferWidth = width;
	bufferHeight = height;

	return 0;
}

int32_t Application::init_direct3d()
{
	int32_t err = -1;

	IDXGIFactory2* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;

	HRESULT ret = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	if (ret != S_OK)
		return -1;

	{
		uint32_t i = 0;
		while (S_OK == factory->EnumAdapters1(i++, &adapter))
		{
			DXGI_ADAPTER_DESC1 desc = {};
			if (S_OK != adapter->GetDesc1(&desc))
				return -1;

			if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
				break;

			adapter->Release();
			adapter = nullptr;
		}
	}


	do
	{
		if (nullptr == adapter)
			break;

		UINT creationFlags = 0;
#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

		HRESULT ret = D3D11CreateDevice(
			adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			creationFlags,
			featureLevels,
			2,
			D3D11_SDK_VERSION,
			&device,
			nullptr,
			&context);

		if (S_OK != ret)
			break;

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = bufferWidth;
		desc.Height = bufferHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		//desc.Scaling = DXGI_SCALING_NONE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		ret = factory->CreateSwapChainForHwnd(device, hWnd, &desc, nullptr, nullptr, &swapChain);

		if (S_OK != ret)
			break;

		{
			ID3D11Texture2D* backBuffer = nullptr;
			if (S_OK != swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))
				break;

			if (S_OK != device->CreateRenderTargetView(backBuffer, nullptr, &rtv))
				break;

			backBuffer->Release();
		}

		context->OMSetRenderTargets(1, &rtv, nullptr);

		// now we can trigger WM_SIZE message
		ShowWindow(hWnd, SW_SHOW);

		if (!ImGui_ImplDX11_Init(hWnd, device, context))
			break;

		err = 0;
	} while (0);

	if (nullptr != factory) factory->Release();
	if (nullptr != adapter) adapter->Release();

	return err;
}

int32_t Application::cleanup()
{
	ImGui_ImplDX11_Shutdown();

	rtv->Release();
	swapChain->Release();
	context->Release();
	device->Release();

	return 0;
}

void Application::on_resize(uint32_t width, uint32_t height)
{
	if (width == bufferWidth && height == bufferHeight)
		return;

	context->ClearState();
	rtv->Release();

	assert(S_OK == swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	ID3D11Texture2D* backBuffer = nullptr;
	assert(S_OK == swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	assert(S_OK == device->CreateRenderTargetView(backBuffer, nullptr, &rtv));

	backBuffer->Release();

	bufferWidth = width;
	bufferHeight = height;
}

void Application::init_timer()
{
	LARGE_INTEGER i;
	QueryPerformanceFrequency(&i);
	this->freq = i.QuadPart;

	QueryPerformanceCounter(&i);
	startTime = i.QuadPart;
	prevTime = startTime;
}

void Application::on_tick()
{
	{
		LARGE_INTEGER i;
		QueryPerformanceCounter(&i);
		totalTime = (float)(i.QuadPart - startTime) / freq;
		deltaTime = (float)(i.QuadPart - prevTime) / freq;
		prevTime = i.QuadPart;
	}

	ImGui_ImplDX11_NewFrame();

	update();

	float color[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	context->ClearRenderTargetView(rtv, color);

	render();

	ImGui::Render();
	swapChain->Present(0, 0);
}