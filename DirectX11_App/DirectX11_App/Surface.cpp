#define FULL_WIN_API // GDI+ 사용을 위해 특정 API 제외 없이 모든 Window API를 사용하기 위해 FULL_WIN_API 정의해줌.
#include "Surface.h"
#include "Window.h"
#include <sstream>
#include <cassert>
#include <filesystem>
#include "CustomUtil.h"

Surface::Surface(unsigned int width, unsigned int height)
{
	HRESULT hr = scratch.Initialize2D(
		format,
		width, height, 1u, 1u
	);
	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, "Failed to initialize ScratchImage", hr);
	}
}

void Surface::Clear(Color fillValue) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++)
	{
		auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, fillValue);
	}
}

// 특정 위치의 색상 값을 바꾸는 함수.
void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
}
// 특정 위치의 색상 값을 리턴하는 함수.
Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
}
// Surface의 너비를 리턴하는 함수.
unsigned int Surface::GetWidth() const noexcept
{
	return (unsigned int)scratch.GetMetadata().width;
}
// Surface의 높이를 리턴하는 함수.
unsigned int Surface::GetHeight() const noexcept
{
	return (unsigned int)scratch.GetMetadata().height;
}
// Surface의 색상 값이 저장된 버퍼의 포인터를 리턴하는 함수.
Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>(scratch.GetPixels());
}
// Surface의 색상 값이 저장된 버퍼의 포인터를 리턴하는 함수.(const 버전)
const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}
// Surface의 색상 값이 저장된 버퍼의 포인터를 리턴하는 함수.(const 버전)
const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}
// 파일로 부터 Surface 객체를 생성해 리턴하는 함수.
Surface Surface::FromFile(const std::string& name)
{

	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(ToWide(name).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch);

	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, name, "Failed to load image", hr);
	}

	if (scratch.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (FAILED(hr))
		{
			throw Surface::Exception(__LINE__, __FILE__, name, "Failed to convert image", hr);
		}

		return Surface(std::move(converted));
	}

	return Surface(std::move(scratch));
}
// Surface 객체를 저장해주는 함수.
void Surface::Save(const std::string& filename) const
{
	const auto GetCodecID = [](const std::string& filename) {
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();
		if (ext == ".png")
		{
			return DirectX::WIC_CODEC_PNG;
		}
		else if (ext == ".jpg")
		{
			return DirectX::WIC_CODEC_JPEG;
		}
		else if (ext == ".bmp")
		{
			return DirectX::WIC_CODEC_BMP;
		}
		throw Exception(__LINE__, __FILE__, filename, "Image format not supported");
	};

	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec(GetCodecID(filename)),
		ToWide(filename).c_str()
	);
	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, filename, "Failed to save image", hr);
	}
}

// 텍스쳐에 Alpha 채널이 있는지 여부를 리턴하는 함수.
bool Surface::AlphaLoaded() const noexcept
{
	return !scratch.IsAlphaAllOpaque();
}

Surface::Surface(DirectX::ScratchImage scratch) noexcept
	:
	scratch(std::move(scratch))
{}

#pragma region Exception
Surface::Exception::Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr) noexcept
	:
	CustomException(line, file),
	note("[Note] " + note)
{
	if (hr)
	{
		note = "[Error String] " + Window::Exception::TranslateErrorCode(*hr) + "\n" + note;
	}
}

Surface::Exception::Exception(int line, const char* file, std::string filename, std::string note_in, std::optional<HRESULT> hr) noexcept
	:
	CustomException(line, file)
{
	using namespace std::string_literals;
	note = "[File] "s + filename + "\n"s + "[Note] "s + note_in;

	if (hr)
	{
		note = "[Error String] " + Window::Exception::TranslateErrorCode(*hr) + note;
	}
}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << CustomException::what() << std::endl << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "Custom Surface Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return note;
}
#pragma endregion