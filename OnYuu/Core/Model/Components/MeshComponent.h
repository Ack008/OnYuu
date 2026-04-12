#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Core/RenderingTypeEnum.h"
#include "Core/Shader.h"
#include "Core/Material.h"

namespace OnYuu {

    // Mesh rimane struct dati pura — non cambia
    struct Mesh {
        std::vector<glm::vec3> position;
        std::vector<glm::vec4> color;
        std::vector<glm::vec2> texCoord;
        std::vector<glm::vec3> normal;
        std::vector<uint32_t>  indices;

        bool empty() const { return position.empty() || indices.empty(); }
    };

    // Flags che il renderer legge per sapere cosa aggiornare
    enum class RenderDirtyFlags : uint8_t {
        None = 0,
        Mesh = 1 << 0,  // geometria cambiata → re-upload GPU
        Material = 1 << 1,  // material/texture cambiati → update descriptor
        Topology = 1 << 2,  // renderingType cambiato → nuova pipeline
        All = Mesh | Material | Topology
    };

    inline RenderDirtyFlags operator|(RenderDirtyFlags a, RenderDirtyFlags b) {
        return static_cast<RenderDirtyFlags>(
            static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
            );
    }
    inline RenderDirtyFlags operator&(RenderDirtyFlags a, RenderDirtyFlags b) {
        return static_cast<RenderDirtyFlags>(
            static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
            );
    }
    inline bool hasFlag(RenderDirtyFlags flags, RenderDirtyFlags flag) {
        return (flags & flag) != RenderDirtyFlags::None;
    }

    class RenderMeshComponent {
    public:
        RenderMeshComponent() = default;

        RenderMeshComponent(
            std::shared_ptr<Mesh> mesh,
            std::string           materialID,
            RenderingTypeEnum     topology = RenderingTypeEnum::TRIANGLE)
            : mesh_(std::move(mesh))
            , materialID_(std::move(materialID))
            , renderingType_(topology)
        {}

        // ── Getter ────────────────────────────────────────────────────
        const std::shared_ptr<Mesh>& getMesh()          const { return mesh_; }
        const std::string& getMaterialID()    const { return materialID_; }
        RenderingTypeEnum            getRenderingType() const { return renderingType_; }

        // ── Setter — settano il flag corrispondente ───────────────────
        void setMesh(std::shared_ptr<Mesh> mesh) {
            if (mesh_ == mesh) return;
            mesh_ = std::move(mesh);
            dirtyFlags_ = dirtyFlags_ | RenderDirtyFlags::Mesh;
        }

        void setMaterialID(std::string id) {
            if (materialID_ == id) return;
            materialID_ = std::move(id);
            dirtyFlags_ = dirtyFlags_ | RenderDirtyFlags::Material;
        }

        void setRenderingType(RenderingTypeEnum t) {
            if (renderingType_ == t) return;
            renderingType_ = t;
            // Topology diversa = pipeline diversa = trattala come mesh+material cambiati
            dirtyFlags_ = dirtyFlags_ | RenderDirtyFlags::Topology
                | RenderDirtyFlags::Material;
        }

        // ── Dirty flag ────────────────────────────────────────────────
        bool             isDirty()                        const { return dirtyFlags_ != RenderDirtyFlags::None; }
        bool             isDirty(RenderDirtyFlags flag)   const { return hasFlag(dirtyFlags_, flag); }
        RenderDirtyFlags getDirtyFlags()                  const { return dirtyFlags_; }
        void             clearDirty() { dirtyFlags_ = RenderDirtyFlags::None; }

        // ── Utility ───────────────────────────────────────────────────
        bool empty() const { return !mesh_ || mesh_->empty(); }

        size_t getSize() const {
            if (!mesh_) return 0;
            return mesh_->position.size() * sizeof(glm::vec3)
                + mesh_->color.size() * sizeof(glm::vec4)
                + mesh_->texCoord.size() * sizeof(glm::vec2)
                + mesh_->normal.size() * sizeof(glm::vec3);
        }

    private:
        std::shared_ptr<Mesh> mesh_;
        std::string           materialID_;
        RenderingTypeEnum     renderingType_ = RenderingTypeEnum::TRIANGLE;

        // Inizia con All così al primo registerMesh il renderer sa che deve
        // fare il setup completo
        RenderDirtyFlags dirtyFlags_ = RenderDirtyFlags::All;
    };

} // namespace OnYuu