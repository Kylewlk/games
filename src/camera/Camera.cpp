//
// Created by DELL on 2021/11/12.
//

#include "Camera.h"

using namespace math;
using namespace glm;

void Camera::lookAt(const math::Vec3& eye, const math::Vec3& center, const math::Vec3& up_)
{
    this->position = eye;
    this->target = center;
    this->up = up_;

    this->isNeedUpdate = true;
}

void Camera::lookAt(const math::Vec3& eye, const math::Vec3& center)
{
    this->lookAt(eye, center, math::Y_Axis);
}

void Camera::setViewSize(float width, float height)
{
    if(!equal(viewWidth, width))
    {
        this->viewWidth = width;
        this->isNeedUpdate = true;
    }
    if(!equal(viewHeight, height))
    {
        this->viewHeight = height;
        this->isNeedUpdate = true;
    }
}

void Camera::setNearFar(float zNear, float zFar)
{
    if(!equal(nearPlane, zNear))
    {
        this->nearPlane = zNear;
        this->isNeedUpdate = true;
    }
    if(!equal(farPlane, zFar))
    {
        this->farPlane = zFar;
        this->isNeedUpdate = true;
    }
}

void Camera::setViewPosition(const Vec3& pos)
{
    if (pos != this->position)
    {
        this->position = pos;
        this->isNeedUpdate = true;
    }
}

void Camera::setViewTarget(const Vec3& t)
{
    if (t != this->target)
    {
        this->target = t;
        this->isNeedUpdate = true;
    }
}

void Camera::setViewUp(const Vec3& u)
{
    if (u != this->up)
    {
        this->up = u;
        this->isNeedUpdate = true;
    }
}

math::Vec2 Camera::screenToWorld(const math::Vec2& point) const
{
    float x = (point.x / viewWidth) * 2.0f - 1.0f;
    float y = (point.y / viewHeight) * 2.0f - 1.0f;
    auto mat = glm::inverse(this->viewProj);

    auto world = mat * math::Vec4{x, y, 0.0f, 1.0f};
    return {world.x, world.y};
}

math::Vec2 Camera::worldToScreen(const math::Vec3& point) const
{
    auto clip = this->viewProj * math::Vec4{point, 1.0f};
    float x = ((clip.x / clip.w) + 1.0f) * 0.5f * viewWidth;
    float y = ((clip.y / clip.w) + 1.0f) * 0.5f * viewHeight;
    return {x, y};
}


void Camera::move(const math::Vec3& step)
{
    Vec3 const zDir(normalize(position - target));
    Vec3 const xDir(normalize(cross(up, zDir)));
    Vec3 const yDir(cross(zDir, xDir));
    Vec3 offset{};
    offset = (xDir * (step.x * updateSpeed));
    offset += (yDir * (step.y * updateSpeed));
    offset += (zDir * (step.z * updateSpeed));

    this->position -= offset;
    this->target -= offset;

    this->isNeedUpdate = true;
}
