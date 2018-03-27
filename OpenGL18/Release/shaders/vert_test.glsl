#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 LightPosition_worldspace;

void main()
{
	gl_Position = projection*view*model*vec4(vertex,1.0f);
	
	Position_worldspace = (model * vec4(vertex,1)).xyz;
	vec3 vertexPosition_cameraspace = (view * model * vec4(vertex,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace = (view * vec4(LightPosition_worldspace,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	Normal_cameraspace = (view * model * vec4(vertexNormal,0)).xyz;


	UV = vertexUV;
}
