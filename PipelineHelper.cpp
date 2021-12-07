#include "PipelineHelper.h"
#include <string>
#include <fstream>
#include <iostream>
#include <DirectXMath.h>

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
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

	return true;
}

bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, //Index 3 �r fr�n vilken vertex buffer den ska komma ifr�n men eftersom vi bara har en s� kommer det fr�n index 0
		//{"COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, //12 = 4*3 offset per vertex
		{"UV", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
	SimpleVertex triangle[] = //KLOCKANS H�LL
	{
		{{-0.5, 0.5f, 0.0f}, {0,0,1}, {0,0}}, //H�GST UPP POSITION{x, y, z}, NORMALEN{x,y,z}
		{{0.5, -0.5f, 0.0f}, {0,0,1},{0,0}}, //NER TILL H�GER
		{{-0.5, -0.5f, 0.0f},{0,0,1},{0,0}},//NER TILL V�NSTER
		
		{{-0.5, 0.5f, 0.0f}, {0,0,1}, {0,0}}, //H�GST UPP {x, y, z}, {R, G, B}
		{{0.5, 0.5f, 0.0f}, {0,0,1} ,{0,0}}, //NER TILL H�GER
		{{0.5, -0.5f, 0.0f}, {0,0,1},{0,0}} //NER TILL V�NSTER
	};

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(triangle); //Storleken p� datan 24*3 =72
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; //Den kan inte �ndras p� efter den skapas
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //Ska vara en vertex buffer
	bufferDesc.CPUAccessFlags = 0; 
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0; //spelar bara roll vid en speciell typ av buffer

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangle;
	data.SysMemPitch = 0; //Bara relevanta f�r texturer, inte f�r buffrar
	data.SysMemSlicePitch = 0; //Hur mycket p� en slice ska de vara i 3d rummet om man delar upp de i 2d slices

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	return !FAILED(hr);
}

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout)
{
	std::string vShaderByteCode;
	if (!LoadShaders(device, vShader, pShader, vShaderByteCode))
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
	return true;
}