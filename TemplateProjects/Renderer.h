#pragma once
#include <utility>
#include <unordered_map>
#include <glm/glm.hpp>
#include "RenderingTypeEnum.h"
#include "MeshComponent.h"
#include "MeshGPUusage.h"
#include "Shader.h"	

class Render
{
public:
	Render() = default;
	~Render() = default;
    virtual void draw() = 0;
	virtual void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) { meshRenders.push_back(mesh); };
    virtual void clear() = 0;
	void setCameraMatrix(const glm::mat4& camMatrix) { cameraMatrix = camMatrix; }
	glm::mat4 getCameraMatrix() const { return cameraMatrix; }
    static std::shared_ptr<Render> getInstance();
protected:

	std::vector<RenderMeshComponent*> meshRenders;
	std::unordered_map<Mesh*, MeshGPUusage> meshGPUmap;
private:
	glm::mat4 cameraMatrix;
    static std::shared_ptr<Render> s_instance;
	static std::shared_ptr<Render> create();
};
