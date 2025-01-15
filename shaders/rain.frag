#version 410 core

in float visibility;
out vec4 FragColor;
in vec3 debugPosition;

void main() {
    // Create circular raindrop shape
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float r = length(coord);
    
    // Discard pixels outside the circle
    if (r > 1.0) discard;
    
    // Create a soft, round edge
    float alpha = (1.0 - r * r) * 0.8;
    
    // Apply distance fade
    alpha *= visibility;
    
    // Slightly elongate vertically by scaling the alpha
    alpha *= 1.0 + coord.y * 0.5;  // Subtle vertical elongation
    
    // Bright, visible color with smooth falloff
    FragColor = vec4(0.8, 0.9, 1.0, alpha * 0.8);
    
    // Optional: print debug info
    // This won't actually print, but can help confirm
    if (gl_FragCoord.x < 10 && gl_FragCoord.y < 10) {
        FragColor = vec4(debugPosition, 1.0);
    }
}