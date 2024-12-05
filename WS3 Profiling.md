After adding the feature of multiple model loading to the artefact I then proceeded to stress test it by loading and testing different numbers of models.

### 1 model loaded:

![alt text](image-2.png)

Average cpu usage of 5 percent with a peak of 6 percent through a 30 second run time.

![alt text](image-3.png)

Just like before I added the multiple model loading the hot path points to nvoglv64.dll using the majority of resources required by the artefact. This is the a link library responsible for linking openGL with nvidia GPUs. There is not much room for optimization here as this is necessary to make openGL work on nvidia GPUs.

### 10 modles loaded:

![alt text](image-4.png)

Again average cpu use of 5 percent however with a peak of 7 percent instead of 6 seconds through a 30 second run time.

![alt text](image-5.png)

Hot path does not show any substantial change, however the link library used around an extra 0.2 percent of usage, will contiune and see if this keeps increasing with amount of models.

### 100 modles loaded:

![alt text](image-6.png)

Average usage is still 5 percent however the peak has increased again to 8 percent over the 30 second run time.

![alt text](image-7.png)

As expected the link library is still using the most of the resources, however it's usage has gone down instead of increasing which I was not expecting, this is likely due to the "glm::operator* <float,0>" operation which has over taken the sld2_image call for the second most resource heavy operation. Increasing from 0.12 to 0.92 when going from 10 to 100 models. I would expect this value to keep increasing as I keep adding models.

### 1,000 models loaded:

![alt text](image-8.png)

Average use is still 5 percent and the peak has not increased by 8 percent. I was expecting it to increase with the amount of models again.

![alt text](image-9.png)
![alt text](image-10.png)

When increasing to loading 1000 models the "glm::operator* <float,0>" usage has multiplied by almost 10. I expect it to continue to increase as more models are added.

### 10,000 models loaded

![alt text](image-11.png)

Average use at 10,000 models is 13 percent with a peak of 17 percent this is what I was expecting but am suprised it took until 10k to see a substantial increase.

![alt text](image-12.png)
![alt text](image-13.png)

Looking at the hot path it is showing that the "glm::operator* <float,0>" usage has increased to take up the majority of the usage, so much so that the linker library does not show on the hot graph until much further down at 1.36 percent usage. This is likely due to all the models needing to do positon cals to decide where to spawn as well as constant position updates to move towards the glass.

### 100,000 models loaded

![alt text](image-14.png)

When trying to load 100,000 models it appears that this reaches the maximum memory allowed and produces a stack overflow exception meaning the artefact cannot be tested with models beyond this point.