#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in float size;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 windDirection;
uniform float windStrength;

out float visibility;
out vec3 debugPosition;

void main() {
    debugPosition = position;
    
    vec3 windOffset = windDirection * windStrength * 0.1;
    vec3 windAffectedPosition = position + windOffset;
    
    vec4 viewPos = view * vec4(windAffectedPosition, 1.0);
    gl_Position = projection * viewPos;
    
    float distance = length(viewPos.xyz);
    gl_PointSize = clamp(size * 800.0 / distance, 2.0, 8.0);
    
    visibility = clamp(1.0 - (distance / 2000.0), 0.2, 1.0);
}