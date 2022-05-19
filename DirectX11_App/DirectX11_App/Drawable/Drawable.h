#pragma once
#include "../Graphics.h"
#include <DirectXMath.h>
#include "../ConditionalNoexcept.h"
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics & gfx) const noxnd;
	virtual ~Drawable() = default;

	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}
protected:
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noxnd;
private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr; // 인덱스 버퍼의 경우 DrawIndexed 함수 호출 시 인덱스의 개수를 알아야 하므로, 따로 포인터로 들고 있어 줌.
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};