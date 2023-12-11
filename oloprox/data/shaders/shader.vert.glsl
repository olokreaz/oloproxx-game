#version 450

const float PI = 3.1415926535897932384626433832795;

void main() {
	gl_Position = vec4(sin(PI * gl_VertexIndex / 2.0), cos(PI * gl_VertexIndex / 2.0), 0.0, 1.0);
}