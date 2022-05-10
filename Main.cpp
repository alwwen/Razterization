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
//#include <wrl/client.h> Com-pointers



void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer, ID3D11Buffer* constantBuffer,
	ConstantBufferPerObject constantBufferPerObject, float angle, ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState*& samplerState,
	DirectX::XMFLOAT4 cameraPosition, DirectX::XMFLOAT3 cameraFocus, ID3D11RasterizerState*& ras_state,
	ID3D11HullShader*& hShader, ID3D11DomainShader*& dShader)
{
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	UpdateBuffer(immediateContext, constantBuffer, &constantBufferPerObject, angle, cameraPosition, cameraFocus);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->HSSetShader(hShader, nullptr, 0);
	immediateContext->DSSetShader(dShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->RSSetState(ras_state);
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
	ID3D11HullShader* hShader;
	ID3D11DomainShader* dShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11RasterizerState* ras_state;
	ConstantBufferPerObject constantBufferPerObject = {};
	LightConstantBuffer lightConstantBuffer = {};
	DirectX::XMFLOAT4 cameraPosition = { 0.0f, 0.0f, -5.5f, 1.0f };
	DirectX::XMFLOAT3 cameraFocus = { 0.0f, 0.0f, 0.0f };

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout, constantBuffer, lightBuffer, hShader, dShader))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}


	int first_width = 0;
	int first_height = 0;
	int first_channels = 3;
	const char* picc = "goku1.jpg";
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

	D3D11_RASTERIZER_DESC ras_desc;
	ZeroMemory(&ras_desc, sizeof(D3D11_RASTERIZER_DESC));
	ras_desc.FillMode = D3D11_FILL_WIREFRAME;
	ras_desc.CullMode = D3D11_CULL_NONE;
	hr = device->CreateRasterizerState(&ras_desc, &ras_state);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 1;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	ID3D11SamplerState* samplerState;

	device->CreateSamplerState(&samplerDesc, &samplerState);

	lightConstantBuffer.att = DirectX::XMFLOAT4(10.0f, 4.0f, 1.0f, 1.0f);
	lightConstantBuffer.position = DirectX::XMFLOAT4(0.0f, 0.0f, -5.5f, 1.0f);
	lightConstantBuffer.cameraPosition = DirectX::XMFLOAT4(0.0f, 0.0f, -5.5f, 1.0f);
	lightConstantBuffer.colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	lightConstantBuffer.ambient = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	lightConstantBuffer.diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	immediateContext->PSSetConstantBuffers(1u, 1u, &lightBuffer);
	immediateContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &lightConstantBuffer, sizeof(LightConstantBuffer));
	immediateContext->Unmap(lightBuffer, 0);

	std::chrono::high_resolution_clock clock;
	auto starttime = clock.now();
	auto endtime = clock.now();
	float time = 0;
	float speed = 1.0;
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
		//currentangle += time * speed;
		if (currentangle > 2 * M_PI)
		{
			currentangle -= 2 * M_PI;
		}
		if (msg.message == WM_KEYDOWN)
		{
			if (GetKeyState(VK_LEFT) & 0x8000)
			{
				cameraPosition.x -= 0.1f;
				cameraFocus.x -= 0.1f;
			}
			if (GetKeyState(VK_RIGHT) & 0x8000)
			{
				cameraPosition.x += 0.1f;
				cameraFocus.x += 0.1f;
			}
			if (GetKeyState(VK_UP) & 0x8000)
			{
				cameraPosition.z += 0.1f;
				cameraFocus.z += 0.1f;
			}
			if (GetKeyState(VK_DOWN) & 0x8000)
			{
				cameraPosition.z -= 0.1f;
				cameraFocus.z -= 0.1f;
			}
			if (GetKeyState(VK_ESCAPE) & 0x8000)
			{
				msg.message = WM_QUIT;
			}
		}
		Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, constantBuffer,
			constantBufferPerObject, currentangle, textureSRV, samplerState, cameraPosition, cameraFocus, ras_state, hShader, dShader);
		//GetCursorPos(&cursorPosition);
		swapChain->Present(1, 0);
		endtime = clock.now();
	}

	lightBuffer->Release();
	constantBuffer->Release();
	textureSRV->Release();
	vertexBuffer->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	hShader->Release();
	dShader->Release();
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();
	samplerState->Release();
	ras_state->Release();

	return 0;
}
