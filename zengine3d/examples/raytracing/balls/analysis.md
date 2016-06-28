time complexity is currently around O(screenWidth * screenHeight * numSamples * numObjects)

it's actually worse than this because the photons scatter among the objects recursively until one flies off into space

assuming a single core architecture, how could this be improved?

i dont see any getting around looping through each pixel.

an obvious way to improve the performance is to use some sort of acceleration structure for the intersection testing

currently the time complexity of the intersections testing is O(numObjects). a Bounding Volume Hierarchy could reduce this to O(log(numObjects))

then the path tracing algorithm would have an improved time complexity of O(screenWidth * screenHeight * numSamples * log(numObjects))

this is explored in the bvh example

an actual mistake in the current implementation is that the
