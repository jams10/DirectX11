#pragma once
#include "../Graphics.h"
#include "Job.h"
#include <vector>

class Pass
{
public:
	void Accept(Job job) noexcept
	{
		jobs.push_back(job); // FrameCommander에서 Pass에 대한 Accept를 호출하면, 해당 Pass에 Job들을 밀어넣게 됨.
	}
	void Execute(Graphics& gfx) const noxnd
	{
		for (const auto& j : jobs)
		{
			j.Execute(gfx);    // FrameCommander에서 Pass에 대한 Execute를 호출하면, 해당 Pass가 소유한 Job들의 Execute를 호출함.
		}
	}
	void Reset() noexcept
	{
		jobs.clear();
	}
private:
	std::vector<Job> jobs; // 각각의 Pass는 여러 개의 Job들을 들고 있음.
};