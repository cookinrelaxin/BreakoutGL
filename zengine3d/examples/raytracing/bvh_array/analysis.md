time complexity is currently around O(screenWidth * screenHeight * numSamples * numObjects)

it's actually worse than this because the photons scatter among the objects recursively until one flies off into space

assuming a single core architecture, how could this be improved?

i dont see any getting around looping through each pixel.

an obvious way to improve the performance is to use some sort of acceleration structure for the intersection testing

currently the time complexity of the intersections testing is O(numObjects). a Bounding Volume Hierarchy could reduce this to O(log(numObjects))

then the path tracing algorithm would have an improved time complexity of O(screenWidth * screenHeight * numSamples * log(numObjects))

parallelism should further improve the time complexity of the render.

~~~~
implemented bvh. it's definitely faster. profiling with valgrind now to see for low hanging fruit improvements.

havent yet compared performance benchmarks with linear search intersection testing.

~~~~
according to valgrind, 92.10% of cpu instructions occurred in bvh_node::hit.
This is the obvious bottleneck. now lets look deeper.

aabb::hit accounted for 75.86% of instructions within bvh_node::hit

ultimately a lot comes down to glm::length().

~~~ 
still O(screenWidth * screenHeight * numSamples * log(numObjects))

parallelism should help enormously

even with numSamples = 1, a scene with 500 spheres at 1024x512 takes 14.15s to render.
at least 2 orders of magnitude of improvement are required for 60 fps. (.14s render times)

~~~
with -O3 level optimization enabled with clang, performance to render a 1600x800 scene with 500 balls and one sample was cut down ~50% from 34.50s to 16.18s

inlining ray::origin, ray::direction, and ray::point_at_parameter reduced time further to 11.85s

~~~
converting the bounding volume hierarchy from a linked list representation to an array based representation should improve performance by being more cache friendly
