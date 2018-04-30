#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

// �X�P�[��
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// �e�N�X�`��
layout (location = 2) uniform sampler2D color;      // �J���[�̃e�N�X�`��
layout (location = 3) uniform sampler2D depth;

// �e�N�X�`�����W
//in vec2 texcoord;
in vec2 colorcoord;

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec2 texcoord;                                   // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

// 臒l
const float threshold = 0.1 * MILLIMETER;

//���U -- �t�B���^�̋��x�̎w��炵��
const float variance = 0.05;

// 臒l����
vec2 f2(const in float z)
{
  return vec2(z, 1.0) * step(threshold, z);
}

// �d�ݕt����f�l�̍��v�Əd�݂̍��v�����߂�
void f(inout vec3 csum, inout vec3 wsum, const in vec3 base, const in vec4 color, const in vec4 c, const in float w)
{
  vec3 d = color.xyz - base;
  vec3 e = exp(-0.5 * d * d / variance) * w;
  csum += c.xyz * e * step(threshold, c.x) * step(threshold, base.r);
  wsum += e * step(threshold, c.x) * step(threshold, base.r);
}


void main(void)
{


  vec3 csum = texture(depth, texcoord).xyz;
  vec3 wsum = vec3(1.0);
  vec3 base = texture(color, colorcoord).xyz;
  
  int Miss_num = 0;

  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-2, -2)), textureOffset(depth, texcoord, ivec2(-2, -2)), 0.018315639);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-1, -2)), textureOffset(depth, texcoord, ivec2(-1, -2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 0, -2)), textureOffset(depth, texcoord, ivec2( 0, -2)), 0.135335283);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 1, -2)), textureOffset(depth, texcoord, ivec2( 1, -2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 2, -2)), textureOffset(depth, texcoord, ivec2( 2, -2)), 0.018315639);

  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-2, -1)), textureOffset(depth, texcoord, ivec2(-2, -1)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-1, -1)), textureOffset(depth, texcoord, ivec2(-1, -1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 0, -1)), textureOffset(depth, texcoord, ivec2( 0, -1)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 1, -1)), textureOffset(depth, texcoord, ivec2( 1, -1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 2, -1)), textureOffset(depth, texcoord, ivec2( 2, -1)), 0.082084999);
  
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-2,  0)), textureOffset(depth, texcoord, ivec2(-2,  0)), 0.135335283);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-1,  0)), textureOffset(depth, texcoord, ivec2(-1,  0)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 1,  0)), textureOffset(depth, texcoord, ivec2( 1,  0)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 2,  0)), textureOffset(depth, texcoord, ivec2( 2,  0)), 0.135335283);

  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-2,  1)), textureOffset(depth, texcoord, ivec2(-2,  1)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-1,  1)), textureOffset(depth, texcoord, ivec2(-1,  1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 0,  1)), textureOffset(depth, texcoord, ivec2( 0,  1)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 1,  1)), textureOffset(depth, texcoord, ivec2( 1,  1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 2,  1)), textureOffset(depth, texcoord, ivec2( 2,  1)), 0.082084999);

  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-2,  2)), textureOffset(depth, texcoord, ivec2(-2,  2)), 0.018315639);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2(-1,  2)), textureOffset(depth, texcoord, ivec2(-1,  2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 0,  2)), textureOffset(depth, texcoord, ivec2( 0,  2)), 0.135335283);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 1,  2)), textureOffset(depth, texcoord, ivec2( 1,  2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, colorcoord, ivec2( 2,  2)), textureOffset(depth, texcoord, ivec2( 2,  2)), 0.018315639);

  vec3 smoothed = csum / wsum;
  
  //�@���x�N�g�������o�Ă邯�ǂǂ�����Č�����悤�ɂ��邩�l����
  //float gray = dot(texture(depth, texcoord).rgb*50, vec3(0.299, 0.587, 0.114));
  //float gray = dot(smoothed, vec3(0.299, 0.587, 0.114));
  //float gray = (smoothed.r + smoothed.g + smoothed.b) * 50/3;
  // �e�N�X�`���}�b�s���O���s���ĉA�e�����߂�
  //fc = idiff + ispec;
  //fc = texture(color, texcoord);
  //fc = vec4(csum,1.0);
  //fc = vec4(gray, gray, gray, texture(color, texcoord).a);// * idiff + ispec;
  fc = texture(color, texcoord) * idiff + ispec;

}
