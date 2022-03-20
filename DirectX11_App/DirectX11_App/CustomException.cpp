#include "CustomException.h"
#include <sstream>

CustomException::CustomException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

// what() : ����� ���� ���ڿ��� ������ �����ϴ� �Լ�.
const char* CustomException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
// ���� Ÿ���� ���ڿ��� �����ϴ� �Լ�.
const char* CustomException::GetType() const noexcept
{
	return "Custom Exception";
}
// ������ �߻��� �ڵ� �� ��ȣ�� �����ϴ� �Լ�.
int CustomException::GetLine() const noexcept
{
	return line;
}
// ������ �߻��� ���� �̸��� �����ϴ� �Լ�.
const std::string& CustomException::GetFile() const noexcept
{
	return file;
}
// ������ �߻��� �ڵ� �� ��ȣ + ���� �̸��� ���� ���ڿ��� �����ϴ� �Լ�.
std::string CustomException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}