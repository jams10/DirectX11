#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"
#include "../ConditionalNoexcept.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty(); // static 데이터들이 담긴 컨테이너가 비어 있으면, 아직 초기화를 해주지 않았음을 의미.
	} 
	static void AddStaticBind(std::unique_ptr<Bind::Bindable> bind) noxnd
	{
		assert("인덱스 버퍼를 bind 하기 위해서는 반드시 AddStaticIndexBuffer를 써야 함." && typeid(*bind) != typeid(Bind::IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noxnd
	{
		assert("인덱스 버퍼를 두 번 추가하려고 시도하고 있음." && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic() noxnd
	{
		assert("인덱스 버퍼를 두 번 추가하려고 시도하고 있음." && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds) // staticBinds를 순회하면서 인덱스 버퍼를 찾아 pIndexBuffer에 넣어줌.
		{
			if (const auto p = dynamic_cast<Bind::IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("staticBinds에서 인덱스 버퍼를 찾을 수 없음." && pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bind::Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bind::Bindable>> DrawableBase<T>::staticBinds;