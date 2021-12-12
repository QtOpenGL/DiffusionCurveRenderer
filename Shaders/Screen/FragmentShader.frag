#version 430 core

in vec2 fs_TextureCoords;
uniform sampler2D sourceTexture;
uniform float widthRatio;
uniform float heightRatio;

out vec4 out_Color;

void main()
{
    vec2 textureCoords = vec2(fs_TextureCoords.s * widthRatio, fs_TextureCoords.t * heightRatio);
    out_Color = texture(sourceTexture, textureCoords).rgba;
}
