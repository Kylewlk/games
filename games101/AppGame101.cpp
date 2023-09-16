//
// Created by wlk12 on 2023/9/14.
//

#include "App.h"

#include "scene/PictureScene.hpp"
#include "1_Transform/TransformScene.h"
#include "2_Rasterize/RasterizerScene.h"
#include "3_Shading/ShadingScene.h"

#define ADD_SCENE_MENU(scene) {scene::ID, &scene::create}

AppMenu mainMenus[]{
    ADD_SCENE_MENU(PictureScene),
    ADD_SCENE_MENU(TransformScene),
    ADD_SCENE_MENU(RasterizerScene),
    ADD_SCENE_MENU(Game101_HW3::ShadingScene),
};

int mainMenuCount = sizeof(mainMenus)/(sizeof (mainMenus[0]));
