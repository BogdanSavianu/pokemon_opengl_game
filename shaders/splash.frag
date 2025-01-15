#version 410 core

out vec4 FragColor;

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float circle = dot(circCoord, circCoord);
    if (circle > 1.0) {
        discard;
    }
    
    FragColor = vec4(0.9, 0.9, 1.0, 0.4);
} 