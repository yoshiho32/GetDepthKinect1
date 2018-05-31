#version 430 core
#extension GL_NV_shader_buffer_load : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

// �X�P�[��
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// �e�N�X�`��
layout (location = 0) uniform sampler2D depth1;
layout (location = 1) uniform sampler2D depth;
layout (location = 2) uniform sampler2D color;      // �J���[�̃e�N�X�`��

// �e�N�X�`�����W
in vec2 texcoord;
in vec2 colorcoord;
in vec2 texcoord_value;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

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

// �d�ݕt�����ς����߂�
void main()
{

  vec3 csum = texture(depth, texcoord).xyz;
  vec3 wsum = vec3(1.0);
  //�W���C���g�o�C���e�����t�B���^�̐F�̏d�ݗp�̃x�[�X�J���[
  vec3 base = texture(color, texcoord).xyz;
  
  int Miss_num = 0;
  vec2 texcoord_value2 = vec2(1.0, 1.0);

  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2 * texcoord_value.r , -2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-2, -2)), 0.018315639);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1 * texcoord_value.r , -2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-1, -2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0 * texcoord_value.r , -2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 0, -2)), 0.135335283);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1 * texcoord_value.r , -2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 1, -2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2 * texcoord_value.r , -2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 2, -2)), 0.018315639);

  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2 * texcoord_value.r , -1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-2, -1)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1 * texcoord_value.r , -1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-1, -1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0 * texcoord_value.r , -1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 0, -1)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1 * texcoord_value.r , -1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 1, -1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2 * texcoord_value.r , -1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 2, -1)), 0.082084999);
  
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2 * texcoord_value.r ,  0 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-2,  0)), 0.135335283);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1 * texcoord_value.r ,  0 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-1,  0)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1 * texcoord_value.r ,  0 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 1,  0)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2 * texcoord_value.r ,  0 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 2,  0)), 0.135335283);

  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2 * texcoord_value.r ,  1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-2,  1)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1 * texcoord_value.r ,  1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-1,  1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0 * texcoord_value.r ,  1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 0,  1)), 0.60653066);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1 * texcoord_value.r ,  1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 1,  1)), 0.367879441);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2 * texcoord_value.r ,  1 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 2,  1)), 0.082084999);

  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-2 * texcoord_value.r ,  2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-2,  2)), 0.018315639);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2(-1 * texcoord_value.r ,  2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2(-1,  2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 0 * texcoord_value.r ,  2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 0,  2)), 0.135335283);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 1 * texcoord_value.r ,  2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 1,  2)), 0.082084999);
  f(csum, wsum, base, textureOffset(color, texcoord, ivec2( 2 * texcoord_value.r ,  2 * texcoord_value.g)), textureOffset(depth, texcoord, ivec2( 2,  2)), 0.018315639);

  vec3 smoothed = csum / wsum;
 // float tex = texture(depth, texcoord).r > 0 ? texture(depth, texcoord).r * DEPTH_SCALE : DEPTH_MAXIMUM;
  float z = smoothed.r > 0 ? (smoothed.r + smoothed.g + smoothed.b) * DEPTH_SCALE : DEPTH_MAXIMUM;

  // �f�v�X�l����J�������W�l�����߂�
  position = vec3((texcoord - 0.5 )* scale * z, z);

}
