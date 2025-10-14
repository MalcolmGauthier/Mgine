#version 460 core

in vec3 oNormal;
in vec2 oTexCoord;
in vec3 oFragPos;

out vec4 outputColor;

uniform sampler2D texture1;
uniform vec3 camPos;

void main(void)
{
    // rainbow mode
	//outputColor = vec4(abs(normalize(oNormal)), 1.0);

    vec3 ambient = vec3(1);
  	
    vec3 norm = normalize(oNormal);
    vec3 lightDir = normalize(camPos - oFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(diff);
    
    vec3 viewDir = normalize(camPos - oFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10);
    vec3 specular = vec3(spec);  
        
    vec3 result = ambient + diffuse + specular;

	outputColor = texture(texture1, oTexCoord) * vec4(result, 1);
}