#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

//
// 5x5 �o�C���e�����t�B���^
//

// �e�N�X�`��
layout (location = 0) uniform sampler2D position;

// �e�N�X�`�����W
in vec2 texcoord;

// �o��
layout (location = 0) out vec3 smoothed;

// ���U
const float variance = 10.0;

// �d�ݕt����f�l�̍��v�Əd�݂̍��v�����߂�
void f(inout vec3 csum, inout vec3 wsum, const in vec3 base, const in vec4 c, const in float w)
{
  vec3 d = c.xyz - base;
  vec3 e = exp(-0.5 * d * d / variance) * w;
  csum += c.xyz * e;
  wsum += e;
}

// �d�ݕt�����ς����߂�
void main()
{
  vec3 csum = texture(position, texcoord).xyz;
  vec3 wsum = vec3(1.0);
  vec3 base = csum;

  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-2, -2)), 0.018315639);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-1, -2)), 0.082084999);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 0, -2)), 0.135335283);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 1, -2)), 0.082084999);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 2, -2)), 0.018315639);

  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-2, -1)), 0.082084999);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-1, -1)), 0.367879441);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 0, -1)), 0.60653066);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 1, -1)), 0.367879441);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 2, -1)), 0.082084999);

  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-2,  0)), 0.135335283);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-1,  0)), 0.60653066);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 1,  0)), 0.60653066);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 2,  0)), 0.135335283);

  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-2,  1)), 0.082084999);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-1,  1)), 0.367879441);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 0,  1)), 0.60653066);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 1,  1)), 0.367879441);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 2,  1)), 0.082084999);

  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-2,  2)), 0.018315639);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2(-1,  2)), 0.082084999);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 0,  2)), 0.135335283);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 1,  2)), 0.082084999);
  f(csum, wsum, base, textureOffset(position, texcoord, ivec2( 2,  2)), 0.018315639);

  smoothed = csum / wsum;
}
