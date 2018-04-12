#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

// スケール
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// テクスチャ
layout (location = 0) uniform sampler2D depth;

// テクスチャ座標
in vec2 texcoord;

// フレームバッファに出力するデータ
layout (location = 0) out vec3 position;

// 閾値
const float threshold = 0.1 * MILLIMETER;

//分散 -- フィルタの強度の指定らしい
const float variance = 0.05;


// 重み付き画素値の合計と重みの合計を求める
void f(inout float csum, inout float wsum, const in float base, const in float c, const in float w)
{
  float d = c- base;
  float e = exp(-0.5 * d * d / variance) * w;
  csum += c * e;
  wsum += e;
}


// 重み付き平均を求める
void main()
{
  float csum = texture(depth, texcoord).r;
  float wsum = 1.0;
  float base = csum;
  
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-2, -2)).r, 0.018315639);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-1, -2)).r, 0.082084999);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 0, -2)).r, 0.135335283);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 1, -2)).r, 0.082084999);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 2, -2)).r, 0.018315639);
  
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-2, -1)).r, 0.082084999);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-1, -1)).r, 0.367879441);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 0, -1)).r, 0.60653066);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 1, -1)).r, 0.367879441);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 2, -1)).r, 0.082084999);
  
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-2,  0)).r, 0.135335283);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-1,  0)).r, 0.60653066);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 1,  0)).r, 0.60653066);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 2,  0)).r, 0.135335283);
  
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-2,  1)).r, 0.082084999);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-1,  1)).r, 0.367879441);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 0,  1)).r, 0.60653066);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 1,  1)).r, 0.367879441);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 2,  1)).r, 0.082084999);
  
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-2,  2)).r, 0.018315639);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2(-1,  2)).r, 0.082084999);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 0,  2)).r, 0.135335283);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 1,  2)).r, 0.082084999);
  f(csum, wsum, base, textureOffset(depth, texcoord, ivec2( 2,  2)).r, 0.018315639);

  float z = csum / wsum* DEPTH_SCALE;// > 0 ? csum / wsum * DEPTH_SCALE : DEPTH_MAXIMUM;
  //z = z < 10 ? DEPTH_MAXIMUM : z;
  // デプス値からカメラ座標値を求める
 // position = vec3(-(texcoord - 0.5) * scale * z, z);
  position = vec3( -texcoord ,z);

}