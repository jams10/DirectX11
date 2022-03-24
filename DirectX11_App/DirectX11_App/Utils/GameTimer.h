#pragma once

#include<chrono>

class GameTimer
{
public:
	GameTimer() noexcept;

	float GetTotalTime() noexcept;
	float GetDeltaTime() noexcept;

	void ReStartTotalTimer() noexcept;
	void PauseTotalTimer() noexcept;
	void Tick() noexcept;

	bool IsStopped() noexcept { return hasStopped; }

private:
	bool hasStopped;
	float pausedTime;
	std::chrono::steady_clock::time_point initialTimePoint; // �ʱ� Timer ��ü ���� ������ Time Point
	std::chrono::steady_clock::time_point startTimePoint;   // ��ü Ÿ�̸� ���� ���� Time Point
	std::chrono::steady_clock::time_point stopTimePoint;    // ��ü Ÿ�̸� ���� ���� Time Point
	std::chrono::steady_clock::time_point prevTimePoint;    // ���� ������ ���� Time Point
	std::chrono::steady_clock::time_point currentTimePoint; // ���� ������ ���� Time Point
};