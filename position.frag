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
layout (location = 0) uniform sampler2D depth;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

//臒l
const float threshold = 0.1 * MILLIMETER;

//���U
const float variance = 0.5;

// �d�ݕt����f���̍��v�Əd�݂̍��v�����߂�
vec2 f(const in vec2 base, const in float c, const in float w)
{
  float d = c - base.r;
  float e = exp(-0.5 * d * d / variance) * w;
  return vec2(c * e, 1.0) * step(threshold, c * e);
}

// �d�ݕt�����ς�����m��
void main()
{
  vec2 zsum = vec2(texture(depth, texcoord).r ,1.0) * step(threshold, texture(depth, texcoord).r);
  vec2 base = zsum;
  

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

  float z = zsum.g > 0.0 ? zsum.r * DEPTH_SCALE / zsum.g : DEPTH_MAXIMUM;

  // �f�v�X�l����J�������W�l�����߂�
  position = vec3((texcoord - 0.5) * scale * z, z);
}
