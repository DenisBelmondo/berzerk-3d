#version 330

in vec3 VertexNormal;
out vec4 finalColor;

void main() {
    float multiplier = abs(dot(vec3(0.5, 0.75, 1), VertexNormal));
    finalColor = vec4(vec3(0, 0, 0.5) * multiplier, 1);
}
