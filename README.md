

**University of Pennsylvania, CIS 599: Independent Study, Progressive Photon Mapping**
======================


* Yaoyi (Elyson) Bai
* byaoyi

### TODO: Your README

The structure of the Progressive Photon Mapping algorithm is:

![WorkFlowChat](https://lh3.googleusercontent.com/-YOInK9my1aQ/WhRWmL_awAI/AAAAAAAABR8/onxjLVKB8yIVGK8I8JAIEB8NoSNU27h2wCLcBGAs/s0/workflow.png "workflow.png")

### **Implementation Details** 

The approach to implement Progressive Photon Mapping is basically the same as the flow chart shown above. However, there are still some differences:

#### 1. The result are rendered based on a decomposition of Progressive Photon Mapping. 

In this project, the progressive photon mapping is implemented based on the method from PBRT 3rd Edition. 

In Chapter 16.2, the LTE function are divided into two parts, the direct lighting and the indirect light. The formula could be written as:

![enter image description here](https://lh3.googleusercontent.com/-rVCG1XwAhW8/WjL0eHWDARI/AAAAAAAABUU/PV1jZy93DaEXm_sLjUsil5eDevqGxr4BwCLcBGAs/s0/formular.gif "formular.gif")

For the accuracy of the result, the first photon trace is replaced by the uniform version of direct lighting integrator. However, a new improvement of implementation method is needed to get better result. Because there is no sampling in ray tracing, the rendered result is very sparse, which photon tracing can slightly compensate if many photons are traced each time, like 500,000 photons. 

#### 2. Photons Only Contributes to Indirect Colors

The direct color is calculate through uniform direct lighting, so photon trace will only contribute to the indirect lighting part of the result. For each non-specular photon trace, if it is the first trace, we only spawn the ray and calculate the bounce color and then continue the while loop. For traces other than the first trace, do color accumulation, and divide the accumulated color over the total number of new photons traced and finally shrink the radius of the searching ares by calculate the density of the given searching area of each hitPoints. 

#### 3. Searching Radius Decision 

The paper does not mention how to decide searching radius of each hitPoints. However, the searching radius is very important for the final result. 

The searching strategy for hitPoints are based on the method applied to Photon Mapping, which is the KdTree struct. Therefore, if the searching radius is badly decided, it is possible that all the photons will only accumulates in only one hitPoints. Therefore, the searching radius should be carefully decided. 

In this project, the radius is decided through the scene size in camera space over the number of pixels in x-axis. The idea is to let each pixel (i.e. all the hitPoints) "covers" all the possible photon influences around its neighboring area. This method is tricky because it is similar to manually decide a static number in a program, but basically it works well. 

There are some rendered results have some grid-like error, and this is the result of badly chosen searching radius. 

Also, since it is the tree structure, the hitPoints are not linearly arranged, therefore the will not be any radius shrinking effect when the scenes are rendering. But it is very apparent that the walls are gradually "lighten" when there is a proper radius given and several photon traces. 

#### 4. Linear Index of Each HitPoints are Stored inside the KdTree Structure

To speed up the nearest hitPoint searching, a linear index integer is added to the KdTree struct, which is different from the normal KdTree. Therefore, the searching function of the KdTree only returns the linear index of the hitPoint that the photon will affect, otherwise the function will return -1.

### **Render Result**

First get the shadow color through ray tracing and direct lighting integrator:

![enter image description here](https://lh3.googleusercontent.com/-FebrvCr-DG4/WjL3yA_K6dI/AAAAAAAABUk/gnlmuzEsKcErJ3zWbR-DydQ1Vdqh0fl5gCLcBGAs/s0/rendered_images7.png "rendered_images7.png")

Then calculate the color of naive direct lighting integrator:

![enter image description here](https://lh3.googleusercontent.com/-q1kXNZ5w6S8/WjL395g-ZuI/AAAAAAAABUs/CHqzrWsxADkh9lgzePukM-yGQU6G1wFkACLcBGAs/s0/rendered_images15.png "rendered_images15.png")

This image looks sparse and dark for several reasons:

- Sampling is needed when calculating the color of the ray tracing with uniform direct lighting, or there will be too many noises inside this scene. 

- There is no global illumination inside this scene currently. The n times of photon tracing would compensate for the final color.

Then photons will be shot into the scene.

20,000 photons with 2 traces

![enter image description here](https://lh3.googleusercontent.com/-6ognVmHsTVQ/WjME4c95kWI/AAAAAAAABVM/YHvsAvIRVREPMMa_BuqxuoSXPrt_zlhuACLcBGAs/s0/2_20000.png "2_20000.png")

40,000 photons with 4 traces

![enter image description here](https://lh3.googleusercontent.com/-hIySpeZULQk/WjME8_yYAWI/AAAAAAAABVU/O1AM-oCpIikbeLCbvyUp_RvvAAjDfPv5wCLcBGAs/s0/4_40000.png "4_40000.png")

80,000 photons with 4 traces

![enter image description here](https://lh3.googleusercontent.com/-dsbecMqpPm0/WjMFBHoOF-I/AAAAAAAABVc/gl6-mz9RJg8MQ3JpS8fGq_M-ZuUn-2drACLcBGAs/s0/4_80000.png "4_80000.png")

Until:

200,000 photons with 4 traces 

![enter image description here](https://lh3.googleusercontent.com/-eNuBVAwv7zw/WjMO86iLstI/AAAAAAAABWA/KYxRhTCrhOQWI9goVCUO-3meBo1xykZBACLcBGAs/s0/4-200000.png "4-200000.png")

1 million photons with 10 traces

![enter image description here](https://lh3.googleusercontent.com/-BIvWJR3jpK4/WjNKIDIG5OI/AAAAAAAABWk/GODoUjhVPpoMglGsGIWLMazFbrWhMdAWACLcBGAs/s0/1million_10.png "1million_10.png")

After some improvement of direct lighting (sampling direct lighting for 100 times and average the result of direct lighting), the direct lighting map looks like:

![enter image description here](https://lh3.googleusercontent.com/-3NZP_HxFZ3g/Wjc5cKVXcVI/AAAAAAAABYw/Edz3h96e9KI_0Y9HVMCJghYSjQ3bp08ewCLcBGAs/s0/rendered_images26.png "rendered_images26.png")

And 1 million photons with 10 traces

![enter image description here](https://lh3.googleusercontent.com/-BZVzKkP9cDU/Wjc4eIt3luI/AAAAAAAABYg/GsAKUpDiQnk0xYKofNrQffhMhZYPfV4wQCLcBGAs/s0/rendered_images25.png "rendered_images25.png")

Still very sparse with bugs?????? Why? 


#### Solutions 

1. Sparse Rendering Result: because of the kd-tree. Perhaps the kd-tree structure does not suit for Progressive Photon Mapping. Therefore, for the accuracy of the result, the acceleration structure, i.e. the kd-tree structure is removed. To attain better result, for each photon that reaches the destination, loop through all the hitpoints in  the scene and find all the possible hitpoints that this photon will influence and revise the color for this hitpoint. 

2. Dark areas beside the light source: because of wrong light source judging method in radius shrinking.

#### Revised Rendered Images

50,000 Photons with 2 traces alpha = 0.6



100,000 Photons with 5 traces alpha = 0.6

![enter image description here](https://lh3.googleusercontent.com/-3L23b7HZ3T4/Wl2Ogwv6fdI/AAAAAAAABbk/i59xbz_FG5Mg33fplbiwU4OMAXVkIE3xQCLcBGAs/s0/rendered_images48.png "rendered_images48.png")

2,000,000 photons with 10 traces alpha = 0.6
(rendering time 8.2 h)

![enter image description here](https://lh3.googleusercontent.com/-SB7J-edyG00/Wl4YRR7hWfI/AAAAAAAABcE/nC9KMiVVjd4Avw5qJJO1MD6dsUUv8yyGgCLcBGAs/s0/rendered_images49.png "rendered_images49.png")

### **Future Improvement**

#### 1. Butter HitPoint Sorting Method

To have hitPoint color shrinking effect, linear data structure is essential. In the future version, perhaps dividing the scene or the camera frustum into grids to store to hitPoints or applying hashed structure to implement near hitPoints searching should be implemented to improve the result. 

#### 2. Using MIS To Calculate the Direct Lighting Color or Implementing Sampling for Direction Lighting 

Implementing MIS will of course make the result looks much better, but this method has already taken global illumination into account, and therefore it would not be the best way to implement the direct lighting for photon tracing.

Hence, to solve the "sparse" problem, sampling the direct lighting would be the best way.

#### 3. Implementing Sampling in the First Trace

To make the result more smooth, the direct lighting part should be sampled multiple times to get better result. 

#### 4. Better Searching Radius Calculation Strategy Needed

As mentioned above, the searching radius should be carefully decided. Currently, the idea is to let the hitPoints cover the possible photon influenced areas. But the coverage for 2D space does not guarantee the "coverage" in 3D space, which will cause trouble to the rendered result (like the stripes and grids in some image). Therefore, a crucial problem to solve in the future is how to decide the searching radius. 

#### 5. Photon Reusing 

There are a lot of wasted photons inside the scene, which may take extra rendering time if higher resolution is needed. Therefore, there should be some better strategy to recycle the photons. 

#### 6. What makes the rendered image so sparse?

Have to figure out this problem.

### **Credit To:** 
Basecode: University of Pennsylvania, CIS561 2017 Spring 