# Games101 Experiment 6

## 实验内容

- 在实验五实现Whitted Style Ray tracing 的基础上，实现BVH加速。在实验五中，模型只有两个球面和一个平面，直接遍历判断是否相交即可。
- 但是在实验六中，模型具有大量的三角形，如果直接暴力遍历，会耗时很久。
- 因此，我们需要手动构造一颗BVH树。这个树是一颗二叉树，并且只有叶子结点会存储三角形。中间结点，一般是存储bounding box。在判断光线是否和场景相交的时候，本质上就是遍历一遍BVH树。



## 实验结果

- <img src="C:\Users\i love china\AppData\Roaming\Typora\typora-user-images\image-20250511173148645.png" alt="image-20250511173148645" style="zoom:67%;" />
- <img src="C:\Users\i love china\AppData\Roaming\Typora\typora-user-images\image-20250511173214715.png" alt="image-20250511173214715" style="zoom:50%;" />
- 可以看到，对于上述形状较为复杂的图形，BVH加速后，也可以在5s内渲染出效果不错的图片。