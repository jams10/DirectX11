#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noxnd
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noxnd
{
	// 인덱스 버퍼를 넣으려고 하면 assert 발생 시킴.
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noxnd
{
	// 인덱스 버퍼가 아닌 다른 것들을 넣으려고 하면 assert 발생 시킴.
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get(); // 인덱스 버퍼 포인터를 얻어와야 하기 때문에 인덱스 버퍼를 추가하는 함수만 따로 빼주었음.
	binds.push_back(std::move(ibuf));
}