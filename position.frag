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

//分散 -- 正確に指定するべきかも
const float variance = 0.017;

// 閾値処理
vec2 f(const in float z)
{
  return vec2(z, 1.0) * step(threshold, z);
}

//gausiann filter
vec2 Gf(const in vec4 c, const in float w){
  //ずれた値にガウスの値を追加して
  return f(c.r)*w;
}

float GaussianF(){
  
  vec2 zsum = Gf(texture(depth, texcoord), 36.0);

  //周囲5*5マスに対してガウシアンする
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-2, -2)), 1.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-1, -2)), 4.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 0, -2)), 6.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 1, -2)), 4.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 2, -2)), 1.0);
  
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-2, -1)), 4.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-1, -1)), 16.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 0, -1)), 24.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 1, -1)), 16.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 2, -1)), 4.0);
  
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-2,  0)), 6.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-1,  0)), 24.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 1,  0)), 24.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 2,  0)), 6.0);
  
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-2,  1)), 4.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-1,  1)), 16.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 0,  1)), 24.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 1,  1)), 16.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 2,  1)), 4.0);
  
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-2,  2)), 1.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2(-1,  2)), 4.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 0,  2)), 6.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 1,  2)), 4.0);
  zsum += Gf(textureOffset(depth, texcoord, ivec2( 2,  2)), 1.0);

  float z = zsum.g > 0.0 ? zsum.r * DEPTH_SCALE / zsum.g : DEPTH_MAXIMUM;

  return z;

}

/*
//bilateral filter
//base : 注目点 --- c : 比較先の点    ---  w : ガウシアンフィルタを掛けたときの参照値
vec2 Bf(const in vec2 const in vec4 c, const in float w)
{

  //差が大きすぎるものは無効にする
  float d = abs(c.r - base.r);//バグの元っぽい

  //w(p,q) = exp((p-q)^2 / (2*variance)) * Gussian
  //差が大きすぎた場合無効にする
  float e = d == 0 ? 0 : exp(-0.5 * d * d / variance) * w;
  float z = c.r * e;

  //もとの画素に反映率をかけて、その値が閾値未満であれば0にする。
  //2つ目の値が0になるのでそれで有効値かどうか判定
  return f(z);
}
*/

void main()
{

  //着目する点の値をそのまんまの値を設定
  //vec2 zsum = f(texture(depth, texcoord).r);
  //着目点を元データして登録
  //vec2 base = zsum;



  /*
    //周囲5*5マスに対して重み付き画素数の合計と重みの合計を求める
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-2, -2)), 0.018315639);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-1, -2)), 0.082084999);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 0, -2)), 0.135335283);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 1, -2)), 0.082084999);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 2, -2)), 0.018315639);
  
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-2, -1)), 0.082084999);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-1, -1)), 0.367879441);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 0, -1)), 0.60653066);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 1, -1)), 0.367879441);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 2, -1)), 0.082084999);
  
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-2,  0)), 0.135335283);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-1,  0)), 0.60653066);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 1,  0)), 0.60653066);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 2,  0)), 0.135335283);
  
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-2,  1)), 0.082084999);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-1,  1)), 0.367879441);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 0,  1)), 0.60653066);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 1,  1)), 0.367879441);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 2,  1)), 0.082084999);
  
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-2,  2)), 0.018315639);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2(-1,  2)), 0.082084999);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 0,  2)), 0.135335283);
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 1,  2)), 0.082084999);
  zsum += Bf(textureOffset(depth, texcoord, ivec2( 2,  2)), 0.018315639);
  */

  /*
  //比較用3*3のフィルター
  zsum += f(textureOffset(depth, texcoord, ivec2(-1, -1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 0, -1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 1, -1)).r);
  
  zsum += f(textureOffset(depth, texcoord, ivec2(-1,  0)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 1,  0)).r);
  
  zsum += f(textureOffset(depth, texcoord, ivec2(-1,  1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 0,  1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 1,  1)).r);
  */

  //zsumの2つ目の値が0→有効値がなかったため遠くに飛ばす
  //でなければzsumの値を足し合わせた数で割って（平均）スケールに合わせる
  float z = GaussianF();//zsum.g > 0.0 ? zsum.r * DEPTH_SCALE : DEPTH_MAXIMUM;

  // デプス値からカメラ座標値を求める
  position = vec3((texcoord - 0.5) * scale * z, z);
}
