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
//入力は0 ~ 8000...?

// テクスチャ座標
in vec2 texcoord;

// フレームバッファに出力するデータ
layout (location = 0) out vec3 position;

//閾値
const float threshold = 0.1 * MILLIMETER;

//分散
const float variance =  0.5;

// 重み付き画素数の合計と重みの合計を求める
vec2 f(const in vec2 base, const in float c, const in float w)
{
  //dにずらした点の座標値から元の点のデプス値を引く(中心との色の差)
  float d = c - base.r;

  //ガウス分布による反映率を設定
  float e = exp(-0.5 * d * d / variance) * w;

  //もとの画素に反映率をかけて、その値が閾値未満であれば0にする。
  //2つ目の値が0になるのでそれで有効値かどうか判定
  return vec2(c * e, 1.0) * step(threshold, c * e);
}

// 重み付き平均をもとmる
void main()
{
  //着目する点の値をそのまんまの値を設定
  vec2 zsum = vec2(texture(depth, texcoord).r ,1.0);
  //着目点を元データして登録
  vec2 base = zsum;

  //周囲5*5マスに対して重み付き画素数の合計と重みの合計を求める
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-2, -2)).r, 0.018315639);
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-1, -2)).r, 0.082084999);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 0, -2)).r, 0.135335283);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 1, -2)).r, 0.082084999);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 2, -2)).r, 0.018315639);
  
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-2, -1)).r, 0.082084999);
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-1, -1)).r, 0.367879441);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 0, -1)).r, 0.60653066);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 1, -1)).r, 0.367879441);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 2, -1)).r, 0.082084999);
  
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-2,  0)).r, 0.135335283);
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-1,  0)).r, 0.60653066);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 1,  0)).r, 0.60653066);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 2,  0)).r, 0.135335283);
  
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-2,  1)).r, 0.082084999);
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-1,  1)).r, 0.367879441);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 0,  1)).r, 0.60653066);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 1,  1)).r, 0.367879441);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 2,  1)).r, 0.082084999);
  
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-2,  2)).r, 0.018315639);
  zsum += f(base, textureOffset(depth, texcoord, ivec2(-1,  2)).r, 0.082084999);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 0,  2)).r, 0.135335283);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 1,  2)).r, 0.082084999);
  zsum += f(base, textureOffset(depth, texcoord, ivec2( 2,  2)).r, 0.018315639);

  //zsumの2つ目の値が0→有効値がなかったため遠くに飛ばす
  //でなければzsumの値を足し合わせた数で割って（平均）スケールに合わせる
  float z = zsum.g > 0.0 ? zsum.r * DEPTH_SCALE / zsum.g : DEPTH_MAXIMUM;

  // デプス値からカメラ座標値を求める
  // z → (-1.0) に変更。ここが原因だった臭い
  position = vec3((texcoord - 0.5) * scale * (-1.0) , z);
}
