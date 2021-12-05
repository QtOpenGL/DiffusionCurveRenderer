#version 430 core
out vec4 gl_Color;
in vec2 fs_TextureCoords;
uniform sampler2D fs_Texture;

void main()
{
    vec2 textureCoords = vec2(fs_TextureCoords.s, fs_TextureCoords.t);
    vec3 color = texture(fs_Texture, textureCoords).rgb;

    gl_Color = vec4(color, 1.0);
}
