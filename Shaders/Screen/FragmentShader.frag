#version 430 core

in vec2 fs_TextureCoords;
uniform sampler2D sourceTexture;

out vec4 out_Color;

void main()
{
    vec2 textureCoords = vec2(fs_TextureCoords.s, -fs_TextureCoords.t);
        out_Color = texture(sourceTexture, textureCoords).rgba;
}
