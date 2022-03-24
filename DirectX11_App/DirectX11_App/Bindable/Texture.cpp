#include "Texture.h"
#include "../GDI+/Surface.h"
#include "../ErrorHandling/GraphicsThrowMacros.h"

Texture::Texture(Graphics& gfx, const Surface& s)
{
	INFOMAN(gfx);

	// 텍스쳐 리소스 생성.
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = s.GetWidth();
	textureDesc.Height = s.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = s.GetBufferPtr(); // Surface 객체가 들고 있는 픽셀의 색상 값(Color 객체)을 들고 있는 배열에 대한 포인터를 넘겨줌.
	sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color); // 한 줄은 Color 객체 크기 * Surface(이미지)의 너비
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc, &sd, &pTexture
	));

	// 텍스쳐 자원에 대한 리소스 뷰를 생성함. 셰이더에는 이 리소스 뷰를 묶어줄 것임.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.Get(), &srvDesc, &pTextureView
	));
}

void Texture::Bind(Graphics& gfx) noexcept
{
	// 픽셀 셰이더에 텍스쳐를 묶어줌.
	GetContext(gfx)->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
}