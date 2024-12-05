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

As expected the link library is still using the most of the resources, however it's usage has gone down instead of increasing which I was not expecting. Also the "glm::operator* <float,0>" operation has over taken the sld2_image call for the second most resource heavy operation. Increasing from 0.12 to 0.92 when going from 10 to 100 models. I would expect this value to keep increasing as I keep adding models.

### 1000 models loaded:

