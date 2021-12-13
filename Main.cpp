#include <Windows.h>
#include <iostream>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include <d3d11.h>
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include <stdlib.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"




void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer, ID3D11Buffer* constantBuffer,
	ConstantBufferPerObject constantBufferPerObject, float angle, ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState*& samplerState /*ID3D11RasterizerState* RasterationState*/)
{
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	UpdateBuffer(immediateContext, constantBuffer, &constantBufferPerObject, angle);
	//immediateContext->RSSetState(RasterationState);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, &textureSRV);
	immediateContext->PSSetSamplers(0, 1, &samplerState);
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
	//ID3D11RasterizerState* RasterationState = nullptr;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport/* RasterationState*/))
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
	float speed = 1.0;
	float currentangle = 0;

	int first_width = 0;
	int first_height = 0;
	int first_channels = 3;
	const char* picc = "Kiwi.jpg";
	unsigned char* load_picc = stbi_load(picc, &first_width, &first_height, &first_channels, STBI_rgb_alpha);
	
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = first_width;
	textureDesc.Height = first_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = load_picc;
	data.SysMemPitch = first_width * STBI_rgb_alpha;

	ID3D11Texture2D* img;
	ID3D11ShaderResourceView* textureSRV;
	HRESULT hr = device->CreateTexture2D(&textureDesc, &data, &img);
	if (FAILED(hr))
	{
		return false;
	}
	hr = device->CreateShaderResourceView(img, nullptr, &textureSRV);
	if (FAILED(hr))
	{
		return false;
	}

	stbi_image_free(load_picc);
	img->Release();
	img = nullptr;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 1;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	ID3D11SamplerState* samplerState;

	device->CreateSamplerState(&samplerDesc, &samplerState);
	

	MSG msg = {};

	while(msg.message != WM_QUIT)
	{
		time = std::chrono::duration<float>(endtime - starttime).count();
		starttime = clock.now();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		currentangle += time * speed;
		if (currentangle > 2 * M_PI) // RENDERAR ENDAST ENA SIDAN EFTER ETT HELT VARV REDAN ÄR GJORT
		{
			currentangle -= 2 * M_PI;
		}
		else if (currentangle < -2 * M_PI)
		{
			currentangle += 2 * M_PI;
		}
		Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, constantBuffer, constantBufferPerObject, currentangle, textureSRV, samplerState/*RasterationState*/);
		
		swapChain->Present(1, 0);
		endtime = clock.now();
	}

	//RasterationState->Release();
	textureSRV->Release();
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

/*
	D3D11_SUBRESOURCE_DATA data = {load_picc, sizeof(uint32_t), 0 };

	D3D11_TEXTURE2D_DESC picDesc = {};
	picDesc.Width = first_width;
	picDesc.Height = first_height;
	picDesc.MipLevels = 1;
	picDesc.ArraySize = 1;
	picDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	picDesc.SampleDesc.Count = 1;
	picDesc.Usage = D3D11_USAGE_IMMUTABLE;
	picDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* pictureTexture2D;

	ID3D11ShaderResourceView* textureSRV = nullptr;

	HRESULT hr = device->CreateTexture2D(&picDesc, &data, &pictureTexture2D);

	if (!FAILED(hr)) {
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(pictureTexture2D, &SRVDesc, &textureSRV);
	}

	*/