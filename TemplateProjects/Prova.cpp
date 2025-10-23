#include <iostream>
#include "Prova.h"
#include "TryComponent.h"
#include "MeshComponent.h"
#include "OpenGLShader.h"
#include "Camera.h"
Prova::Prova()
	:Scene(), Triangle(this), Square(this), Camera(this), shader(std::make_shared<OpenGLShader>("vertexShaderC.glsl", "fragmentShaderC.glsl")), material(shader)
{
	
	RenderMeshComponent& renderMesh = Triangle.addComponent<RenderMeshComponent>();
	renderMesh.material = &material;
	renderMesh.mesh = &triangoloMesh;
	RenderMeshComponent& renderMesh2 = Square.addComponent<RenderMeshComponent>();
	renderMesh2.material = &material;
	renderMesh2.renderingType = RenderingTypeEnum::LINE;
	renderMesh2.mesh = &squareMesh;


	Triangle.addComponent<TryComponent>();
	Camera.addComponent<Orthographic>(0,1600,0,900,1,20);
}

void Prova::OnResize(uint32_t width, uint32_t height)
{
	std::cout << "Resize Prova: " << width << "x" << height << std::endl;
	Camera.getComponent<Orthographic>().OnResize(width, height);
}
