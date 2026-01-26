#version 330 core
out vec4 FragColor;
in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;
  
uniform vec3 uLightPos; 
uniform vec3 uViewPos; 
uniform vec3 uLightColor;
uniform sampler2D uDiffMap1;
uniform bool uUseTexture;      // NOVO
uniform vec3 uObjectColor;     // NOVO
uniform float uAlpha;          // NOVO

void main()
{    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;
  	
    // diffuse 
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(uLightPos - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;  
    
    vec3 result = ambient + diffuse + specular;
    
    // NOVO: Koristi teksturu ili boju
    if (uUseTexture) {
        FragColor = texture(uDiffMap1, chUV) * vec4(result, 1.0);
    } else {
        FragColor = vec4(result * uObjectColor, uAlpha);
    }
}