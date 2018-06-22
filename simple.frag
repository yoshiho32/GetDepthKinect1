#version 430 core
#define SOBEL_FILTER 0

// �e�N�X�`��
layout (location = 2) uniform sampler2D color;      // �J���[�̃e�N�X�`��
layout (location = 3) uniform sampler2D depth; 

#ifdef SOBEL_FILTER
layout (location = 4) uniform sampler2D sobel;
#endif

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x
in vec2 texcoord;                                   // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
 // float z = texture(sobel, texcoord).r;
  // �e�N�X�`���}�b�s���O���s���ĉA�e�����߂�
  //fc = vec4(z,0,0,0);
  //fc = idiff + ispec;
  //fc = texture(color, texcoord);
  fc = texture(color, texcoord) * idiff + ispec;
 // fc = texture(color, texcoord) * idiff + ispec;//+ texture(sobel, texcoord);
 // fc = texture(depth, texcoord);//. + texture(sobel, texcoord);//* idiff + ispec;
}
