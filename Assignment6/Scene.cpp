//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    printf("%d\n",objects.size());
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
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

// Implementation of the Whitted-syle light transport algorithm (E [S*] (D|G) L)
//
// This function is the function that compute the color at the intersection point
// of a ray defined by a position and a direction. Note that thus function is recursive (it calls itself).
//
// If the material of the intersected object is either reflective or reflective and refractive,
// then we compute the reflection/refracton direction and cast two new rays into the scene
// by calling the castRay() function recursively. When the surface is transparent, we mix
// the reflection and refraction color using the result of the fresnel equations (it computes
// the amount of reflection and refractin depending on the surface normal, incident view direction
// and surface refractive index).
//
// If the surface is duffuse/glossy we use the Phong illumation model to compute the color
// at the intersection point.
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    //一开始其实就是在收集交点的信息
    if (depth > this->maxDepth) {
        return Vector3f(0.0,0.0,0.0);
    }
    Intersection intersection = Scene::intersect(ray);
    Material *m = intersection.m;
    Object *hitObject = intersection.obj;
    Vector3f hitColor = this->backgroundColor;
//    float tnear = kInfinity;
    Vector2f uv;
    uint32_t index = 0;
    if(intersection.happened) {

        Vector3f hitPoint = intersection.coords;
        Vector3f N = intersection.normal; // normal
        Vector2f st; // st coordinates
        hitObject->getSurfaceProperties(hitPoint, ray.direction, index, uv, N, st);
//        Vector3f tmp = hitPoint;
        //寻找交点颜色
        switch (m->getType()) {
            case REFLECTION_AND_REFRACTION:
            {
                Vector3f reflectionDirection = normalize(reflect(ray.direction, N));
                Vector3f refractionDirection = normalize(refract(ray.direction, N, m->ior));
                Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
                                             hitPoint - N * EPSILON :
                                             hitPoint + N * EPSILON;
                Vector3f refractionRayOrig = (dotProduct(refractionDirection, N) < 0) ?
                                             hitPoint - N * EPSILON :
                                             hitPoint + N * EPSILON;
                Vector3f reflectionColor = castRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1);
                Vector3f refractionColor = castRay(Ray(refractionRayOrig, refractionDirection), depth + 1);
                float kr;
                fresnel(ray.direction, N, m->ior, kr);
                hitColor = reflectionColor * kr + refractionColor * (1 - kr);
                break;
            }
            case REFLECTION:
            {
                float kr;
                fresnel(ray.direction, N, m->ior, kr);
                Vector3f reflectionDirection = reflect(ray.direction, N);
                Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
                                             hitPoint + N * EPSILON :
                                             hitPoint - N * EPSILON;
                hitColor = castRay(Ray(reflectionRayOrig, reflectionDirection),depth + 1) * kr;
                break;
            }
            default:
            {
                // [comment]
                // We use the Phong illumation model int the default case. The phong model
                // is composed of a diffuse and a specular reflection component.
                // [/comment]
                Vector3f lightAmt = 0, specularColor = 0;
                //为了处理浮点数的精度问题。<0的时候，说明hitpoint在镜子外，加上一个epsilon, 确保其肯定在镜子外，不会因为浮点数误判在镜子内
                Vector3f shadowPointOrig = (dotProduct(ray.direction, N) < 0) ?
                                           hitPoint + N * EPSILON :
                                           hitPoint - N * EPSILON;
                // [comment]
                // Loop over all lights in the scene and sum their contribution up
                // We also apply the lambert cosine law
                // [/comment]
                for (uint32_t i = 0; i < get_lights().size(); ++i)
                {
                    auto area_ptr = dynamic_cast<AreaLight*>(this->get_lights()[i].get());
                    if (area_ptr)
                    {
                        // Do nothing for this assignment
                    }
                    else
                    {
                        Vector3f lightDir = get_lights()[i]->position - hitPoint;
                        float lightDistance2 = dotProduct(lightDir, lightDir);
                        // square of the distance between hitPoint and the light
                        lightDir = normalize(lightDir);
                        float LdotN = std::max(0.f, dotProduct(lightDir, N));
                        //Object *shadowHitObject = nullptr;
                        //float tNearShadow = kInfinity;
                        Intersection p = bvh->Intersect(Ray(shadowPointOrig, lightDir));
                        //有可能遮挡物在光源的后面，这个时候虽然会相交，但是不会有遮挡
                        bool inShadow = (p.happened) && (p.distance < lightDistance2);
                        lightAmt += (1 - inShadow) * get_lights()[i]->intensity * LdotN;

                        Vector3f reflectionDirection = reflect(-lightDir, N);
                        specularColor += (1 - inShadow)*powf(std::max(0.f, dotProduct(reflectionDirection, -ray.direction)),
                                              m->specularExponent) * get_lights()[i]->intensity;
                        //按理说，是要除以距离的，但是这里距离太大，一般就不乘了   
                    }
                }
                //因为我们当前的材质比较复杂，因此object_color并不是每一个点都一样的
                //hitObject->evalDiffuseColor(st)使用这个可以让材质上每个点的颜色都不同
                //kd,ks在这里意思改变了，其表示的是一个比例系数，即为算出光照后，在这里再增强/削弱一点。
                hitColor = lightAmt * hitObject->evalDiffuseColor(st) * m->Kd  + specularColor * hitObject->evalDiffuseColor(st)* m->Ks;
                break;
            }
        }
    }

    return hitColor;
}