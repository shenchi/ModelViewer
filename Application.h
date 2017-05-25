#pragma once
#include <d3d11_1.h>
#include <cstdint>
#include <Windows.h>

class Application
{
public:
	int32_t init();

	int32_t run();

public:
	LRESULT wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	bool			running;
	HWND			hWnd;
	uint32_t		bufferWidth;
	uint32_t		bufferHeight;

private:
	int32_t create_window(uint32_t width, uint32_t height);

protected:
	IDXGISwapChain1*			swapChain;
	ID3D11Device*				device;
	ID3D11DeviceContext*		context;

	ID3D11RenderTargetView*		rtv;

private:
	int32_t init_direct3d();

	int32_t cleanup();

protected:
	virtual void on_resize(uint32_t width, uint32_t height);

protected:
	int64_t	freq;
	int64_t startTime;
	int64_t prevTime;

	float deltaTime;
	float totalTime;

private:
	void init_timer();

	void on_tick();

protected:
	virtual int32_t init_assets() = 0;
	virtual void cleanup_assets() = 0;

	virtual void update() = 0;
	virtual void render() = 0;
};