#pragma once
#include <array>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

struct SimpleVertex
{
	float pos[3];
	//float clr[3];
	float nor[3];
	float uv[2];
	
	SimpleVertex(const std::array<float, 3>& position, const std::array<float, 3> normal, const std::array<float, 2> UV)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
			//clr[i] = colour[i];
			nor[i] = normal[i];
		}
		for (int i = 0; i < 2; ++i)
		{
			uv[i] = UV[i];
		}
	}
};

struct ConstantBufferPerObject
{
	DirectX::XMFLOAT4X4 WVP;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMFLOAT4X4 rotation;
};

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBuffer);
bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode);
bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode);
bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer);
bool CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer*& constantBuffer);
void UpdateBuffer(ID3D11DeviceContext* immediatecontext, ID3D11Buffer*& constantPerObjectBuffer, ConstantBufferPerObject* constantBufferPerObject, float angle);
