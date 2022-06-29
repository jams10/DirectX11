#pragma once
#include <math.h>

// ������ ����.
constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

// ������ �����ϴ� ���ø� �Լ�.
template <typename T>
constexpr auto sq(const T& x) noexcept
{
	return x * x;
}

// -�� ~ ��� ������ ���ѽ��� �ִ� ���ø� �Լ�.
template<typename T>
T wrap_angle(T theta) noexcept
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = fmod(theta, twoPi);
	if (mod > (T)PI_D)
	{
		return mod - twoPi;
	}
	else if (mod < (T)PI_D)
	{
		return mod + twoPi;
	}
	return mod;
}

// ���� ���� ���ø� �Լ�.
template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha) noexcept
{
	return src + (dst - src) * alpha;
}

// 60�й� ������ �������� ��ȯ���ִ� ���ø� �Լ�.
template<typename T>
constexpr T to_rad(T deg) noexcept
{
	return deg * PI / (T)180.0;
}

// ����þ� ������ ������ִ� �Լ�.
template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}