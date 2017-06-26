#pragma once

//
// �[�x�Z���T�֘A�̏���
//

// DepthSense �֘A
#include <Windows.h>
#include <DepthSense.hxx>
using namespace DepthSense;
#undef INT64_C
#undef UINT64_C

// �W�����C�u����
#include <thread>
#include <mutex>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �[�x�Z���T�֘A�̊��N���X
#include "DepthCamera.h"

// �L���v�`�����̃��b�Z�[�W�o��
#define CAPTURE_VERBOSE         0

// DepthSense �̎��
#define DS311                   0
#define DS325                   1

// �g�p���� DepthSense 
#define CAPTURE_CAMERA          DS325

// DepthSense �̓��샂�[�h
#if CAPTURE_CAMERA == DS325
const FrameFormat capture_depth_format(FRAME_FORMAT_QVGA);
const DepthNode::CameraMode capture_depth_mode(DepthNode::CAMERA_MODE_CLOSE_MODE);
const FrameFormat capture_color_format(FRAME_FORMAT_VGA);
const CompressionType capture_color_compression(COMPRESSION_TYPE_MJPEG);
#else
const FrameFormat capture_depth_format(FRAME_FORMAT_QQVGA);
const DepthNode::CameraMode capture_depth_mode(DepthNode::CAMERA_MODE_LONG_RANGE);
const FrameFormat capture_color_format(FRAME_FORMAT_VGA);
const CompressionType capture_color_compression(COMPRESSION_TYPE_YUY2);
#endif

// �f�v�X�J�����̃t���[�����[�g
const unsigned int capture_depth_fps(60);

// �J���[�J�����̃t���[�����[�g
const unsigned int capture_color_fps(30);

// �d�����g��
const PowerLineFrequency color_frequency(POWER_LINE_FREQUENCY_60HZ);    // �֐�
//const PowerLineFrequency color_frequency(POWER_LINE_FREQUENCY_50HZ);    // �֓�

class Ds325 : public DepthCamera
{
  // �f�v�X�J�����̉𑜓x
  const FrameFormat depth_format;

  // �f�v�X�J�����̃t���[�����[�g
  const unsigned int depth_fps;

  // �f�v�X�J�����̃��[�h
  const DepthNode::CameraMode depth_mode;

  // �J���[�J�����̉𑜓x
  const FrameFormat color_format;

  // �J���[�J�����̃t���[�����[�g
  const unsigned int color_fps;

  // �J���[�J�����̃f�[�^���k����
  const CompressionType color_compression;

  // �d�����g��
  const PowerLineFrequency power_line_frequency;

  // DepthSense �̃R���e�L�X�g
  static Context context;

  // �f�[�^�擾�p�̃X���b�h
  static std::thread worker;

  // �C�x���g���[�v����~���Ă�����C�x���g���[�v���J�n����
  static void startLoop();

  // �m�[�h��o�^����
  void configureNode(Node &node);

  // �m�[�h���폜����
  void unregisterNode(Node node);

  // DepthSense �����t����ꂽ���̏���
  static void Ds325::onDeviceConnected(Context context, Context::DeviceAddedData data);

  // DepthSense �����O���ꂽ�Ƃ��̏���
  static void Ds325::onDeviceDisconnected(Context context, Context::DeviceRemovedData data);

  // �m�[�h���ڑ����ꂽ���̏���
  static void onNodeConnected(Device device, Device::NodeAddedData data, Ds325 *sensor);

  // �m�[�h�̐ڑ����������ꂽ���̏���
  static void onNodeDisconnected(Device device, Device::NodeRemovedData data, Ds325 *sensor);

  // �f�v�X�m�[�h
  DepthNode depthNode;

  // �f�v�X�m�[�h������������
  void configureDepthNode(DepthNode &dnode);

  // �f�v�X�m�[�h�̃C�x���g�������̏���
  static void onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data, Ds325 *sensor);

  // �f�v�X�m�[�h�p�� mutex
  std::mutex depthMutex;

  // �J���[�m�[�h
  ColorNode colorNode;

  // �J���[�m�[�h������������
  void configureColorNode(ColorNode &cnode);

  // �J���[�m�[�h�̃C�x���g�������̏���
  static void onNewColorSample(ColorNode node, ColorNode::NewSampleReceivedData data, Ds325 *sensor);

  // �J���[�m�[�h�p�� mutex
  std::mutex colorMutex;

  // �f�v�X�f�[�^�]���p�̃�����
  GLshort *depth, *depthBuffer;

  // �J�������W�]���p�̃�����
  GLfloat *point;

  // �e�N�X�`�����W�]���p�̃�����
  GLfloat *uvmap;

  // �J���[�f�[�^�]���p�̃�����
  GLubyte *color, *colorBuffer;

  // �R�s�[�R���X�g���N�^ (�R�s�[�֎~)
  Ds325(const Ds325 &w);

  // ��� (����֎~)
  Ds325 &operator=(const Ds325 &w);

public:

  // �R���X�g���N�^
  Ds325(
    FrameFormat depth_format = capture_depth_format,  // �f�v�X�J�����̉𑜓x
    unsigned int depth_fps = capture_depth_fps,       // �f�v�X�J�����̃t���[�����[�g
    DepthNode::CameraMode = capture_depth_mode,       // �f�v�X�J�����̃��[�h
    FrameFormat color_format = capture_color_format,  // �J���[�J�����̉𑜓x
    unsigned int color_fps = capture_color_fps,       // �J���[�J�����̃t���[�����[�g
    CompressionType color_compression = capture_color_compression,  // �J���[�J�����̈��k����
    PowerLineFrequency frequency = color_frequency    // �d�����g��
    );

  // �f�X�g���N�^
  virtual ~Ds325();

  // �f�v�X�f�[�^���擾����
  GLuint getDepth();

  // �J�������W���擾����
  GLuint getPoint();

  // �J���[�f�[�^���擾����
  GLuint getColor();
};
