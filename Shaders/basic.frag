#version 330 core
out vec4 FragColor;
in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;
  
uniform vec3 uLightPos2;
uniform vec3 uLightColor2;
uniform bool useSecondLight;


uniform vec3 uLightPos;  // POZICIJA SVJETLA
uniform vec3 uViewPos;   // POZICIJA KAMERE
uniform vec3 uLightColor;// BOJA SVJETLA
uniform sampler2D uDiffMap1;
uniform bool uUseTexture;      // NOVO
uniform vec3 uObjectColor;     // NOVO
uniform float uAlpha;          // NOVO

vec3 CalcLight(vec3 lightPos, vec3 lightColor)
{
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(lightPos - chFragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = 0.5 * spec * lightColor;

    return ambient + diffuse + specular;
}

vec3 CalcLightCoin(vec3 lightPos, vec3 lightColor)
{
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(lightPos - chFragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = 0.5 * spec * lightColor;

    return ambient*0.4f + diffuse*0.5f + specular*1.2f;
}


void main()
{    
    vec3 result = CalcLight(uLightPos, uLightColor);
    if (useSecondLight)
        result += CalcLightCoin(uLightPos2, uLightColor2);
    
    // NOVO: Koristi teksturu ili boju
    if (uUseTexture) {
        FragColor = texture(uDiffMap1, chUV) * vec4(result, 1.0);
    } else {
        FragColor = vec4(result * uObjectColor, uAlpha);
    }
}