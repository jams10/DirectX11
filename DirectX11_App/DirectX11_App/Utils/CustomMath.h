#pragma once
#include <math.h>

// ������ ����.
constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

// ������ �����ϴ� ���ø� �Լ�.
template <typename T>
constexpr auto sq(const T& x)
{
	return x * x;
}

// -�� ~ ��� ������ ���ѽ��� �ִ� ���ø� �Լ�.
template<typename T>
T wrap_angle(T theta)
{
	// ȸ�� ������ ��� �ε� �Ҽ����̱� ������ ���� ��� ������ ��� ���� Ŀ�� ���е��� ������.
	// ���� -pi ~ pi ������ ������ ���ѽ��� ��.
	// fmod : ������ ��� �Լ�.
	const T modded = fmod(theta, (T)2.0 * (T)PI_D); // ���� : theta / �и� : 2��
	return (modded > (T)PI_D) ? // �������� �𺸴� Ŭ ���, ���������� -2��ŭ ���� -�� ~ �� ������ ������.(�ֱ� �Լ��̱� ������ 2�� �� ���� ����.)
		(modded - (T)2.0 * (T)PI_D) :
		modded;
}

// ���� ���� ���ø� �Լ�.
template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha)
{
	return src + (dst - src) * alpha;
}

// 60�й� ������ �������� ��ȯ���ִ� ���ø� �Լ�.
template<typename T>
constexpr T to_rad(T deg)
{
	return deg * PI / (T)180.0;
}