#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main(int argc, char** argv)
{
    Scene scene(1280, 960);

    //mesh triangle即为全部的三角形的信息
    //同时，还初始化了一个bound3和一颗bvh树。
    //注意，不只是scen中有bvh树，每个mesh也有一颗bvh树
    MeshTriangle bunny("D:\\learning_codes\\games_work\\PA6\\PA6\\Assignment6\\models\\bunny\\bunny.obj");
    
    //bunny继承了objct，因此也可以作为object加入scene
    //scene也在下列代码中初始化了一个bvh树
    scene.Add(&bunny);
    scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 1));
    scene.Add(std::make_unique<Light>(Vector3f(20, 70, 20), 1));
    scene.buildBVH();//这个时候，只有一个object，BVH中只有一个结点，object是mesh_triangle

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}