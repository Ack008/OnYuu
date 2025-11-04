#pragma once
// Engine.h: include centrale per il motore di gioco. Include tutte le
// classi e componenti principali necessari per lo sviluppo di giochi
// Standard / third-party
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

// Core application / window / input
#include "Application.h"
#include "Layer.h"
#include "View.h"
#include "ImGuiLayer.h"
#include "Input.h"
#include "API.h"

// Scene / game objects / components
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "TreeComponent.h"
#include "TagComponent.h"
#include "ScriptingSystem.h"
#include "BackGround2DRender.h"

// Physics / collision
#include "Collider.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

// Rendering (mesh, material, shader, renderer, render command, buffers)
#include "RenderingTypeEnum.h"
#include "MeshComponent.h"
#include "Material.h"
#include "Shader.h"
#include "OpenGLShader.h"
#include "Buffer.h"
#include "RenderCommand.h"
#include "OpenGLRenderCommand.h"
#include "Renderer.h"
#include "BatchRenderer.h"
#include "OpenGLBatchRender.h"
#include "AssetManager.h"
#include "HermitInterpolationMesh.h"

// Camera
#include "Camera.h"

// Debug / tools
#include "DebugLayer.h"

// Utility / helpers
#include "MeshGPUusage.h"

// Small helper macro
#ifndef ENGINE_UNUSED
#  define ENGINE_UNUSED(x) (void)(x)
#endif

