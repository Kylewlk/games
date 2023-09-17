//
// Created by LEI XU on 5/16/19.
//
#pragma once

#include "Vector.hpp"
#include "Material.hpp"

namespace Game101_HW6
{

class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false;
        coords=Vector3f();
        normal=Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened;
    Vector3f coords;
    Vector3f normal;
    double distance;
    Object* obj;
    Material* m;
};

}
