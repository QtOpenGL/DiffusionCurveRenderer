#version 430 core

in vec2 fs_TextureCoords;
uniform sampler2D fs_Texture;
uniform float widthRatio;
uniform float heightRatio;

out vec4 out_Color;

void main()
{
    vec2 textureCoords = vec2(fs_TextureCoords.s * widthRatio, fs_TextureCoords.t * heightRatio);
    out_Color = texture(fs_Texture, textureCoords).rgba;
}
