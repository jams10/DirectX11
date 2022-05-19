#pragma once
#include "../Windows/WindowsHeader.h"
#include "../ErrorHandling/CustomException.h"
#include <string>
#include <assert.h>
#include <memory>
#include "../ConditionalNoexcept.h"

// Surface : �̹��� ������ �ڵ忡�� �ٷ�� ���� ����ȭ�� Ŭ����.
class Surface
{
public:
	// ���� �ȼ� ���� �ǹ��ϴ� Ŭ����. dword Ÿ������ �ȼ� ���� �����.
	class Color
	{
	public:
		unsigned int dword; // unsigned int(4����Ʈ) ������, 8bit * 4(ARGB) = 32bit. �� �ȼ��� �ش��ϴ� ������ ��� �ֱ⿡ ����� ũ��.
		/*
		    <dword 32��Ʈ �� ����>
			      Alpha                Red                Green                Blue
			[][][][] [][][][] | [][][][] [][][][] | [][][][] [][][][] | [][][][] [][][][]
		*/
	public:
		constexpr Color() noexcept : dword()
		{}
		// Color ��ü�� �޾� dword ���� �ʱ�ȭ ���ִ� ���� ������.
		constexpr Color(const Color& col) noexcept
			:
			dword(col.dword)
		{}
		// dword ���� ���� �޾� �ʱ�ȭ ���ִ� ������.
		constexpr Color(unsigned int dw) noexcept
			:
			dword(dw)
		{}
		// ������ a,r,g,b ���� ���� �޾� dword ���� �ʱ�ȭ ���ִ� ������.
		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((x << 24u) | (r << 16u) | (g << 8u) | b) // dword�� �ѹ��� �־��ֱ� ���� �� ������ ��Ʈ ��ġ�� �̵����� �־���.
		{}
		// ������ r,g,b ���� ���� �޾� dword ���� �ʱ�ȭ ���ִ� ������.
		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((r << 16u) | (g << 8u) | b)
		{}
		// r,g,b ���� Color ��ü��, a���� ���� �޾� dword ���� �ʱ�ȭ ���ִ� ������.
		constexpr Color(Color col, unsigned char x) noexcept
			:
			Color((x << 24u) | col.dword)
		{}
		// ���� ���� ������
		Color& operator =(Color color) noexcept
		{
			dword = color.dword;
			return *this;
		}
		// �ȼ� ������ Alpha �� �����ϴ� �Լ�.
		constexpr unsigned char GetX() const noexcept
		{
			return dword >> 24u;
		}
		// �ȼ� ������ Alpha �� �����ϴ� �Լ�.
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		// �ȼ� ������ Red �� �����ϴ� �Լ�.
		constexpr unsigned char GetR() const noexcept
		{
			return (dword >> 16u) & 0xFFu; // 0xFFu : 1111 1111. 8��Ʈ ���� �Ѱ��ֱ� ���� �� �ڷ� ���ܿͼ� &���� �� �Ѱ���.
		}
		// �ȼ� ������ Green �� �����ϴ� �Լ�.
		constexpr unsigned char GetG() const noexcept
		{
			return (dword >> 8u) & 0xFFu;
		}
		// �ȼ� ������ Blue �� �����ϴ� �Լ�.
		constexpr unsigned char GetB() const noexcept
		{
			return dword & 0xFFu;         // ���ʿ� Blue ��ġ�� 8��Ʈ ��ġ�̱� ������ ����Ʈ ������ �ʿ� ����.
		}
		// �ȼ� ������ Alpha �� �����ϴ� �Լ�.
		void SetX(unsigned char x) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u); // Ư�� ��ġ�� ��Ʈ ���� �ٲ��ֱ� ���� �������� ��Ʈ�� ��ġ�� ���� | ������ ����.
		}
		// �ȼ� ������ Alpha �� �����ϴ� �Լ�.
		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}
		// �ȼ� ������ Red �� �����ϴ� �Լ�.
		void SetR(unsigned char r) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}
		// �ȼ� ������ Green �� �����ϴ� �Լ�.
		void SetG(unsigned char g) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}
		// �ȼ� ������ Blue �� �����ϴ� �Լ�.
		void SetB(unsigned char b) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	};
public:
	// ���� ó���� Ŀ���� ���� Ŭ����.
	class Exception : public CustomException
	{
	public:
		Exception(int line, const char* file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
public:
	//
	// Surface Ŭ��������
	//
	Surface(unsigned int width, unsigned int height) noexcept;
	Surface(Surface&& source) noexcept;
	Surface(Surface&) = delete;
	Surface& operator=(Surface&& donor) noexcept;
	Surface& operator=(const Surface&) = delete;
	~Surface();
	//
	// Color �� ����
	//
	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noxnd;
	Color GetPixel(unsigned int x, unsigned int y) const noxnd;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	//
	//Surface ���� ����
	//
	static Surface FromFile(const std::string& name);
	void Save(const std::string& filename) const;
	void Copy(const Surface& src) noxnd;
	bool AlphaLoaded() const noexcept;
private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam, bool alphaLoaded = false) noexcept;
private:
	std::unique_ptr<Color[]> pBuffer;
	unsigned int width;  // Surface(�̹���) �ʺ�
	unsigned int height; // Surface(�̹���) ����
	bool alphaLoaded = false;
};