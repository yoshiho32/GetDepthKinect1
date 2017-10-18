//
// Kinect (v2) �̃f�v�X�}�b�v�擾
//

// �W�����C�u����
#include <Windows.h>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �Z���T�֘A�̏���
#include "KinectV2.h"

// �`��ɗp���郁�b�V��
#include "Mesh.h"

// �v�Z�ɗp����V�F�[�_
#include "Calculate.h"

// ���_�ʒu�̐������V�F�[�_ (position.frag) �ōs���Ȃ� 1
#define GENERATE_POSITION 0

//
// ���C���v���O����
//
int main()
{
  // OpenCV �ɂ��r�f�I�L���v�`��������������
  cv::VideoCapture camera(0);
  if (!camera.isOpened())
  {
    // �r�f�I�J�������擾�ł��Ȃ�
    MessageBox(NULL, TEXT("�r�f�I�J������������܂���B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // �J�����̏����ݒ�
  camera.grab();
  const GLsizei capture_env_width(GLsizei(camera.get(CV_CAP_PROP_FRAME_WIDTH)));
  const GLsizei capture_env_height(GLsizei(camera.get(CV_CAP_PROP_FRAME_HEIGHT)));

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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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

  // �w�i�Ɏg����`
  const Rect rect;

  // �`��p�̃V�F�[�_
  GgSimpleShader simple("refraction.vert", "refraction.frag");
  const GLint sizeLoc(glGetUniformLocation(simple.get(), "size"));

  // �e�N�X�`�������̂܂ܕ\������V�F�[�_
  const GLuint backimage(ggLoadShader("rectangle.vert", "rectangle.frag"));

  // �f�v�X�f�[�^���璸�_�ʒu���v�Z����V�F�[�_
  const Calculate position(width, height, "position.frag");

  // �o�C���e�����t�B���^�̃V�F�[�_
  const Calculate bilateral(width, height, "bilateral.frag");

  // ���_�ʒu����@���x�N�g�����v�Z����V�F�[�_
  const Calculate normal(width, height, "normal.frag");

  // �w�i�摜
  GLuint bmap;
  glGenTextures(1, &bmap);
  glBindTexture(GL_TEXTURE_2D, bmap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, capture_env_width, capture_env_height, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // �w�i�F��ݒ肷��
  glClearColor(background[0], background[1], background[2], background[3]);

  // �B�ʏ���������L���ɂ���
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // �E�B���h�E���J���Ă���Ԃ���Ԃ��`�悷��
  while (!window.shouldClose())
  {
#if GENERATE_POSITION
    // ���_�ʒu�̌v�Z
    position.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    sensor.getDepth();
    const std::vector<GLuint> &positionTexture(position.calculate());

    // �o�C���e�����t�B���^
    bilateral.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture[0]);
    const std::vector<GLuint> &bilateralTexture(bilateral.calculate());

    // �@���x�N�g���̌v�Z
    normal.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bilateralTexture[0]);
    const std::vector<GLuint> &normalTexture(normal.calculate());
#else
    // �o�C���e�����t�B���^
    bilateral.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    sensor.getPoint();
    const std::vector<GLuint> &bilateralTexture(bilateral.calculate());

    // �@���x�N�g���̌v�Z
    normal.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bilateralTexture[0]);
    const std::vector<GLuint> &normalTexture(normal.calculate());
#endif

    // �摜�̃L���v�`��
    if (camera.grab())
    {
      // �L���v�`���f������摜��؂�o��
      cv::Mat frame;
      camera.retrieve(frame, 3);

      // �؂�o�����摜���e�N�X�`���ɓ]������
      cv::Mat flipped;
      cv::flip(frame, flipped, 0);
      glBindTexture(GL_TEXTURE_2D, bmap);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, flipped.rows, GL_BGR, GL_UNSIGNED_BYTE, flipped.data);
    }

    // ��ʏ���
    window.clear();

    // �w�i�`��
    glUseProgram(backimage);
    glUniform1i(3, 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, bmap);
    glDepthMask(GL_FALSE);
    rect.draw();
    glDepthMask(GL_TRUE);

    // �`��p�̃V�F�[�_�v���O�����̎g�p�J�n
    simple.use();
    simple.loadMatrix(window.getMp(), window.getMw());
    simple.setLight(light);
    simple.setMaterial(material);
    glUniform2f(sizeLoc, GLfloat(window.getSize()[0]), GLfloat(window.getSize()[1]));

    // ���_���W�l�e�N�X�`��
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bilateralTexture[0]);

    // �@���x�N�g���e�N�X�`��
    glUniform1i(1, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture[0]);

    // �w�i�e�N�X�`��
    glUniform1i(3, 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, bmap);

    // �}�`�`��
    mesh.draw();

    // �o�b�t�@�����ւ���
    window.swapBuffers();
  }
}
