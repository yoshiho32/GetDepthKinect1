#version 430 core
#define SOBEL_FILTER 0

// テクスチャ
layout (location = 2) uniform sampler2D color;      // カラーのテクスチャ
layout (location = 3) uniform sampler2D depth; 

#ifdef SOBEL_FILTER
layout (location = 4) uniform sampler2D sobel;
#endif

// ラスタライザから受け取る頂点属性の補間値
in vec4 idiff;                                      // 拡散反射光強度
in vec4 ispec;                                      // 鏡面反射光強度
in vec2 texcoord;                                   // テクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main(void)
{
 // float z = texture(sobel, texcoord).r;
  // テクスチャマッピングを行って陰影を求める
  //fc = vec4(z,0,0,0);
  //fc = idiff + ispec;
  //fc = texture(color, texcoord);
  fc = texture(color, texcoord) * idiff + ispec;
 // fc = texture(color, texcoord) * idiff + ispec;//+ texture(sobel, texcoord);
 // fc = texture(depth, texcoord);//. + texture(sobel, texcoord);//* idiff + ispec;
}
