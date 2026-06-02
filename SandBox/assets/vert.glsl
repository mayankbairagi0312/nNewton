#version 460 core


layout(location = 0) in vec4 aPos;

layout(location = 1) in vec4 aModelCol0;
layout(location = 2) in vec4 aModelCol1;
layout(location = 3) in vec4 aModelCol2;
layout(location = 4) in vec4 aModelCol3;
layout(location = 5) in vec4 aColor;

uniform mat4 uView;
uniform mat4 uProjection;

out vec4 vColor;

void main()
{
    mat4 model = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);
    vec4 worldPos = model * aPos;
    gl_Position = uProjection * uView * worldPos;
    vColor = aColor;
}