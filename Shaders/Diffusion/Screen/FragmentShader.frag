#version 430 core

in vec2 fs_TextureCoords;
uniform sampler2D screenTexture;
uniform float frameBufferToTargetWidthRatio;
uniform float frameBufferToTargetHeightRatio;

out vec4 out_Color;

void main()
{

    vec2 textureCoords = vec2(fs_TextureCoords.s / frameBufferToTargetWidthRatio, fs_TextureCoords.t / frameBufferToTargetHeightRatio);
    vec3 color = texture(screenTexture, textureCoords).rgb;

    out_Color = vec4(color, 1.0);
}
