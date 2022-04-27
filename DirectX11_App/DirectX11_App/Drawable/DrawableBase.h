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
		return !staticBinds.empty(); // static �����͵��� ��� �����̳ʰ� ��� ������, ���� �ʱ�ȭ�� ������ �ʾ����� �ǹ�.
	} 
	static void AddStaticBind(std::unique_ptr<Bind::Bindable> bind) noxnd
	{
		assert("�ε��� ���۸� bind �ϱ� ���ؼ��� �ݵ�� AddStaticIndexBuffer�� ��� ��." && typeid(*bind) != typeid(Bind::IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noxnd
	{
		assert("�ε��� ���۸� �� �� �߰��Ϸ��� �õ��ϰ� ����." && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic() noxnd
	{
		assert("�ε��� ���۸� �� �� �߰��Ϸ��� �õ��ϰ� ����." && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds) // staticBinds�� ��ȸ�ϸ鼭 �ε��� ���۸� ã�� pIndexBuffer�� �־���.
		{
			if (const auto p = dynamic_cast<Bind::IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("staticBinds���� �ε��� ���۸� ã�� �� ����." && pIndexBuffer != nullptr);
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