//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "camera/Camera2D.h"
#include "BaseScene.h"

namespace Game101_HW4
{

class GeometryScene : public BaseScene
{
public:
    static constexpr const char* ID = "04 Geometry";

    static SceneRef create();

    ~GeometryScene() override;
    GeometryScene(const GeometryScene&) = delete;
    GeometryScene& operator=(const GeometryScene&) = delete;

private:
    math::Vec2  naive_bezier(float t);
    math::Vec2 recursive_bezier(float t);
    void bezier(bool native);
    void updatePoints();

    GeometryScene();
    bool init();
    void onMouseEvent(const MouseEvent* e) override;

    void draw() override;
    void reset() override;
    void drawSettings() override;


    int drawType{0};
    bool drawIndicator{true};

    math::Vec2 points[4]{};
    float step{ 0.01f};
    int activeIndex{-1};
    GLuint ctrlVao{0};
    GLuint ctrlVbo{0};

    GLuint vao{0};
    GLuint vbo{0};
    int vertexCount{0};

    math::Vec2 lastMousePos{};
};

}