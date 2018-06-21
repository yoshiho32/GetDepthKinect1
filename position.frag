#version 430 core

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
layout (location = 2) uniform sampler2D color;      // カラーのテクスチャ

// テクスチャ座標
in vec2 texcoord;
in vec2 colorcoord;
in vec2 texcoord_value;

// フレームバッファに出力するデータ
layout (location = 0) out vec3 position;

// 閾値
const float threshold = 0.1 * MILLIMETER;

//分散 -- フィルタの強度の指定
const float variance = 0.05;

// 閾値処理
vec2 f2(const in float z)
{
	return vec2(z, 1.0) * step(threshold, z);
}

// 重み付き画素値の合計と重みの合計を求める
void f(inout vec3 csum, inout vec3 wsum, const in vec3 base, const in vec4 color, const in vec4 data, const in float weight)
{
	//まず各RGB値の差をとる
	vec3 d = color.xyz - base;
	//次にその差に応じて重み付けを決定し、ガウスによる重み付けを畳み込む
	//vec3 e = exp(-0.5 * d * d / variance) * weight;
	float esum = d.x + d.y + d.z;
	float e = exp(-0.5 * esum * esum / variance ) * weight;
	//色の合計を足していく
	csum += data.xyz * e * step(threshold, data.g) * step(threshold, base.r);
	//csum = data.xyz;
	wsum += e * step(threshold, data.g) * step(threshold, base.r);
}

// 重み付き平均を求める
void main()
{

	vec3 csum = texture(depth, texcoord).xyz;
	vec3 wsum = vec3(1.0);

	//ジョイントバイラテラルフィルタの色の重み用のベースカラー
	vec3 base = texture(color, texcoord).xyz;

	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2  , -2)), textureOffset(depth, texcoord, ivec2(-2, -2)), 0.000335463);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1  , -2)), textureOffset(depth, texcoord, ivec2(-1, -2)), 0.006737947);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0  , -2)), textureOffset(depth, texcoord, ivec2( 0, -2)), 0.018315639);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1  , -2)), textureOffset(depth, texcoord, ivec2( 1, -2)), 0.006737947);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2  , -2)), textureOffset(depth, texcoord, ivec2( 2, -2)), 0.000335463);

	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2  , -1)), textureOffset(depth, texcoord, ivec2(-2, -1)), 0.006737947);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1  , -1)), textureOffset(depth, texcoord, ivec2(-1, -1)), 0.135335283);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0  , -1)), textureOffset(depth, texcoord, ivec2( 0, -1)), 0.367879441);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1  , -1)), textureOffset(depth, texcoord, ivec2( 1, -1)), 0.135335283);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2  , -1)), textureOffset(depth, texcoord, ivec2( 2, -1)), 0.006737947);
  
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2  ,  0)), textureOffset(depth, texcoord, ivec2(-2,  0)), 0.018315639);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1  ,  0)), textureOffset(depth, texcoord, ivec2(-1,  0)), 0.367879441);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1  ,  0)), textureOffset(depth, texcoord, ivec2( 1,  0)), 0.367879441);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2  ,  0)), textureOffset(depth, texcoord, ivec2( 2,  0)), 0.018315639);

	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2  ,  1)), textureOffset(depth, texcoord, ivec2(-2,  1)), 0.006737947);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1  ,  1)), textureOffset(depth, texcoord, ivec2(-1,  1)), 0.135335283);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0  ,  1)), textureOffset(depth, texcoord, ivec2( 0,  1)), 0.367879441);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1  ,  1)), textureOffset(depth, texcoord, ivec2( 1,  1)), 0.135335283);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2  ,  1)), textureOffset(depth, texcoord, ivec2( 2,  1)), 0.006737947);

	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2  ,  2)), textureOffset(depth, texcoord, ivec2(-2,  2)), 0.000335463);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1  ,  2)), textureOffset(depth, texcoord, ivec2(-1,  2)), 0.006737947);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0  ,  2)), textureOffset(depth, texcoord, ivec2( 0,  2)), 0.018315639);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1  ,  2)), textureOffset(depth, texcoord, ivec2( 1,  2)), 0.006737947);
	f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2  ,  2)), textureOffset(depth, texcoord, ivec2( 2,  2)), 0.000335463);
	
	vec3 smoothed = csum / wsum;
	// float tex = texture(depth, texcoord).r > 0 ? texture(depth, texcoord).r * DEPTH_SCALE : DEPTH_MAXIMUM;
	float z = smoothed.r > 0 ? (smoothed.r + smoothed.g + smoothed.b) * DEPTH_SCALE : DEPTH_MAXIMUM;
  
    //float z = texture(depth, texcoord).r*DEPTH_SCALE;// >0 ? texture(depth, texcoord).r * DEPTH_SCALE : DEPTH_MAXIMUM;
	// デプス値からカメラ座標値を求める
	position = vec3(-(texcoord - 0.5 )* scale, 0);

}
