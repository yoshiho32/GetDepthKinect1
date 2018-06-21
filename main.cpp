//
// Kinect (v2) �̃f�v�X�}�b�v�擾
//

// �W�����C�u����
#include <Windows.h>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �Z���T�֘A�̏���
#include "KinectV2.h"

// �`��ɗp���郁�b�V��
#include "Mesh.h"

// �v�Z�ɗp����V�F�[�_
#include "Calculate.h"

//�J���}���t�B���^�[�̌v�Z�ɗp����R���s���[�g�V�F�[�_
#include "ComputeShader.h"

// ���_�ʒu�̐������V�F�[�_ (position.frag) �ōs���Ȃ� 1
#define GENERATE_POSITION 1
#define SOBEL_FILTER 0

//
// ���C���v���O����
//
int main()
{
  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    // GLFW �̏������Ɏ��s����
    MessageBox(NULL, TEXT("GLFW �̏������Ɏ��s���܂����B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // �v���O�����I�����ɂ� GLFW ���I������
  atexit(glfwTerminate);

  // OpenGL Version 3.2 Core Profile ��I������
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // �E�B���h�E���J��
  Window window(640, 480, "Depth Map Viewer");
  if (!window.get())
  {
    // �E�B���h�E���쐬�ł��Ȃ�����
    MessageBox(NULL, TEXT("GLFW �̃E�B���h�E���J���܂���ł����B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // �[�x�Z���T��L���ɂ���
  KinectV2 sensor;
  if (sensor.getActivated() == 0)
  {
    // �Z���T���g���Ȃ�����
    MessageBox(NULL, TEXT("�[�x�Z���T��L���ɂł��܂���ł����B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // �[�x�Z���T�̉𑜓x
  int width, height;
  sensor.getDepthResolution(&width, &height);

  // �`��Ɏg�����b�V��
  const Mesh mesh(width, height, sensor.getCoordBuffer());

  // �`��p�̃V�F�[�_
  GgSimpleShader simple("simple.vert", "simple.frag");

  // �f�v�X�f�[�^���璸�_�ʒu���v�Z����V�F�[�_
  const Calculate position(width, height, "position.frag");

  // ���_�ʒu����@���x�N�g�����v�Z����V�F�[�_
  const Calculate normal(width, height, "normal.frag");

#ifdef SOBEL_FILTER
  //�����t�B���^�[�p�̃R���s���[�g�V�F�[�_
  ComputeShader sobel(width, height, "sobel.comp");
#endif

  //�J���}���t�B���^�[�p�̃R���s���[�g�V�F�[�_
  ComputeShader kalman(width, height, "kalman.comp");

  //�J���}���t�B���^�p�ϐ�
  const float Q(0.0001f); 
  const float R(0.01f);
  float P(0.0f);
  float K(0.0f);
  GLint KLoc = glGetUniformLocation(kalman.setprogram(), "K");
  int SwitchKalman(0);

  // �w�i�F��ݒ肷��
  glClearColor(background[0], background[1], background[2], background[3]);

  // �B�ʏ���������L���ɂ���
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // �E�B���h�E���J���Ă���Ԃ���Ԃ��`�悷��
  while (!window.shouldClose())
  {
#if GENERATE_POSITION

	//�J���}���t�B���^�[�̌v�Z�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	// �J���}���t�B���^�[�p�ϐ��̌v�Z
	K = (P + Q) / (P + Q + R);
	P = R * (P + Q) / (R + P + Q);

	kalman.use();
	//uniform�ϐ�K�Ƃ���GPU�Ƀf�[�^��n��
	glUniform1f(KLoc, K);

	//depth�f�[�^�̓]��
	glUniform1i(0, 0);
	glActiveTexture(GL_TEXTURE0);
	sensor.getDepth();

	//�e�N�X�`�������ւ��Čv�Z����
	glActiveTexture(GL_TEXTURE1);
	if (SwitchKalman) {
		//�e�N�X�`���̐ݒ�
		glBindImageTexture(1, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//�v�Z���ʂ�����ꏊ��ݒ�
		glActiveTexture(GL_TEXTURE2);
		glBindImageTexture(2, kalman.tex_A, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//std::cout << "A" << std::endl;
	}
	else {
		//�e�N�X�`���̐ݒ�
		glBindImageTexture(1, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//�v�Z���ʂ�����ꏊ��ݒ�
		glActiveTexture(GL_TEXTURE2);
		glBindImageTexture(2, kalman.tex_B, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//std::cout << "B" << std::endl;
	}
	//�����̎��s
	kalman.calculate();
	//�J���}���t�B���^�[�I���[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

#ifdef SOBEL_FILTER
	//�����t�B���^�[
	sobel.use();
	glActiveTexture(GL_TEXTURE0);
	if (SwitchKalman) {
		glBindImageTexture(0, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}
	else {
		glBindImageTexture(0, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindImageTexture(1, sobel.tex_A, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	sobel.calculate();
#endif

    // ���_�ʒu�̌v�Z
    position.use();

	//�����ς݂̃f�v�X�f�[�^��n��

    glActiveTexture(GL_TEXTURE0);
	//���݂ɓ����Ă�ꏊ���Q�Ƃ��āA�v�Z�����\���ʒu��n��
	if (SwitchKalman) {
		glBindImageTexture(0, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}
	else {
		glBindImageTexture(0, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	glUniform1i(2, 2);
	glActiveTexture(GL_TEXTURE2);
	sensor.getColor();

	const std::vector<GLuint> &positionTexture(position.calculate());

    // �@���x�N�g���̌v�Z
    normal.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positionTexture[0]);
    const std::vector<GLuint> &normalTexture(normal.calculate());

#else
    // �@���x�N�g���̌v�Z
    normal.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    sensor.getPoint();
    const std::vector<GLuint> &normalTexture(normal.calculate());
#endif

    // ��ʏ���
    window.clear();

    // �`��p�̃V�F�[�_�v���O�����̎g�p�J�n
    simple.use();
    simple.loadMatrix(window.getMp(), window.getMw());
    simple.setLight(light);
    simple.setMaterial(material);
	 
    // �e�N�X�`��
#if GENERATE_POSITION
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture[0]);

#endif
	//�@���x�N�g���̎󂯓n��
    glUniform1i(1, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture[0]);
	//�F���̎󂯓n��
	glUniform1i(2, 2);
    glActiveTexture(GL_TEXTURE2);
    sensor.getColor();

	//�f�v�X���̎󂯓n��
	glActiveTexture(GL_TEXTURE3);
	//���݂ɓ����Ă�ꏊ���Q�Ƃ��āA�v�Z�����\���ʒu��n��
	if (SwitchKalman) {
		glBindImageTexture(3, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		SwitchKalman--;
	}
	else {
		glBindImageTexture(3, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		SwitchKalman++;
	}

#ifdef SOBEL_FILTER
	glActiveTexture(GL_TEXTURE4);
	glBindImageTexture(4, sobel.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
#endif

    // �}�`�`��
    mesh.draw();

    // �o�b�t�@�����ւ���
    window.swapBuffers();
  }
}
