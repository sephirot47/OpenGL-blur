#version 130	

uniform float time;
uniform mat4 transform, projection;

in vec2 pix;

void main()  
{  
    gl_FragColor = vec4(sin(time), cos(time), clamp(sin(time) + cos(time), 0.3, 1.0), 1.0);  
}

