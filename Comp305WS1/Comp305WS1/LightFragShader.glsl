#version 330 core

//material struct contains the necessary material values that affect the lighting
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

//Light struct controls the light
struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

//Declare output variables using "out"
out vec4 FragColor;

//Get the normals of the cube
in vec3 Normal;
in vec3 FragPos;

//Position of the light source
uniform vec3 lightPos;  
uniform vec3 lightColor;
uniform vec3 objectColor;
//Position of the camera
uniform vec3 viewPos;

void main()
{
    //ambient light
    vec3 ambient = material.ambient * light.ambient;

    //diffuse light
    vec3 norm = normalize(Normal);
    //Gets the direction between the light and the fragment
    vec3 lightDir = normalize(lightPos - FragPos);
    //dot product of the norm and light direction creates the diffuse effect add a max becaues once it goes beyond 90 it messes up
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;

    //Specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //get the dot product of view direction and reflect direction make sure it is not negative using max and raise it to the power of 32
    //this affects the sharpness of the reflection
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;

    //applies the result of all the lighting methods to the objects color
    vec3 result = ambient + diffuse + specular;
    //applies the result of the objects color with the lighting to the cube
    FragColor = vec4(result, 1.0);
}  