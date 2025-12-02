#version 330 core
out vec4 FragColor;
in vec4 color; // input color from vertex shader
void main()
{
	FragColor = color;
}