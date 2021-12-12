#version 430 core

in vec2 fs_TextureCoords;

uniform sampler2D constrainedTexture;
uniform sampler2D targetTexture;
uniform float targetWidth;
uniform float targetHeight;

out vec4 out_Color;

void main()
{

    vec2 targetPosition = vec2(fs_TextureCoords.s, fs_TextureCoords.t);
    vec4 color = texture(constrainedTexture, targetPosition).rgba;

    if(color.a < 1) // Smooth
    {
        float uStep = 1.0 / targetWidth;
        float vStep = 1.0 / targetHeight;

        // nw n ne
        // w  c e
        // sw s se

        //vec2 nw = vec2(fs_TextureCoords.s - uStep, fs_TextureCoords.t + vStep);
        vec2 n = vec2(fs_TextureCoords.s, fs_TextureCoords.t + vStep);
        //vec2 ne = vec2(fs_TextureCoords.s + uStep, fs_TextureCoords.t + vStep);

        vec2 w = vec2(fs_TextureCoords.s - uStep, fs_TextureCoords.t);
        vec2 e = vec2(fs_TextureCoords.s + uStep, fs_TextureCoords.t);

        //vec2 sw = vec2(fs_TextureCoords.s - uStep, fs_TextureCoords.t - vStep);
        vec2 s = vec2(fs_TextureCoords.s, fs_TextureCoords.t - vStep);
        //vec2 se = vec2(fs_TextureCoords.s + uStep, fs_TextureCoords.t - vStep);

//        vec2 vectors[8];
//        vectors[0] = nw;
//        vectors[1] = n;
//        vectors[2] = ne;
//        vectors[3] = w;
//        vectors[4] = e;
//        vectors[5] = sw;
//        vectors[6] = s;
//        vectors[7] = se;

        vec2 vectors[4];
        vectors[0] = n;
        vectors[1] = w;
        vectors[2] = e;
        vectors[3] = s;

        vec4 colors[4];

        for(int i = 0; i < 4; i++)
        {
            colors[i] = texture(targetTexture, vectors[i]).rgba;
        }

        vec4 color = vec4(0,0,0,0);
        int count = 0;
        for(int i = 0; i < 4; i++)
        {
            if(colors[i].a > 0)
            {
                color += colors[i];
                count++;
            }

        }

        if(count > 0)
        {
            out_Color = color / count;
        }
        else{
            out_Color = vec4(1,1,1,1);
        }

    }
    else{
        out_Color = color;

    }
}
