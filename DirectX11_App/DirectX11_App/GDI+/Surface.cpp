#define FULL_WIN_API // GDI+ 사용을 위해 특정 API 제외 없이 모든 Window API를 사용하기 위해 FULL_WIN_API 정의해줌.
#include "Surface.h"
#include <sstream>

namespace Gdiplus
{
	// 우리가 WindowsHeader.h 파일에서 Windows에서 사용되는 min, max와 표준 네임스페이스에서의 min,max의
	// 충돌을 막아주고자 NOMINMAX를 정의했는데, Gdiplus에서 표준 네임스페이스의 min, max를 사용하므로 Gdiplus 네임스페이스에
	// std::min, std::max를 사용한다고 알려준 뒤, gdiplus 헤더 파일을 include 함.
	using std::min;
	using std::max;
}
#include <gdiplus.h>

#pragma comment( lib,"gdiplus.lib" )

Surface::Surface(unsigned int width, unsigned int height) noexcept
	:
	pBuffer(std::make_unique<Color[]>(width* height)),
	width(width),
	height(height)
{}
// 이동 대입 연산자
Surface& Surface::operator=(Surface&& donor) noexcept
{
	width = donor.width;
	height = donor.height;
	pBuffer = std::move(donor.pBuffer);
	donor.pBuffer = nullptr;
	return *this;
}
// 이동 생성자
Surface::Surface(Surface&& source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height)
{}

Surface::~Surface()
{}
// Surface를 하나의 색상으로 채워주는 함수.
void Surface::Clear(Color fillValue) noexcept
{
	memset(pBuffer.get(), fillValue.dword, width * height * sizeof(Color));
}
// 특정 위치의 색상 값을 바꾸는 함수.
void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	pBuffer[y * width + x] = c;
}
// 특정 위치의 색상 값을 리턴하는 함수.
Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return pBuffer[y * width + x];
}
// Surface의 너비를 리턴하는 함수.
unsigned int Surface::GetWidth() const noexcept
{
	return width;
}
// Surface의 높이를 리턴하는 함수.
unsigned int Surface::GetHeight() const noexcept
{
	return height;
}
// Surface의 색상 값이 저장된 버퍼의 포인터를 리턴하는 함수.
Surface::Color* Surface::GetBufferPtr() noexcept
{
	return pBuffer.get();
}
// Surface의 색상 값이 저장된 버퍼의 포인터를 리턴하는 함수.(const 버전)
const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}
// Surface의 색상 값이 저장된 버퍼의 포인터를 리턴하는 함수.(const 버전)
const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}
// 파일로 부터 Surface 객체를 생성해 리턴하는 함수.
Surface Surface::FromFile(const std::string& name)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer;

	{
		// GDI+를 위해 파일 이름을 wide string으로 바꿔줌.
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE); // 멀티바이트 문자열을 구성하는 문자열을 유니코드로 변환해주는 함수.

		// 이미지 파일을 불러와 Bitmap 객체를 생성해줌. Bitmap 클래스는 그래픽 이미지의 픽셀 데이터를 저장하는 클래스.
		Gdiplus::Bitmap bitmap(wideName);
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << name << "]: failed to load.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height); // Color 값을 저장해줄 배열을 Surface가 들고 있도록 함.

		// Bitmap 클래스의 GetPixel 함수를 통해 픽셀 색상을 얻어와 pBuffer를 채워줌.
		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
			}
		}
	}

	return Surface(width, height, std::move(pBuffer)); // 파일로 부터 모든 픽셀의 색상 값을 받아온 뒤, Surface 객체를 만들어 리턴해줌.
}
// Surface 객체를 저장해주는 함수.
void Surface::Save(const std::string& filename) const
{
	// 이미지 encoder의 클래스 아이디를 얻어오는 람다함수.
	//                  캡쳐 값(참조 캡쳐)           파라미터               리턴 값
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT  num = 0;          // 이미지 encoder들의 개수
		UINT  size = 0;         // 이미지 encoder 배열의 바이트 크기

		// ImageCodecInfo : 설치된 이미지 encoder 및 decoder(codec이라 함)의 관련 정보를 검색하는데 필요한 멤버 및 함수들을 제공함.
		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		//	사용 가능한 image encoder들의 개수와 ImageCodecInfo 객체 배열의 총 크기를 얻어옴.
		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}
		// ImageCodecInfo 배열의 총 크기 만큼 공간을 할당.
		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}
		// 이미지 encoder의 배열을 얻어옴.
		GetImageEncoders(num, size, pImageCodecInfo);

		// image encoder 배열을 돌면서, 와이드 문자 스트링 비교를 통해 같은 format의 
		// image encoder가 있으면, CLSID(COM 객체 식별 고유 키)를 넘겨줌. 
		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) // 같은 format의 encoder를 발견한 경우
			{
				*pClsid = pImageCodecInfo[j].Clsid; // pClsid에 클래스 아이디를 넘겨줌.
				free(pImageCodecInfo);              // 할당 받았던 공간 해제.
				return;
			}
		}

		// 같은 format의 image encoder가 없으면, 예외를 던짐.
		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving surface to [" << filename <<
			"]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};
	
	// bmp 이미지 파일 encoder가 있는지 확인해서 있으면 클래스 아이디를 얻어와 bmpID에 저장.
	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);

	// GDI+를 위해 파일 이름을 wide string으로 바꿔줌.
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	// Bitmap 클래스 객체를 생성해줌.
	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	// 우리가 위에서 얻어온 파일 encoder 클래스 아이디를 이용, 원하는 파일 포맷으로 이미지 파일을 저장해줌.
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}
// Surface 객체를 복사해주는 함수.
void Surface::Copy(const Surface& src) noxnd
{
	assert(width == src.width);
	assert(height == src.height);
	memcpy(pBuffer.get(), src.pBuffer.get(), width * height * sizeof(Color));
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
	:
	width(width),
	height(height),
	pBuffer(std::move(pBufferParam))
{}

#pragma region Exception
Surface::Exception::Exception(int line, const char* file, std::string note) noexcept
	:
	CustomException(line, file),
	note(std::move(note))
{}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << CustomException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "Custom Graphics Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return note;
}
#pragma endregion