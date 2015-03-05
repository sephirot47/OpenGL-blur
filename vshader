#version 130

uniform float time;
uniform mat4 transform, projection;

in vec3 position;
out vec2 pix;

void main()  
{  
    gl_Position = projection * transform * vec4(position.xyz, 1.0);
    pix = gl_Position.xy;
}

