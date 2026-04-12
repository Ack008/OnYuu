#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace OnYuu {

    class Transform {
    public:
        Transform() = default;
        Transform(glm::vec3 pos, glm::vec3 rot = { 0,0,0 }, glm::vec3 scl = { 1,1,1 })
            : position_(pos), rotation_(rot), scale_(scl) {}

        // ── Getter ────────────────────────────────────────────────────
        const glm::vec3 getPosition() const { return position_; }
        const glm::vec3 getRotation() const { return rotation_; }
        const glm::vec3 getScale()    const { return scale_; }

        // ── Setter — settano dirty automaticamente ────────────────────
        void setPosition(const glm::vec3& v) { position_ = v; dirty_ = true; }
        void setRotation(const glm::vec3& v) { rotation_ = v; dirty_ = true; }
        void setScale(const glm::vec3& v) { scale_ = v; dirty_ = true; }

        void translate(const glm::vec3& delta) { position_ += delta; dirty_ = true; }
        void rotate(const glm::vec3& delta) { rotation_ += delta; dirty_ = true; }
        void scaleBy(const glm::vec3& delta) { scale_ *= delta; dirty_ = true; }

        // ── Model matrix — ricalcolata solo se dirty ──────────────────
        const glm::mat4& getModelMatrix() const {
            if (dirty_) {
                cached_ = glm::mat4(1.0f);
                cached_ = glm::translate(cached_, position_);
                cached_ = glm::rotate(cached_, glm::radians(rotation_.x), { 1,0,0 });
                cached_ = glm::rotate(cached_, glm::radians(rotation_.y), { 0,1,0 });
                cached_ = glm::rotate(cached_, glm::radians(rotation_.z), { 0,0,1 });
                cached_ = glm::scale(cached_, scale_);
                dirty_ = false;
            }
            return cached_;
        }

        // ── Dirty flag ────────────────────────────────────────────────
        bool isDirty() const { return dirty_; }
        void clearDirty() { dirty_ = false; }

        // ── Composizione ──────────────────────────────────────────────
        Transform operator+(const Transform& other) const {
            return Transform(
                position_ + other.position_,
                rotation_ + other.rotation_,
                scale_ * other.scale_
            );
        }

        Transform& operator+=(const Transform& other) {
            position_ += other.position_;
            rotation_ += other.rotation_;
            scale_ *= other.scale_;
            dirty_ = true;
            return *this;
        }

    private:
        glm::vec3 position_{ 0, 0, 0 };
        glm::vec3 rotation_{ 0, 0, 0 };
        glm::vec3 scale_{ 1, 1, 1 };

        mutable bool      dirty_ = true;
        mutable glm::mat4 cached_ = glm::mat4(1.0f);
    };

} // namespace OnYuu