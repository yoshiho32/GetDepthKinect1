#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

// �e�N�X�`��
layout (location = 1) uniform sampler2D normal;     // �@���x�N�g���̃e�N�X�`��
layout (location = 3) uniform sampler2D back;		// �w�i�̃e�N�X�`��

// �E�B���h�E�T�C�Y
uniform vec2 size;

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec3 s;                                          // �����̋��܃x�N�g��
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec2 texcoord;                                   // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
  // �@���x�N�g��
  vec3 nv = texture(normal, texcoord).xyz;

  // ���܃}�b�s���O
  fc = texture(back, gl_FragCoord.xy / size + refract(vec3(0.0, 0.0, -1.0), nv, 0.67).xy * 0.2) * idiff + ispec;
}
