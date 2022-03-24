#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty(); // static �����͵��� ��� �����̳ʰ� ��� ������, ���� �ʱ�ȭ�� ������ �ʾ����� �ǹ�.
	} 
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("�ε��� ���۸� bind �ϱ� ���ؼ��� �ݵ�� AddStaticIndexBuffer�� ��� ��." && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert("�ε��� ���۸� �� �� �߰��Ϸ��� �õ��ϰ� ����." && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("�ε��� ���۸� �� �� �߰��Ϸ��� �õ��ϰ� ����." && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds) // staticBinds�� ��ȸ�ϸ鼭 �ε��� ���۸� ã�� pIndexBuffer�� �־���.
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("staticBinds���� �ε��� ���۸� ã�� �� ����." && pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;