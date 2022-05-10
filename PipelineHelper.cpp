#include "PipelineHelper.h"
#include <string>
#include <fstream>
#include <iostream>
#include <DirectXMath.h>



bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, 
	std::string& vShaderByteCode, ID3D11HullShader*& hShader, ID3D11DomainShader*& dShader)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("Debug/VertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open Vertex Shader file!" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	vShaderByteCode = shaderData;

	shaderData.clear();
	reader.close();
	reader.open("Debug/PixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PShader file!" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();
	reader.open("Debug/HullShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open HShader file!" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &hShader)))
	{
		std::cerr << "Failed to create hull shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();
	reader.open("Debug/DomainShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PShader file!" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &dShader)))
	{
		std::cerr << "Failed to create domain shader!" << std::endl;
		return false;
	}


	return true;
}

bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, //Index 3 �r fr�n vilken vertex buffer den ska komma ifr�n men eftersom vi bara har en s� kommer det fr�n index 0
		//{"COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, //12 = 4*3 offset per vertex
		//{"UV", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
	SimpleVertex triangle[] = //KLOCKANS H�LL
	{
		{{-0.5, 0.5f, -4.0f}, {0,0,1}, {0,0}}, //1 H�GST UPP POSITION{x, y, z}, NORMALEN{x,y,z}, {u,v}
		{{0.5, -0.5f, -4.0f}, {0,0,1},{1,1}}, //2 NER TILL H�GER
		{{-0.5, -0.5f, -4.0f},{0,0,1},{0,1}},//3 NER TILL V�NSTER
		
		{{-0.5, 0.5f, -4.0f}, {0,0,1}, {0,0}}, //4 H�GST UPP {x, y, z}, {R, G, B}
		{{0.5, 0.5f, -4.0f}, {0,0,1} ,{1,0}}, //5 NER TILL H�GER
		{{0.5, -0.5f, -4.0f}, {0,0,1},{1,1}} //6 NER TILL V�NSTER
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(triangle); //Storleken p� datan 24*3 =72
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; //Den kan inte �ndras p� efter den skapas
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //Ska vara en vertex buffer
	bufferDesc.CPUAccessFlags = 0; 
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0; //spelar bara roll vid en speciell typ av buffer

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = triangle;
	data.SysMemPitch = 0; //Bara relevanta f�r texturer, inte f�r buffrar
	data.SysMemSlicePitch = 0; //Hur mycket p� en slice ska de vara i 3d rummet om man delar upp de i 2d slices

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	return !FAILED(hr);
}

bool CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer*& constantBuffer)
{
	D3D11_BUFFER_DESC cBufferDesc;
	ZeroMemory(&cBufferDesc, sizeof(cBufferDesc));
	cBufferDesc.ByteWidth = sizeof(ConstantBufferPerObject);
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = constantBuffer;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&cBufferDesc, nullptr, &constantBuffer);

	return !FAILED(hr);
}

bool CreateLightConstantBuffer(ID3D11Device* device, ID3D11Buffer*& lightBuffer)
{
	D3D11_BUFFER_DESC lBufferDesc;
	ZeroMemory(&lBufferDesc, sizeof(lBufferDesc));
	lBufferDesc.ByteWidth = sizeof(LightConstantBuffer);
	lBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lBufferDesc.MiscFlags = 0;
	lBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = lightBuffer;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&lBufferDesc, nullptr, &lightBuffer);

	return !FAILED(hr);
}

void UpdateBuffer(ID3D11DeviceContext* immediatecontext, ID3D11Buffer*& constantPerObjectBuffer, ConstantBufferPerObject* constantBufferPerObject, float angle,
	DirectX::XMFLOAT4 floatCameraPosition, DirectX::XMFLOAT3 floatCameraFocus)
{
	DirectX::XMMATRIX worldmatrix = DirectX::XMMatrixIdentity(); //Satt
	DirectX::XMMATRIX rotation = DirectX::XMMatrixIdentity(); //DirectX::XMMatrixRotationAxis({0.0f, 1.0f, 0.0f}, angle); //Satt
	DirectX::XMMATRIX scale = DirectX::XMMatrixIdentity(); //Satt
	DirectX::XMMATRIX translation = DirectX::XMMatrixIdentity(); //Satt

	DirectX::XMMATRIX viewmatrix; //Satt
	DirectX::XMMATRIX perspectiveProjection; //Satt

	DirectX::XMVECTOR aRotation; //Satt
	DirectX::XMVECTOR cameraPosition = DirectX::XMLoadFloat4(&floatCameraPosition); //Satt
	//DirectX::XMVECTOR cameraFocus = DirectX::XMLoadFloat3(&floatCameraFocus); //Satt
	//DirectX::XMVECTOR cameraPosition = { 0.0f, 0.0f, -5.5f };
	DirectX::XMVECTOR cameraFocus = DirectX::XMLoadFloat3(&floatCameraFocus);
	DirectX::XMVECTOR cameraUp = {0.0f,1.0f,0.0f}; //Vad som �r upp�t f�r kameran

	DirectX::XMFLOAT4X4 objectsavedMatrix;

	aRotation = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	rotation = DirectX::XMMatrixRotationAxis(aRotation, angle);
	scale = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 2.0f);

	worldmatrix = scale * rotation * translation; 
	viewmatrix = DirectX::XMMatrixLookAtLH(cameraPosition, cameraFocus, cameraUp);
	perspectiveProjection = DirectX::XMMatrixPerspectiveFovLH(0.25f * 3.1415f, 1024.0f / 576.0f, 0.1f, 500.0f);
	DirectX::XMMATRIX WVP = worldmatrix * viewmatrix * perspectiveProjection;

	DirectX::XMStoreFloat4x4(&objectsavedMatrix, DirectX::XMMatrixTranspose(WVP));
	constantBufferPerObject->WVP = objectsavedMatrix;

	DirectX::XMStoreFloat4x4(&objectsavedMatrix, DirectX::XMMatrixTranspose(worldmatrix));
	constantBufferPerObject->world = objectsavedMatrix;

	immediatecontext->VSSetConstantBuffers(0, 1, &constantPerObjectBuffer);
	immediatecontext->HSSetConstantBuffers(0, 1, &constantPerObjectBuffer);
	immediatecontext->DSSetConstantBuffers(0, 1, &constantPerObjectBuffer);
	immediatecontext->PSSetConstantBuffers(0, 1, &constantPerObjectBuffer);
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	immediatecontext->Map(constantPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, constantBufferPerObject, sizeof(ConstantBufferPerObject));
	immediatecontext->Unmap(constantPerObjectBuffer, 0); 

}

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, 
	ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBuffer, 
	ID3D11Buffer*& lightBuffer, ID3D11HullShader*& hShader, ID3D11DomainShader*& dShader)
{
	std::string vShaderByteCode;
	if (!LoadShaders(device, vShader, pShader, vShaderByteCode, hShader, dShader))
	{
		std::cerr << "Error loading shader!" << std::endl;
		return false;
	}
	if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}
	if (!CreateVertexBuffer(device, vertexBuffer))
	{
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}
	if (!CreateConstantBuffer(device, constantBuffer))
	{
		std::cerr << "Error creating constant buffer!" << std::endl;
		return false;
	}
	if (!CreateLightConstantBuffer(device, lightBuffer))
	{
		std::cerr << "Error creating constant buffer!" << std::endl;
		return false;
	}

	return true;
}