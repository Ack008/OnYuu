#pragma once
// Engine.h: include centrale per il motore di gioco. Include tutte le
// classi e componenti principali necessari per lo sviluppo di giochi
// Standard / third-party
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <entt/entt.hpp>
#include "Model/Ray.h"
// Core application / window / input
#include "Application/Application.h"
#include "Application/Layer.h"
#include "Core/View/View.h"
#include "Application/Layer/ImGuiLayer.h"
#include "Application/Input/Input.h"
#include "Platform/API.h"

// Scene / game objects / components
#include "Core/Model/Scene.h"
#include "Core/Model/GameObject.h"
#include "Core/Model/Component.h"
#include "Core/Model/Components/Transform.h"
#include "Core/Model/Components/TreeComponent.h"
#include "Core/Model/Components/TagComponent.h"
#include "Core/Model/Components/ScriptingSystem.h"
#include "Core/Model/Components/BackGround2DRender.h"
#include "Core/Prefab.h"
// Components - Lights
#include "Core/Model/Components/Lights.h"

// Physics / collision
#include "Core/Model/Components/Collider.h"
#include "Physics/PhysicsEngine.h"
#include "Core/Model/Components/RigidBody.h"

// Rendering (mesh, material, shader, renderer, render command, buffers)
#include "Core/RenderingTypeEnum.h"
#include "Core/Model/Components/MeshComponent.h"
#include "Material.h"
#include "MetaShader.h"
#include "Shader.h"
#include "Render/Buffer.h"
#include "Render/RenderCommand.h"
#include "Render/Renderer.h"
#include "Render/BatchRenderer.h"
#include "Render/RenderTarget.h"
#include "Application/AssetManager.h"
#include "Application/MeshImporter.h"
#include "HermitInterpolationMesh.h"

// Camera
#include "Core/Model/Components/Camera.h"

// Debug / tools

// Utility / helpers

//ImGui
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/ImGuiFileDialog.h>
//Math
#include "math.h"



// Small helper macro
#ifndef ENGINE_UNUSED
#  define ENGINE_UNUSED(x) (void)(x)
#endif

