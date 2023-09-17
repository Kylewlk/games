//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


namespace Game101_HW7
{


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    if (depth > this->maxDepth) {
        return {0.0,0.0,0.0};
    }

    Intersection intersection = this->intersect(ray);
    if (!intersection.happened)
    {
        return this->backgroundColor;
    }

    Material *m = intersection.m;
    Object *hitObject = intersection.obj;

    if (m->hasEmission())
    {
        return m->getEmission();
    }

    Vector3f L_dir{0.0f, 0.0f, 0.0f};
    Vector3f L_indir{0.0f, 0.0f, 0.0f};

    Intersection lightIntersect;
    float lightPdf{};
    sampleLight(lightIntersect, lightPdf);
    auto lightVector = lightIntersect.coords - intersection.coords;
    auto lightDir = lightVector.normalized();
    Ray obj2LightRay{intersection.coords, lightDir};
    auto middleLight = this->intersect(obj2LightRay);
    if (middleLight.obj == lightIntersect.obj)
    {
        auto f_r = m->eval(ray.direction, lightDir, intersection.normal);
        auto dis = std::sqrt(dotProduct(lightVector, lightVector));
        L_dir = lightIntersect.emit * f_r * dotProduct(lightDir, intersection.normal)
                * dotProduct(-lightDir, lightIntersect.normal) / dis / lightPdf;
    }

    if (get_random_float() > RussianRoulette)
    {
        return L_dir;
    }

    auto wo = m->sample(ray.direction, intersection.normal).normalized();
    Ray obj2ObjRay{intersection.coords, wo};
    Intersection intersection2 = this->intersect(ray);
    if (intersection2.happened && !intersection2.m->hasEmission())
    {
        float pdf = intersection.m->pdf(ray.direction, wo, intersection.normal);
        auto f_r = intersection.m->eval(ray.direction, wo, intersection.normal);
        L_indir = castRay(obj2ObjRay, depth + 1) * f_r * dotProduct(wo, intersection.normal) / pdf / RussianRoulette;
    }

    return L_dir + L_indir;
}

}