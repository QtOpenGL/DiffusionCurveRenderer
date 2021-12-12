#version 430 core

in vec2 fs_TextureCoords;

uniform sampler2D sourceTexture;
uniform float targetWidth;
uniform float targetHeight;

out vec4 out_Color;

void main()
{

    float uStep = 1 / targetWidth;
    float vStep = 1 / targetHeight;


    // nw n ne
    // w  c e
    // sw s se

    vec2 c = vec2(fs_TextureCoords.s, fs_TextureCoords.t);

    vec2 nw = vec2(fs_TextureCoords.s - uStep, fs_TextureCoords.t + vStep);
    vec2 n = vec2(fs_TextureCoords.s, fs_TextureCoords.t + vStep);
    vec2 ne = vec2(fs_TextureCoords.s + uStep, fs_TextureCoords.t + vStep);

    vec2 w = vec2(fs_TextureCoords.s - uStep, fs_TextureCoords.t);
    vec2 e = vec2(fs_TextureCoords.s + uStep, fs_TextureCoords.t);

    vec2 sw = vec2(fs_TextureCoords.s - uStep, fs_TextureCoords.t - vStep);
    vec2 s = vec2(fs_TextureCoords.s, fs_TextureCoords.t - vStep);
    vec2 se = vec2(fs_TextureCoords.s + uStep, fs_TextureCoords.t - vStep);

    vec2 vectors[9];
    vectors[0] = nw;
    vectors[1] = n;
    vectors[2] = ne;
    vectors[3] = w;
    vectors[4] = c; // center
    vectors[5] = e;
    vectors[6] = sw;
    vectors[7] = s;
    vectors[8] = se;


    float weights[9];
    weights[0] = 1;
    weights[1] = 2;
    weights[2] = 1;
    weights[3] = 2;
    weights[4] = 2; // center
    weights[5] = 2;
    weights[6] = 1;
    weights[7] = 2;
    weights[8] = 1;



    vec4 color = vec4(0,0,0,0);

    for(int i = 0; i < 9; i++)
    {
        color += weights[i] * texture(sourceTexture, vectors[i]).rgba / 14.0f;
    }

    out_Color = color;

}
