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

// 臒l
const float threshold = 0.1 * MILLIMETER;

//���U -- ���m�Ɏw�肷��ׂ�����
const float variance = 0.05 * MILLIMETER;

// 臒l����
vec2 f(const in float z)
{
  return vec2(z, 1.0) * step(threshold, z);
}

// �d�ݕt����f�l�̍��v�Əd�݂̍��v�����߂�
// ���݃o�O�������Bd�̒l���C�J�����l�ɂȂ邱�Ƃɂ�蒆���܂Ŕ��ł��Ă�
// �萔������Ƃ�����ۂ��Ȃ�
// �����������番�U�Ƃ̌��ˍ�����������Ȃ��Ȃ��Ǝv����
// �����������U�͂ǂ������l�ɂȂ�񂩂ȂƂ����̂��m�F�K�v

//base : ���ړ_ --- c : ��r��̓_    ---  w : �K�E�V�A���t�B���^���|�����Ƃ��̎Q�ƒl
vec2 Bf(const in vec2 base, const in vec4 c, const in float w)
{

  float d = c.r - base.r;//�o�O�̌����ۂ�

  //w(p,q) = exp((p-q)^2 / (2*variance)) * Gussian
  float e = exp(-0.5 * d * d / variance) * w;
  float z = c.r * e;

  //���Ƃ̉�f�ɔ��f���������āA���̒l��臒l�����ł����0�ɂ���B
  //2�ڂ̒l��0�ɂȂ�̂ł���ŗL���l���ǂ�������
  return f(z);
}

void main()
{

  //���ڂ���_�̒l�����̂܂�܂̒l��ݒ�
  vec2 zsum = f(texture(depth, texcoord).r);
  //���ړ_�����f�[�^���ēo�^
  vec2 base = zsum;

    //����5*5�}�X�ɑ΂��ďd�ݕt����f���̍��v�Əd�݂̍��v�����߂�
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
  zsum += Bf(base, textureOffset(depth, texcoord, ivec2( 2,  2)), 0.018315639);
  
  /*
  //��r�p3*3�̃t�B���^�[
  zsum += f(textureOffset(depth, texcoord, ivec2(-1, -1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 0, -1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 1, -1)).r);
  
  zsum += f(textureOffset(depth, texcoord, ivec2(-1,  0)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 1,  0)).r);
  
  zsum += f(textureOffset(depth, texcoord, ivec2(-1,  1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 0,  1)).r);
  zsum += f(textureOffset(depth, texcoord, ivec2( 1,  1)).r);
  */

  //zsum��2�ڂ̒l��0���L���l���Ȃ��������߉����ɔ�΂�
  //�łȂ����zsum�̒l�𑫂����킹�����Ŋ����āi���ρj�X�P�[���ɍ��킹��
  float z = zsum.g > 0.0 ? zsum.r * DEPTH_SCALE / zsum.g : DEPTH_MAXIMUM;

  // �f�v�X�l����J�������W�l�����߂�
  position = vec3((texcoord - 0.5) * scale * z, z);
}
