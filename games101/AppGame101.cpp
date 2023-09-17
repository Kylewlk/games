//
// Created by wlk12 on 2023/9/14.
//

#include "App.h"

#include "scene/PictureScene.hpp"
#include "1_Transform/TransformScene.h"
#include "2_Rasterize/RasterizerScene.h"
#include "3_Shading/ShadingScene.h"
#include "4_Geometry/GeometryScene.h"
#include "5_RayIntersection/RayIntersectionScene.h"

#define ADD_SCENE_MENU(scene) {scene::ID, &scene::create}

AppMenu mainMenus[]{
    ADD_SCENE_MENU(PictureScene),
    ADD_SCENE_MENU(TransformScene),
    ADD_SCENE_MENU(RasterizerScene),
    ADD_SCENE_MENU(Game101_HW3::ShadingScene),
    ADD_SCENE_MENU(Game101_HW4::GeometryScene),
    ADD_SCENE_MENU(Game101_HW5::RayIntersectionScene),
};

int mainMenuCount = sizeof(mainMenus)/(sizeof (mainMenus[0]));
