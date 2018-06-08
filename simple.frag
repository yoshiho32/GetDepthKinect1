#version 430 core

// �e�N�X�`��
layout (location = 2) uniform sampler2D color;      // �J���[�̃e�N�X�`��
layout (location = 3) uniform sampler2D depth;

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec2 texcoord;                                   // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{

  // �e�N�X�`���}�b�s���O���s���ĉA�e�����߂�
  //fc = idiff + ispec;
  //fc = texture(color, texcoord);
  fc = texture(color, texcoord) * idiff + ispec;
  //fc = texture(depth, texcoord)* idiff + ispec;
}
