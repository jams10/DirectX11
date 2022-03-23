#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	// �ε��� ���۸� �������� �ϸ� assert �߻� ��Ŵ.
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
{
	// �ε��� ���۰� �ƴ� �ٸ� �͵��� �������� �ϸ� assert �߻� ��Ŵ.
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get(); // �ε��� ���� �����͸� ���;� �ϱ� ������ �ε��� ���۸� �߰��ϴ� �Լ��� ���� ���־���.
	binds.push_back(std::move(ibuf));
}