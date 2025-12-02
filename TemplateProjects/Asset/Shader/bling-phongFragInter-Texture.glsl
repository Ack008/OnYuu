#version 330 core
out vec4 FragColor;
in vec4 color; // input color from vertex shader
in vec2 texCoord;
uniform sampler2D texture1; // texture sampler
void main()
{
	FragColor = texture(texture1,texCoord) * color;
}
