#version 460 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D uTexture;

void main()
{
    FragColor = texture(uTexture, texCoord) * (vertexColor / 2.0f + 0.5f);
}