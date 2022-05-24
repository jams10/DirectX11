#pragma once
#include <math.h>

// 원주율 정의.
constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

// 제곱을 리턴하는 템플릿 함수.
template <typename T>
constexpr auto sq(const T& x)
{
	return x * x;
}

// -π ~ π로 각도를 제한시켜 주는 템플릿 함수.
template<typename T>
T wrap_angle(T theta)
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

// 선형 보간 템플릿 함수.
template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha)
{
	return src + (dst - src) * alpha;
}

// 60분법 각도를 라디안으로 변환해주는 템플릿 함수.
template<typename T>
constexpr T to_rad(T deg)
{
	return deg * PI / (T)180.0;
}