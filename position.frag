#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_NV_shader_buffer_load : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

#define KERNEL_SIZE 5

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

// 閾値処理
vec2 f(const in float z)
{
  return vec2(z, 1.0) * step(threshold, z);
}

// 重み付き画素値の合計と重みの合計を求める
void f2(inout float csum, inout float wsum, const in float base, const in float c, const in float w)
{
  float d = c - base;
  float e = exp(-0.5 * d * d / variance) * w;
  csum += c * e;
  wsum += e;
}


//カーネル内の分散を求める
vec3 Dispersion(){
  
  //カーネル内の平均値
  vec2 base =  f(texture(depth, texcoord + vec2(-2, -2)).r) + f(texture(depth, texcoord + vec2(-1, -2)).r) + f(texture(depth, texcoord + vec2( 0, -2)).r) + f(texture(depth, texcoord + vec2( 1, -2)).r) + f(texture(depth, texcoord + vec2( 2, -2)).r) + 
               f(texture(depth, texcoord + vec2(-2, -1)).r) + f(texture(depth, texcoord + vec2(-1, -1)).r) + f(texture(depth, texcoord + vec2( 0, -1)).r) + f(texture(depth, texcoord + vec2( 1, -1)).r) + f(texture(depth, texcoord + vec2( 2, -1)).r) + 
			   f(texture(depth, texcoord + vec2(-2,  0)).r) + f(texture(depth, texcoord + vec2(-1,  0)).r) + f(texture(depth, texcoord + vec2( 0,  0)).r) + f(texture(depth, texcoord + vec2( 1,  0)).r) + f(texture(depth, texcoord + vec2( 2,  0)).r) +
			   f(texture(depth, texcoord + vec2(-2,  1)).r) + f(texture(depth, texcoord + vec2(-1,  1)).r) + f(texture(depth, texcoord + vec2( 0,  1)).r) + f(texture(depth, texcoord + vec2( 1,  1)).r) + f(texture(depth, texcoord + vec2( 2,  1)).r) + 
			   f(texture(depth, texcoord + vec2(-2,  2)).r) + f(texture(depth, texcoord + vec2(-1,  2)).r) + f(texture(depth, texcoord + vec2( 0,  2)).r) + f(texture(depth, texcoord + vec2( 1,  2)).r) + f(texture(depth, texcoord + vec2( 2,  2)).r);
  float avg = base.r / base.g;

  //分散
  float result = texture(depth, texcoord + vec2( -2, -2)).r*texture(depth, texcoord + vec2( -2, -2)).r + texture(depth, texcoord + vec2( -1, -2)).r*texture(depth, texcoord + vec2( -1, -2)).r + texture(depth, texcoord + vec2( -1, -2)).r*texture(depth, texcoord + vec2( -1, -2)).r + texture(depth, texcoord + vec2(  0, -2)).r*texture(depth, texcoord + vec2(  0, -2)).r + texture(depth, texcoord + vec2( 1, -2)).r*texture(depth, texcoord + vec2( 1, -2)).r + texture(depth, texcoord + vec2( 2, -2)).r*texture(depth, texcoord + vec2( 2, -2)).r + 
				 texture(depth, texcoord + vec2( -2, -1)).r*texture(depth, texcoord + vec2( -2, -1)).r + texture(depth, texcoord + vec2( -1, -1)).r*texture(depth, texcoord + vec2( -1, -1)).r + texture(depth, texcoord + vec2(  0, -1)).r*texture(depth, texcoord + vec2(  0, -1)).r + texture(depth, texcoord + vec2( 1, -1)).r*texture(depth, texcoord + vec2( 1, -1)).r + texture(depth, texcoord + vec2( 2, -1)).r*texture(depth, texcoord + vec2( 2, -1)).r + 
			 	 texture(depth, texcoord + vec2( -2,  0)).r*texture(depth, texcoord + vec2( -2,  0)).r + texture(depth, texcoord + vec2( -1,  0)).r*texture(depth, texcoord + vec2( -1,  0)).r + texture(depth, texcoord + vec2(  0,  0)).r*texture(depth, texcoord + vec2(  0,  0)).r + texture(depth, texcoord + vec2( 1,  0)).r*texture(depth, texcoord + vec2( 1,  0)).r + texture(depth, texcoord + vec2( 2,  0)).r*texture(depth, texcoord + vec2( 2,  0)).r +
				 texture(depth, texcoord + vec2( -2,  1)).r*texture(depth, texcoord + vec2( -2,  1)).r + texture(depth, texcoord + vec2( -1,  1)).r*texture(depth, texcoord + vec2( -1,  1)).r + texture(depth, texcoord + vec2(  0,  1)).r*texture(depth, texcoord + vec2(  0,  1)).r + texture(depth, texcoord + vec2( 1,  1)).r*texture(depth, texcoord + vec2( 1,  1)).r + texture(depth, texcoord + vec2( 2,  1)).r*texture(depth, texcoord + vec2( 2,  1)).r + 
			 	 texture(depth, texcoord + vec2( -2,  2)).r*texture(depth, texcoord + vec2( -2,  2)).r + texture(depth, texcoord + vec2( -1,  2)).r*texture(depth, texcoord + vec2( -1,  2)).r + texture(depth, texcoord + vec2(  0,  2)).r*texture(depth, texcoord + vec2(  0,  2)).r + texture(depth, texcoord + vec2( 1,  2)).r*texture(depth, texcoord + vec2( 1,  2)).r + texture(depth, texcoord + vec2( 2,  2)).r*texture(depth, texcoord + vec2( 2,  2)).r;
  
  exp(KERNEL_SIZE*KERNEL_SIZE / result);

  return vec3(result, avg, base.g);

}


// 重み付き平均を求める
void main()
{
  vec3 stat = Dispersion();

  //float z = csum / wsum* DEPTH_SCALE;// > 0 ? csum / wsum * DEPTH_SCALE : DEPTH_MAXIMUM;
  //z = z < 10 ? DEPTH_MAXIMUM : z;
  // デプス値からカメラ座標値を求める
 // position = vec3(-(texcoord - 0.5) * scale * z, z);
  position = vec3( -texcoord ,stat.g);

}