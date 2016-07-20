progressive path tracing algorithm:
every frame, take one sample per pixel via path tracing. 
write the sampled pixel colors to a screen space quad texture. this texture is updated every frame with the moving average of the samples of previous frames
render the screen space quad with the texture.

radical raster + path tracing algorithm:
every frame, take one sample per pixel via path tracing. 
map the sampled pixel colors onto textures for objects in the scene. these textures are updated every frame with the moving average of the samples of previous frames. to handle dynamic scenes with moving objects and lights, we can use a window function

now is the rasterization step.
do the usual model view projection for each vertex in the scene.
now just render the visible fragments with their texture maps generated via path tracing.

the idea here is that a point on an arbitrary object is likely to be lit at one moment similarly to the previous moment, so there would be very large amounts of recomputation per frame to determine that point's lighting. by cacheing path traced lighting computations with per-object texture maps, we can save lots of computational time.

The problem is to determine when texels do need to be recomputed due to changes in light positions and intensities, object positions, or view positions.
