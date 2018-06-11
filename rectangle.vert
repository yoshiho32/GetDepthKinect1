#version 430 core

layout (location = 2) uniform sampler2D color;      // カラーのテクスチャ

// 頂点座標
layout (location = 0) in vec4 pv;
layout (location = 1) in vec2 cc;                   // カラーのテクスチャ座標

// テクスチャ座標
out vec2 texcoord;
out vec2 colorcoord;

void main()
{
  // 頂点座標をテクスチャ座標に換算
  texcoord = pv.xy * 0.5 + 0.5;

  // テクスチャ座標
  colorcoord = cc / vec2(textureSize(color, 0));

  // 頂点座標をそのまま出力
  gl_Position = pv;
}

