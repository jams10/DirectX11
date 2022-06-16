#pragma once
#include "../Graphics.h"
#include "Job.h"
#include <vector>

class Pass
{
public:
	void Accept(Job job) noexcept
	{
		jobs.push_back(job); // FrameCommander���� Pass�� ���� Accept�� ȣ���ϸ�, �ش� Pass�� Job���� �о�ְ� ��.
	}
	void Execute(Graphics& gfx) const noxnd
	{
		for (const auto& j : jobs)
		{
			j.Execute(gfx);    // FrameCommander���� Pass�� ���� Execute�� ȣ���ϸ�, �ش� Pass�� ������ Job���� Execute�� ȣ����.
		}
	}
	void Reset() noexcept
	{
		jobs.clear();
	}
private:
	std::vector<Job> jobs; // ������ Pass�� ���� ���� Job���� ��� ����.
};