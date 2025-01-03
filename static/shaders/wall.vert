#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in mat4 instanceTransform;

uniform mat4 mvp;

out vec3 VertexNormal;

void main() {
    VertexNormal = vertexNormal;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
