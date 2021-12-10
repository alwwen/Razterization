#include <Windows.h>
#include <iostream>
#include <chrono>
#include <math.h>
#include <d3d11.h>
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"




void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer, ID3D11Buffer* constantBuffer,
	ConstantBufferPerObject constantBufferPerObject, float angle)
{
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	UpdateBuffer(immediateContext, constantBuffer, &constantBufferPerObject, angle);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	immediateContext->Draw(6, 0);

}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 576;
	HWND window;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	D3D11_VIEWPORT viewport;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* constantBuffer;
	ConstantBufferPerObject constantBufferPerObject = {};

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout, constantBuffer))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}


	std::chrono::high_resolution_clock clock;
	auto starttime = clock.now();
	auto endtime = clock.now();
	float time = 0;
	float speed = 0.001;
	float currentangle = 0;


	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		time = std::chrono::duration<float>(endtime - starttime).count();
		starttime = clock.now();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		currentangle += time * speed;
		if (currentangle >= 2 * acos(0.0)) //Kolla p� denna FULKNEP
		{
			currentangle -= 2 * acos(0.0);
		}
		else if (currentangle <= -2 * acos(0.0)) //Kolla p� denna FULKNEP
		{
			currentangle += 2 * acos(0.0);
		}
		Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, constantBuffer, constantBufferPerObject, currentangle);
		swapChain->Present(0, 0);
		endtime = clock.now();
	}


	vertexBuffer->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}