#pragma once

#include <string>
#include <memory>

struct D3D11_INPUT_ELEMENT_DESC;
struct _D3D_SHADER_MACRO;
using D3D_SHADER_MACRO = _D3D_SHADER_MACRO;

struct Shader {
	using PTR = std::shared_ptr<Shader>;

	Shader(
		const std::wstring& path, struct ID3D11Device* device,
		const D3D11_INPUT_ELEMENT_DESC* descs, int descsNum, 
		const D3D_SHADER_MACRO* macros, int macrosCount);

	void Activate(struct ID3D11DeviceContext* context);

	struct ID3D11InputLayout* layout;
	struct ID3D11VertexShader* vertShader = nullptr;
	struct ID3D11PixelShader* pixShader = nullptr;
};
