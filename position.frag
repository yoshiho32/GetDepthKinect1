#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_NV_shader_buffer_load : enable

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

//差分を指定することでvec3( 画素値、　値の有効　、　中央までのL2ノルム距離  )を返す
vec3 makeTile(const int x, const int y){
  float z = texture(depth, texcoord + vec2(x, y)).r;
  return vec3(vec2(z, 1.0) * step(threshold, z), x*x+y*y);
}

//カーネル内の深度情報が入る
vec3 TileData[25];

//Tiledataに全部入れる
void tileset(){
  TileData[ 0] = makeTile(-2,-2);
  TileData[ 1] = makeTile(-1,-2);
  TileData[ 2] = makeTile( 0,-2);
  TileData[ 3] = makeTile( 1,-2);
  TileData[ 4] = makeTile( 2,-2);
  TileData[ 5] = makeTile(-2,-1);
  TileData[ 6] = makeTile(-1,-1);
  TileData[ 7] = makeTile( 0,-1);
  TileData[ 8] = makeTile( 1,-1);
  TileData[ 9] = makeTile( 2,-1);
  TileData[10] = makeTile(-2, 0);
  TileData[11] = makeTile(-1, 0);
  TileData[12] = makeTile( 0, 0);
  TileData[13] = makeTile( 1, 0);
  TileData[14] = makeTile( 2, 0);
  TileData[15] = makeTile(-2, 1);
  TileData[16] = makeTile(-1, 1);
  TileData[17] = makeTile( 0, 1);
  TileData[18] = makeTile( 1, 1);
  TileData[19] = makeTile( 2, 1);
  TileData[20] = makeTile(-2, 2);
  TileData[21] = makeTile(-1, 2);
  TileData[22] = makeTile( 0, 2);
  TileData[23] = makeTile( 1, 2);
  TileData[24] = makeTile( 2, 2);
}

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
vec2 Dispersion(const vec2 *data){
  
  //カーネル内の平均値
  vec2 base = (data[ 0] + data[ 1] + data[ 2] + data[ 3] + data[ 4] + data[ 5] + data[ 6] + data[ 7] + data[ 8] + data[ 9] + 
			   data[10] + data[11] + data[12] + data[13] + data[14] + data[15] + data[16] + data[17] + data[18] + data[19] + 
			   data[20] + data[21] + data[22] + data[23] + data[24]) / 25;

  //分散
  vec2 result = base*base*25 -
  ( data[ 0]*data[ 0] + data[ 1]*data[ 1] + data[ 2]*data[ 2] + data[ 3]*data[ 3] + data[ 4]*data[ 4] + 
    data[ 5]*data[ 5] + data[ 6]*data[ 6] + data[ 7]*data[ 7] + data[ 8]*data[ 8] + data[ 9]*data[ 9] + 
    data[10]*data[10] + data[11]*data[11] + data[12]*data[12] + data[13]*data[13] + data[14]*data[14] +
    data[15]*data[15] + data[16]*data[16] + data[17]*data[17] + data[18]*data[18] + data[19]*data[19] + 
    data[20]*data[20] + data[21]*data[21] + data[22]*data[22] + data[23]*data[23] + data[24]*data[24] );

  return vec2(result.r, base);

}


// 重み付き平均を求める
void main()
{

  //tileset();
  //vec2 Kinfo = vec2(Dispersion(TileData));
  vec2 test[] = test[](
	vec2(10, 1),
	vec2(20, 0)
  );
  float csum = texture(depth, texcoord).r;
  float wsum = 1.0;
  float base = csum;
  

 

  float z = csum / wsum* DEPTH_SCALE;// > 0 ? csum / wsum * DEPTH_SCALE : DEPTH_MAXIMUM;
  //z = z < 10 ? DEPTH_MAXIMUM : z;
  // デプス値からカメラ座標値を求める
 // position = vec3(-(texcoord - 0.5) * scale * z, z);
  position = vec3( -texcoord ,z);

}