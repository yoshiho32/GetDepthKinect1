#version 430 core

// テクスチャ
layout (location = 0) uniform sampler2D position;

// テクスチャ座標
in vec2 texcoord;

// フレームバッファに出力するデータ
layout (location = 0) out vec3 normal;

void main(void)
{
  // 近傍の勾配を求める
  vec3 vx = vec3(textureOffset(position, texcoord, ivec2(1, 0)) - textureOffset(position, texcoord, ivec2(-1, 0)));
  vec3 vy = vec3(textureOffset(position, texcoord, ivec2(0, 1)) - textureOffset(position, texcoord, ivec2(0, -1)));

  // 勾配からから法線ベクトルを求める
  normal = normalize(cross(vx, vy));
}
