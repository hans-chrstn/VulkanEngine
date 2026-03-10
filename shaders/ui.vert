#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform Push {
  vec2 windowSize;
} push;

void main() {
  vec2 normalized = inPos / push.windowSize;
  gl_Position = vec4(normalized * 2.0 - 1.0, 0.0, 1.0);

  fragColor = inColor;
  fragTexCoord = inTexCoord;
}
