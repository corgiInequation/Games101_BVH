# Games101 Experiment 6

## Experiment Content

- Based on the Whitted-style ray tracing implemented in Experiment 5, this task introduces BVH (Bounding Volume Hierarchy) acceleration. In Experiment 5, the scene only contained two spheres and one plane, so it was feasible to check intersections by brute force.
- However, in Experiment 6, the model contains a large number of triangles. Brute-force traversal would be extremely time-consuming.
- Therefore, we need to manually construct a BVH tree. This is a binary tree where only the leaf nodes store actual triangles. Internal nodes typically store bounding boxes. When checking whether a ray intersects with the scene, the algorithm essentially traverses the BVH tree to accelerate the process.

## Experiment Result

- <img src="https://github.com/corgiInequation/Games101_BVH/blob/main/image.png" alt="bvh-result-1" width="67%" />
- <img src="https://github.com/corgiInequation/Games101_BVH/blob/main/image2.png" alt="bvh-result-2" width="50%" />
- As shown, for complex shapes like the one above, BVH acceleration enables high-quality rendering within approximately 5 seconds.
