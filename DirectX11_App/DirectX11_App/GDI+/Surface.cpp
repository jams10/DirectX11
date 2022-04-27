#define FULL_WIN_API // GDI+ ����� ���� Ư�� API ���� ���� ��� Window API�� ����ϱ� ���� FULL_WIN_API ��������.
#include "Surface.h"
#include <sstream>

namespace Gdiplus
{
	// �츮�� WindowsHeader.h ���Ͽ��� Windows���� ���Ǵ� min, max�� ǥ�� ���ӽ����̽������� min,max��
	// �浹�� �����ְ��� NOMINMAX�� �����ߴµ�, Gdiplus���� ǥ�� ���ӽ����̽��� min, max�� ����ϹǷ� Gdiplus ���ӽ����̽���
	// std::min, std::max�� ����Ѵٰ� �˷��� ��, gdiplus ��� ������ include ��.
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
// �̵� ���� ������
Surface& Surface::operator=(Surface&& donor) noexcept
{
	width = donor.width;
	height = donor.height;
	pBuffer = std::move(donor.pBuffer);
	donor.pBuffer = nullptr;
	return *this;
}
// �̵� ������
Surface::Surface(Surface&& source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height)
{}

Surface::~Surface()
{}
// Surface�� �ϳ��� �������� ä���ִ� �Լ�.
void Surface::Clear(Color fillValue) noexcept
{
	memset(pBuffer.get(), fillValue.dword, width * height * sizeof(Color));
}
// Ư�� ��ġ�� ���� ���� �ٲٴ� �Լ�.
void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	pBuffer[y * width + x] = c;
}
// Ư�� ��ġ�� ���� ���� �����ϴ� �Լ�.
Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return pBuffer[y * width + x];
}
// Surface�� �ʺ� �����ϴ� �Լ�.
unsigned int Surface::GetWidth() const noexcept
{
	return width;
}
// Surface�� ���̸� �����ϴ� �Լ�.
unsigned int Surface::GetHeight() const noexcept
{
	return height;
}
// Surface�� ���� ���� ����� ������ �����͸� �����ϴ� �Լ�.
Surface::Color* Surface::GetBufferPtr() noexcept
{
	return pBuffer.get();
}
// Surface�� ���� ���� ����� ������ �����͸� �����ϴ� �Լ�.(const ����)
const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}
// Surface�� ���� ���� ����� ������ �����͸� �����ϴ� �Լ�.(const ����)
const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}
// ���Ϸ� ���� Surface ��ü�� ������ �����ϴ� �Լ�.
Surface Surface::FromFile(const std::string& name)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer;

	{
		// GDI+�� ���� ���� �̸��� wide string���� �ٲ���.
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE); // ��Ƽ����Ʈ ���ڿ��� �����ϴ� ���ڿ��� �����ڵ�� ��ȯ���ִ� �Լ�.

		// �̹��� ������ �ҷ��� Bitmap ��ü�� ��������. Bitmap Ŭ������ �׷��� �̹����� �ȼ� �����͸� �����ϴ� Ŭ����.
		Gdiplus::Bitmap bitmap(wideName);
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << name << "]: failed to load.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height); // Color ���� �������� �迭�� Surface�� ��� �ֵ��� ��.

		// Bitmap Ŭ������ GetPixel �Լ��� ���� �ȼ� ������ ���� pBuffer�� ä����.
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

	return Surface(width, height, std::move(pBuffer)); // ���Ϸ� ���� ��� �ȼ��� ���� ���� �޾ƿ� ��, Surface ��ü�� ����� ��������.
}
// Surface ��ü�� �������ִ� �Լ�.
void Surface::Save(const std::string& filename) const
{
	// �̹��� encoder�� Ŭ���� ���̵� ������ �����Լ�.
	//                  ĸ�� ��(���� ĸ��)           �Ķ����               ���� ��
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT  num = 0;          // �̹��� encoder���� ����
		UINT  size = 0;         // �̹��� encoder �迭�� ����Ʈ ũ��

		// ImageCodecInfo : ��ġ�� �̹��� encoder �� decoder(codec�̶� ��)�� ���� ������ �˻��ϴµ� �ʿ��� ��� �� �Լ����� ������.
		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		//	��� ������ image encoder���� ������ ImageCodecInfo ��ü �迭�� �� ũ�⸦ ����.
		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}
		// ImageCodecInfo �迭�� �� ũ�� ��ŭ ������ �Ҵ�.
		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}
		// �̹��� encoder�� �迭�� ����.
		GetImageEncoders(num, size, pImageCodecInfo);

		// image encoder �迭�� ���鼭, ���̵� ���� ��Ʈ�� �񱳸� ���� ���� format�� 
		// image encoder�� ������, CLSID(COM ��ü �ĺ� ���� Ű)�� �Ѱ���. 
		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) // ���� format�� encoder�� �߰��� ���
			{
				*pClsid = pImageCodecInfo[j].Clsid; // pClsid�� Ŭ���� ���̵� �Ѱ���.
				free(pImageCodecInfo);              // �Ҵ� �޾Ҵ� ���� ����.
				return;
			}
		}

		// ���� format�� image encoder�� ������, ���ܸ� ����.
		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving surface to [" << filename <<
			"]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};
	
	// bmp �̹��� ���� encoder�� �ִ��� Ȯ���ؼ� ������ Ŭ���� ���̵� ���� bmpID�� ����.
	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);

	// GDI+�� ���� ���� �̸��� wide string���� �ٲ���.
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	// Bitmap Ŭ���� ��ü�� ��������.
	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	// �츮�� ������ ���� ���� encoder Ŭ���� ���̵� �̿�, ���ϴ� ���� �������� �̹��� ������ ��������.
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}
// Surface ��ü�� �������ִ� �Լ�.
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