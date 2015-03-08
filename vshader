#version 130

uniform float time;
uniform mat4 transform, projection;

in vec3 position;
in vec2 inuv;
out vec2 pix, uv;

void main()  
{  
    gl_Position = projection * transform * vec4(position.xyz, 1.0);
    pix = gl_Position.xy;
    uv = inuv;
}

