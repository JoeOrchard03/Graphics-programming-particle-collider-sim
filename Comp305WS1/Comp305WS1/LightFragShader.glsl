#version 330 core
//Declare output variables using "out"
out vec4 FragColor;

//material struct contains the necessary material values that affect the lighting
struct Material {
    //sampler2D will let us make a diffuse map so can have different diffuse values for different materials
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

//Light struct controls the light
struct Light{
    //Position of light
    vec3 position;
    //Direction light is facing
    vec3 direction;
    //Cut off angle of centre of the the spotlight
    float cutOff;
    //Cut off angle of outer spotlight ring
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //Variables for attenuation formula (makes light get dimmer further from the source)
    float constant;
    float linear;
    float quadratic;
};

uniform Light light;

//Get the normals of the cube
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

//Position of the camera
uniform vec3 viewPos;

void main()
{
    //ambient light
    //Get the fragments diffuse value from the texture and add it to the calc
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    //diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    //Gets the direction of the 
    //dot product of the norm and light direction creates the diffuse effect add a max becaues once it goes beyond 90 it messes up
    float diff = max(dot(norm, lightDir), 0.0);
    //Get the fragments diffuse value from the texture and add it to the calc
    vec3 diffuse = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords)); 

    //Specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //get the dot product of view direction and reflect direction make sure it is not negative using max and raise it to the power of 32
    //this affects the sharpness of the reflection
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
        
    //Check if the area to light is inside the cone of the spotlight
    float theta = dot(lightDir, normalize(-light.direction));
    //Cosine difference between the inner and outer cone of the spotlight
    float epsilon = light.cutOff - light.outerCutOff;
    //Intensity of the spotlight at the current fragment
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    //Apply the intensity of the spotlight to the lighting effects aside from ambient:
    diffuse *= intensity;
    specular *= intensity;

    //Attenuation (makes light further from point light dimmer)
    float distance = length(light.position - FragPos);
    //Attentation formula
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //Applies attenuation to light effects:
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //applies the result of all the lighting methods to the objects color
    vec3 result = ambient + diffuse + specular;
    //applies the result of the objects color with the lighting to the cube
    FragColor = vec4(result, 1.0);

}  