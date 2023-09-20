//
// Created by wlk12 on 2023/9/14.
//

#include "App.h"

#include "scene/PictureScene.hpp"
#include "scene/ModelScene.h"
#include "0_Model3DScene/Model3DScene.h"


#define ADD_SCENE_MENU(scene) {scene::ID, &scene::create}

AppMenu mainMenus[]{
    ADD_SCENE_MENU(PictureScene),
    ADD_SCENE_MENU(ModelScene),
    ADD_SCENE_MENU(Model3DScene),
};

int mainMenuCount = sizeof(mainMenus)/(sizeof (mainMenus[0]));
