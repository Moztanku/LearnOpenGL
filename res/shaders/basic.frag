#version 460 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D uTexture_0;
uniform sampler2D uTexture_1;
uniform float uMix;

void main()
{
    FragColor = mix(texture(uTexture_0, texCoord), texture(uTexture_1, texCoord), uMix);
}