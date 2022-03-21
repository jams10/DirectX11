#include "GameTimer.h"

GameTimer::GameTimer() noexcept
    :
    hasStopped(false),
    pausedTime(0.0)
{
    initialTimePoint = std::chrono::steady_clock::now();
    startTimePoint = initialTimePoint;
    stopTimePoint = initialTimePoint;
    prevTimePoint = initialTimePoint;
    currentTimePoint = initialTimePoint;
}

// ���� ��ü �ð� ��� �Լ�.
float GameTimer::GetTotalTime() noexcept
{
    // �ʱ� Time Point���� ���� Time Point ������ duration.
    std::chrono::duration<float> timePerFrameFromStart = currentTimePoint - initialTimePoint;

    // ���� ������ �ð��� ��ü ���� �ð��� ������ ��Ű�� �ʱ� ���� ����.
    return timePerFrameFromStart.count() - pausedTime;
}

// ������ ���� �ð� ��� �Լ�.
float GameTimer::GetDeltaTime() noexcept
{
    // duration Ŭ������ �̿�, ��� time point ���.
    std::chrono::duration<float> timePerFrame = currentTimePoint - prevTimePoint;
    // duration Ŭ������ ��� �Լ� count()�� �̿�, ��� �ð� ����.
    return timePerFrame.count();
}

// ��ü �ð� Ÿ�̸� �����.
void GameTimer::ReStartTotalTimer() noexcept
{
    // Ÿ�̸� ���� ����Ʈ ���.
    startTimePoint = std::chrono::steady_clock::now();

    // ���� ���¿��� �ٽ� �����ϴ� ���
    if (hasStopped)
    {
        // �Ͻ� ������ �Ⱓ ���.
        std::chrono::duration<double> timePerFrame = startTimePoint - stopTimePoint;

        // �Ͻ� ������ �ð� ����.
        pausedTime += timePerFrame.count();

        // �Ͻ� ���� ���� ����Ʈ�� �ʱ�ȭ ����.
        stopTimePoint = startTimePoint;

        hasStopped = false;
    }
}

// ��ü �ð� Ÿ�̸� ����.
void GameTimer::PauseTotalTimer() noexcept
{
    // ���� ���°� �ƴ� ���
    if (!hasStopped)
    {
        // Ÿ�̸� ���� ����Ʈ ���.
        stopTimePoint = std::chrono::steady_clock::now();
        hasStopped = true;
    }
}

// Ÿ�̸� ƽ. Ÿ�̸Ӱ� ��� ���ư��� ��.
void GameTimer::Tick() noexcept
{
    prevTimePoint = currentTimePoint;
    currentTimePoint = std::chrono::steady_clock::now();
}