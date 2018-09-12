#pragma once
#include "FishEngine/ECS.hpp"
#include <bgfx/bgfx.h>

class SingletonRenderState : public ECS::SingletonComponent
{
	friend class ECS::Scene;
	friend class RenderSystem;
public:
	uint64_t GetState() const { return m_State; }


protected:
	SingletonRenderState();

private:
	bgfx::UniformHandle m_UniformLightDir;
	bgfx::UniformHandle m_UniformCameraPos;
	bgfx::UniformHandle m_UniformJointMatrix;
	uint64_t m_State = 0;

	bool m_EnableFrustumCulling = true;
};


class RenderSystem : public ECS::ISystem
{
	SYSTEM(RenderSystem);
public:
	void OnAdded() override;
	void Start() override;
	void Update() override {}
	void Resize(int width, int height);

	void Draw();
};
