#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS (3.501e-3)
#define NUI_IMAGE_PLAYER_INDEX_SHIFT 3
#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER / float(1 << NUI_IMAGE_PLAYER_INDEX_SHIFT))
#define DEPTH_MAXIMUM (-4.0)

// �X�P�[��
const vec2 scale = vec2(
  NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS * 320.0,
  NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS * 240.0
);

// �e�N�X�`��
layout (location = 0) uniform sampler2D depth;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

// �f�v�X�l���X�P�[�����O����
float s(in float z)
{
  return z == 0.0 ? DEPTH_MAXIMUM : z * DEPTH_SCALE;
}

void main(void)
{
  // �f�v�X�l�����o��
  float z = s(texture(depth, texcoord).r);

  // �f�v�X�l����J�������W�l�����߂�
  position = vec3((texcoord - 0.5) * scale * z, z);
}
