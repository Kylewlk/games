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
    if (depth > 50)
    {
        return {0.0f, 0.0f, 0.0f};
    }

    // TO DO Implement Path Tracing Algorithm here
    Intersection intersection = Scene::intersect(ray);
    if (!intersection.happened)
    {
        return this->backgroundColor;
    }

    // std::cout<<1<<std::endl;
    Vector3f hitPoint = intersection.coords;
    Vector3f N = intersection.normal; // normal
    Material *m = intersection.m;
    // if(m->hasEmission())
    //     std::cout<<1<<std::endl;

    // init L_dir and L_indir
    Vector3f L_dir(0.0), L_indir(0.0);

    // Uniformly sample the light at x (pdf_light = 1 / A)
    Intersection intersection_light;
    float pdf_light;
    sampleLight(intersection_light, pdf_light);
    // std::cout<<intersection_light.coords.x<<" "<<intersection_light.coords.y<<" "<<intersection_light.coords.z<<std::endl;

    // Shoot a ray from p to x
    Vector3f dir_p_x = (intersection_light.coords - hitPoint).normalized();
    Ray ray_p_x(hitPoint + EPSILON * N, dir_p_x);
    Intersection intersection_p_x = Scene::intersect(ray_p_x);

    // If the ray is not blocked in the middle
    if(intersection_p_x.happened && intersection_p_x.m->hasEmission()) {
        Vector3f NN = intersection_p_x.normal;
        L_dir = intersection_p_x.m->m_emission // 光源radiance
                * m->eval(ray.direction, dir_p_x, N) // BRDF
                * dotProduct(dir_p_x, N) // 表面 costθ, (Theta)
                * dotProduct(-dir_p_x, NN) // 光源方向 costθ, (Theta)
                / (float)intersection_p_x.distance // 光源衰减 distance 记录了距离的平方， 参考：Triangle::getIntersection
                / pdf_light;
    }

    // Test Russian Roulette with probability RussianRoulette
    if(get_random_float() <= RussianRoulette) {
        // Trace a ray r(p, wi)
        Vector3f dir_i = m->sample(ray.direction, N).normalized();
        Ray ray_p_diri(hitPoint, dir_i);
        Intersection intersection_p_diri = Scene::intersect(ray_p_diri);

        // If ray r hit a non-emitting object at q
        if(intersection_p_diri.happened && !intersection_p_diri.m->hasEmission()) {
            L_indir = castRay(ray_p_diri, depth+1) // 间接光 radiance
                      * m->eval(ray.direction, dir_i, N) // BRDF
                      * dotProduct(dir_i, N) // 表面 costθ, (Theta)
                      / m->pdf(ray.direction, dir_i, N)
                      / RussianRoulette;
        }
    }

    return m->getEmission() + L_dir + L_indir;
}



}