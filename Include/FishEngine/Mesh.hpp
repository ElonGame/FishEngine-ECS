#pragma once

#include "Engine.hpp"
#include "Object.hpp"
#include "MeshVertex.hpp"

#include <vector>
#include <bgfx/bgfx.h>

#include <FishEngine/Math/IntVector.hpp>

namespace FishEngine
{

struct SubMeshInfo
{
	int StartIndex = 0;
	int Length = 0;
	int VertexOffset = 0;
};

class Mesh : public Object
{
	friend class MeshUtil;
public:
	bgfx::IndexBufferHandle m_IndexBuffer;
	bgfx::VertexBufferHandle m_VertexBuffer;
	
	// for cpu skinning
	bgfx::DynamicVertexBufferHandle m_DynamicVertexBuffer = BGFX_INVALID_HANDLE;
	std::vector<PUNTVertex> m_DynamicVertices;

	std::vector<FishEngine::Int4> joints;
	std::vector<Vector4> weights;
	
	Bounds bounds;


	inline static Mesh* Cube = nullptr;
	inline static Mesh* Sphere = nullptr;
	inline static Mesh* Cylinder = nullptr;
	inline static Mesh* Cone = nullptr;
	inline static Mesh* Plane = nullptr;
	inline static Mesh* Quad = nullptr;
	inline static Mesh* Capsule = nullptr;

	static void StaticInit();

	void Bind(int subMeshIndex = -1, int bgfxStream = 0);

	bool IsSkinned() const { return joints.size() > 0; }

//private:
public:
	uint32_t m_VertexCount = 0;
	uint32_t m_TriangleCount = 0;
	int						m_SubMeshCount = 1;
	//std::vector<Vector3>    m_vertices;
	//std::vector<Vector3>    m_normals;
	//std::vector<Vector2>    m_uv;
	//std::vector<Vector3>    m_tangents;
	//std::vector<uint32_t>   m_triangles;
	//std::vector<uint32_t>	m_SubMeshIndexOffset;	// index start

	std::vector<SubMeshInfo> m_SubMeshInfos;
	std::vector<PUNTVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	
public:
	void __Upload();
};

namespace ECS
{
	class GameObject;
	class Scene;
}


class MeshUtil : public Static
{
public:
	static Mesh* FromTextFile(const String & str);
	
};

}
