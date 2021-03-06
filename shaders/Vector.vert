#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colorIn;

layout(location = 0) out vec4 colorOut;

layout(push_constant) uniform PushConstant {
	mat4 ViewProj;
	mat4 Model;
} PushConst;

void main()
{
	colorOut = colorIn;
    float scale = 1000.0f;

    vec4 ScaledPosition = vec4(position, 1.0) * scale;
	gl_Position = PushConst.ViewProj * PushConst.Model * ScaledPosition;
}