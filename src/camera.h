#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct camera_t {
    camera_t() { update(); }

    auto view_matrix() -> glm::mat4 {
        return glm::lookAt(position_, position_ + front_, up_);
    }

    auto move(float front, float right, float delta_time) {
        float distance = speed_ * delta_time;
        position_ += front_ * front * distance;
        position_ += right_ * right * distance;
    }

    auto rotate(float x_offset, float y_offset) {
        x_offset *= sensitivity_;
        y_offset *= sensitivity_;

        yaw_ += x_offset;
        pitch_ = std::max(-89.f, std::min(pitch_ + y_offset, 89.f));
        update();
    }

    auto zoom(float off) {
        zoom_ = std::max(1.f, std::min(zoom_ - off, 45.f));
    }

    auto update() -> void {
        front_.x = glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
        front_.y = glm::sin(glm::radians(pitch_));
        front_.z = glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
        front_ = glm::normalize(front_);
        right_ = glm::normalize(glm::cross(front_, glm::vec3{0, 1, 0}));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

    glm::vec3 position_{0, 0, 0}; // 位置
    glm::vec3 front_{0, 0, -1};   // 前向量
    glm::vec3 up_{0, 1, 0};       // 上向量
    glm::vec3 right_{};           // 右向量

    float yaw_{-90.0f}; // 偏航角
    float pitch_{0.0f}; // 俯仰角

    float speed_{2.5f};       // 移动速度
    float sensitivity_{0.1f}; // 旋转零敏度
    float zoom_{45.0f};       // 缩放值
};