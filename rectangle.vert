#version 430 core

layout (location = 2) uniform sampler2D color;      // �J���[�̃e�N�X�`��

// ���_���W
layout (location = 0) in vec4 pv;
layout (location = 1) in vec2 cc;                   // �J���[�̃e�N�X�`�����W

// �e�N�X�`�����W
out vec2 texcoord;
out vec2 colorcoord;

void main()
{
  // ���_���W���e�N�X�`�����W�Ɋ��Z
  texcoord = pv.xy * 0.5 + 0.5;

  // �e�N�X�`�����W
  colorcoord = cc / vec2(textureSize(color, 0));

  // ���_���W�����̂܂܏o��
  gl_Position = pv;
}

