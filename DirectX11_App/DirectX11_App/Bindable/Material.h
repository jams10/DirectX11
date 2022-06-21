#pragma once
#include <assimp/scene.h>
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include "../Jobber/Technique.h"

class Material
{
public:
	Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path) noxnd
	{

	}
	TemplateVertex::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept
	{
		using Type = TemplateVertex::VertexLayout::ElementType;
		TemplateVertex::VertexBuffer buf{ vtxLayout };
		buf.Resize(mesh.mNumVertices);
		if (vtxLayout.Has<Type::Position3D>())
		{
			for (int i = 0; i < mesh.mNumVertices; i++)
			{
				buf[i].
			}
		}
	}
	std::vector<Technique> GetTechniques() const noexcept
	{
		return techniques;
	}
private:
	TemplateVertex::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
};