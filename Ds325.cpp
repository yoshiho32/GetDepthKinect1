#include "Ds325.h"

//
// �[�x�Z���T�֘A�̏���
//

// �W�����C�u����
#include <iostream>

// DepthSense �֘A
#pragma comment (lib, "DepthSense.lib")

// �v���s�\�_�̃f�t�H���g����
#if CAPTURE_CAMERA == DS325
const GLfloat maxDepth(3.0f);
#else
const GLfloat maxDepth(6.0f);
#endif

// �R���X�g���N�^
Ds325::Ds325(
  FrameFormat depth_format,           // �f�v�X�J�����̉𑜓x
  unsigned int depth_fps,             // �f�v�X�J�����̃t���[�����[�g
  DepthNode::CameraMode depth_mode,   // �f�v�X�J�����̃��[�h
  FrameFormat color_format,           // �J���[�J�����̉𑜓x
  unsigned int color_fps,             // �J���[�J�����̃t���[�����[�g
  CompressionType color_compression,  // �J���[�J�����̈��k����
  PowerLineFrequency frequency        // �d�����g��
  )
  : depth_format(depth_format)
  , depth_fps(depth_fps)
  , depth_mode(depth_mode)
  , color_format(color_format)
  , color_fps(color_fps)
  , color_compression(color_compression)
  , power_line_frequency(frequency)
{
  // �C�x���g���[�v���J�n���Đڑ�����Ă��� DepthSense �̐������߂�
  startLoop();

  // DepthSense ���ڑ�����Ă���g�p�䐔���ڑ��䐔�ɒB���Ă��Ȃ����
  if (getActivated() < connected)
  {
    // ���g�p�� DepthSense �����o��
    Device device(context.getDevices()[getActivated()]);

    // DepthSense �̃m�[�h�̃C�x���g�n���h����o�^����
    device.nodeAddedEvent().connect(&onNodeConnected, this);
    device.nodeRemovedEvent().connect(&onNodeDisconnected, this);

    // DepthSense �̃t���[���t�H�[�}�b�g����𑜓x�����߂�
    FrameFormat_toResolution(depth_format, &depthWidth, &depthHeight);
    FrameFormat_toResolution(color_format, &colorWidth, &colorHeight);

    // depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
    makeTexture();

    // �f�[�^�]���p�̃��������m�ۂ���
    depthBuffer = new GLshort[depthCount];
    point = new GLfloat[depthCount * 3];
    uvmap = new GLfloat[depthCount * 2];
    colorBuffer = new GLubyte[colorCount * 3];

    // DepthSense �̊e�m�[�h������������
    for (Node &node : device.getNodes()) configureNode(node);
  }
}

// �f�X�g���N�^
Ds325::~Ds325()
{
  // DepthSense ���L���ɂȂ��Ă�����
  if (getActivated() > 0)
  {
    // �m�[�h�̓o�^����
    unregisterNode(colorNode);
    unregisterNode(depthNode);

    // �f�[�^�]���p�̃������̊J��
    delete[] depthBuffer;
    delete[] point;
    delete[] uvmap;
    delete[] colorBuffer;
  }

  // �Ō�� DepthSense ���폜����Ƃ��̓C�x���g���[�v���~����
  if (getActivated() <= 1 && worker.joinable())
  {
    // �C�x���g���[�v���~����
    context.quit();

    // �C�x���g���[�v�̃X���b�h���I������̂�҂�
    worker.join();

    // �X�g���[�~���O���~����
    context.stopNodes();
  }
}

// �C�x���g���[�v����~���Ă�����C�x���g���[�v���J�n����
void Ds325::startLoop()
{
  // �X���b�h�������Ă��Ȃ�������
  if (!worker.joinable())
  {
    // DepthSense �̃T�[�o�ɐڑ�����
    context = Context::create();

    // DepthSense �̎��t���^���O�����̏�����o�^����
    context.deviceAddedEvent().connect(&onDeviceConnected);
    context.deviceRemovedEvent().connect(&onDeviceDisconnected);

    // ���ݐڑ�����Ă��� DepthSense �̐��𐔂���
    connected = context.getDevices().size();

    // �X�g���[�~���O���J�n����
    context.startNodes();

    // �C�x���g���[�v���J�n����
    worker = std::thread([&]() { context.run(); });
  }
}

// DepthSense �̃m�[�h�̓o�^
void Ds325::configureNode(Node &node)
{
  if (node.is<DepthNode>() && !depthNode.isSet())
  {
    // �f�v�X�f�[�^�̎擾�O�Ȃ̂Ńe�N�X�`����o�b�t�@�I�u�W�F�N�g�ւ̓]���͍s��Ȃ�
    depth = nullptr;

    // �f�v�X�m�[�h
    depthNode = node.as<DepthNode>();

    // �f�v�X�m�[�h�̃C�x���g�������̏����̓o�^
    depthNode.newSampleReceivedEvent().connect(&onNewDepthSample, this);

    // �f�v�X�m�[�h�̏����ݒ�
    configureDepthNode(depthNode);

    // �f�v�X�m�[�h�̓o�^
    context.registerNode(depthNode);
  }
  else if (node.is<ColorNode>() && !colorNode.isSet())
  {
    // �J���[�f�[�^�̎擾�O�Ȃ̂Ńe�N�X�`���ւ̓]���͍s��Ȃ�
    color = nullptr;

    // �J���[�m�[�h
    colorNode = node.as<ColorNode>();

    // �J���[�m�[�h�̃C�x���g�������̏����̓o�^
    colorNode.newSampleReceivedEvent().connect(&onNewColorSample, this);

    // �J���[�m�[�h�̏����ݒ�
    configureColorNode(colorNode);

    // �J���[�m�[�h�̓o�^
    context.registerNode(colorNode);
  }
}

// �m�[�h�̍폜
void Ds325::unregisterNode(Node node)
{
  if (node.is<DepthNode>() && depthNode.isSet())
  {
    // �f�v�X�f�[�^�̎擾�I��
    depthNode.newSampleReceivedEvent().disconnect(&onNewDepthSample, this);
    context.unregisterNode(depthNode);
    depthNode.unset();

    // �f�v�X�f�[�^�̎擾���I�������̂Ńe�N�X�`����o�b�t�@�I�u�W�F�N�g�ւ̓]���͍s��Ȃ�
    depth = nullptr;
  }
  else if (node.is<ColorNode>() && colorNode.isSet())
  {
    // �J���[�f�[�^�̎擾�I��
    colorNode.newSampleReceivedEvent().disconnect(&onNewColorSample, this);
    context.unregisterNode(colorNode);
    colorNode.unset();

    // �J���[�f�[�^�̎擾���I�������̂łȂ̂Ńe�N�X�`���ւ̓]���͍s��Ȃ�
    color = nullptr;
  }
}

// DepthSense �����t����ꂽ���̏���
void Ds325::onDeviceConnected(Context context, Context::DeviceAddedData data)
{
  MessageBox(NULL, TEXT("DepthSense �����t�����܂����B"), TEXT("�����ł���"), MB_OK);

  // �C�x���g���[�v���J�n���Đڑ�����Ă��� DepthSense �̐����X�V����
  startLoop();
}

// DepthSense �����O���ꂽ�Ƃ��̏���
void Ds325::onDeviceDisconnected(Context context, Context::DeviceRemovedData data)
{
  MessageBox(NULL, TEXT("DepthSense �����O����܂����B"), TEXT("�����ł���"), MB_OK);

  // �X���b�h�������Ă���ΐڑ�����Ă��� DepthSense �̐����X�V����
  if (worker.joinable()) connected = context.getDevices().size();
}

// �m�[�h���ڑ����ꂽ���̏���
void Ds325::onNodeConnected(Device device, Device::NodeAddedData data, Ds325 *sensor)
{
  sensor->configureNode(data.node);
}

// �m�[�h�̐ڑ����������ꂽ���̏���
void Ds325::onNodeDisconnected(Device device, Device::NodeRemovedData data, Ds325 *sensor)
{
  if (data.node.is<DepthNode>() && (data.node.as<DepthNode>() == sensor->depthNode))
    sensor->depthNode.unset();
  if (data.node.is<ColorNode>() && (data.node.as<ColorNode>() == sensor->colorNode))
    sensor->colorNode.unset();
}

// DepthSense �̃f�v�X�m�[�h�̏�����
void Ds325::configureDepthNode(DepthNode &dnode)
{
  // �f�v�X�m�[�h�̏����ݒ�
  DepthNode::Configuration config(dnode.getConfiguration());
  config.frameFormat = depth_format;
  config.framerate = depth_fps;
  config.mode = depth_mode;
  config.saturation = true;

  // ���_�f�[�^�擾�̗L����
  dnode.setEnableDepthMap(true);
  dnode.setEnableVerticesFloatingPoint(true);
  dnode.setEnableUvMap(true);

  // �ݒ���s
  try
  {
    context.requestControl(dnode, 0);
    dnode.setConfiguration(config);
  }
  catch (ArgumentException &e)
  {
    std::cout << "Argument Exception: " << e.what() << std::endl;
  }
  catch (UnauthorizedAccessException &e)
  {
    std::cout << "Unauthorized Access Exception: " << e.what() << std::endl;
  }
  catch (IOException& e)
  {
    std::cout << "IO Exception: " << e.what() << std::endl;
  }
  catch (InvalidOperationException &e)
  {
    std::cout << "Invalid Operation Exception: " << e.what() << std::endl;
  }
  catch (ConfigurationException &e)
  {
    std::cout << "Configuration Exception: " << e.what() << std::endl;
  }
  catch (StreamingException &e)
  {
    std::cout << "Streaming Exception: " << e.what() << std::endl;
  }
  catch (TimeoutException &)
  {
    std::cout << "TimeoutException" << std::endl;
  }
}

// DepthSense �̃f�v�X�m�[�h�̃C�x���g�������̏���
void Ds325::onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data, Ds325 *sensor)
{
  // �f�v�X�J�����̓����p�����[�^
  const int &dw(data.stereoCameraParameters.depthIntrinsics.width);
  const int &dh(data.stereoCameraParameters.depthIntrinsics.height);
  const float &dcx(data.stereoCameraParameters.depthIntrinsics.cx);
  const float &dcy(data.stereoCameraParameters.depthIntrinsics.cy);
  const float &dfx(data.stereoCameraParameters.depthIntrinsics.fx);
  const float &dfy(data.stereoCameraParameters.depthIntrinsics.fy);
  const float &dk1(data.stereoCameraParameters.depthIntrinsics.k1);
  const float &dk2(data.stereoCameraParameters.depthIntrinsics.k2);
  const float &dk3(data.stereoCameraParameters.depthIntrinsics.k3);

  // �J���[�J�����̓����p�����[�^
  const int &cw(data.stereoCameraParameters.colorIntrinsics.width);
  const int &ch(data.stereoCameraParameters.colorIntrinsics.height);
  const float &ccx(data.stereoCameraParameters.colorIntrinsics.cx);
  const float &ccy(data.stereoCameraParameters.colorIntrinsics.cy);
  const float &cfx(data.stereoCameraParameters.colorIntrinsics.fx);
  const float &cfy(data.stereoCameraParameters.colorIntrinsics.fy);
  const float &ck1(data.stereoCameraParameters.colorIntrinsics.k1);
  const float &ck2(data.stereoCameraParameters.colorIntrinsics.k2);
  const float &ck3(data.stereoCameraParameters.colorIntrinsics.k3);

  // �f�v�X�J������
  const GLfloat fovx(dfx * maxDepth / dcx);
  const GLfloat fovy(dfy * maxDepth / dcy);

  // �f�[�^�]��
  sensor->depthMutex.lock();
  for (int i = 0; i < sensor->depthCount; ++i)
  {
    // �f�v�X�}�b�v�̉�f�ʒu
    const int u(i % dw);
    const int t(i / dw);
    const int v(dh - t - 1);

    // �]����̃}�b�v�͏㉺�𔽓]����
    const int j(v * dw + u);

    // �f�v�X�l��]������
    sensor->depthBuffer[j] = data.depthMap[i];

    // ��f���v���s�\�Ȃ�
    if (sensor->depthBuffer[j] > 32000)
    {
      // ��f�ʒu����f�v�X�}�b�v�̃X�N���[�����W�����߂�
      const GLfloat dx((static_cast<GLfloat>(u) - dcx + 0.5f) / dfx);
      const GLfloat dy((static_cast<GLfloat>(v) - dcy + 0.5f) / dfy);

      // �f�v�X�J�����̘c�ݕ␳�W��
      const GLfloat dr(dx * dx + dy * dy);
      const GLfloat dq(1.0f + dr * (dk1 + dr * (dk2 + dr * dk3)));

      // �c�݂�␳�����|�C���g�̃X�N���[�����W�l
      const GLfloat x(dx / dq);
      const GLfloat y(dy / dq);

      // �v���s�\�_���ŉ��_�ɂ��ăJ�������W�����߂�
      sensor->point[j * 3 + 0] = x * maxDepth;
      sensor->point[j * 3 + 1] = y * maxDepth;
      sensor->point[j * 3 + 2] = -maxDepth;

      // �J���[�J�����̘c�ݕ␳�W��
      const GLfloat cr(x * x + y * y);
      const GLfloat cq(1.0f + cr * (ck1 + cr * (ck2 + cr * ck3)));

      // �|�C���g�̃J�������W
      const GLfloat cx((x + 0.0508f) / cq);
      const GLfloat cy(y / cq);

      // �c�݂�␳�����|�C���g�̃e�N�X�`�����W�l
      sensor->uvmap[j * 2 + 0] = ccx + cx * cfx;
      sensor->uvmap[j * 2 + 1] = ccy - cy * cfy;
    }
    else
    {
      sensor->point[j * 3 + 0] = data.verticesFloatingPoint[i].x;
      sensor->point[j * 3 + 1] = data.verticesFloatingPoint[i].y;
      sensor->point[j * 3 + 2] = -data.verticesFloatingPoint[i].z;
      sensor->uvmap[j * 2 + 0] = data.uvMap[i].u * static_cast<GLfloat>(cw);
      sensor->uvmap[j * 2 + 1] = data.uvMap[i].v * static_cast<GLfloat>(ch);
    }
  }
  sensor->depth = sensor->depthBuffer;
  sensor->depthMutex.unlock();
}

// DepthSense �̃J���[�m�[�h�̏�����
void Ds325::configureColorNode(ColorNode &cnode)
{
  // �J���[�m�[�h�̏����ݒ�
  ColorNode::Configuration config(cnode.getConfiguration());
  config.frameFormat = color_format;
  config.compression = color_compression;
  config.powerLineFrequency = power_line_frequency;
  config.framerate = color_fps;

  // �F�f�[�^�̎擾�̗L����
  cnode.setEnableColorMap(true);

  try
  {
    context.requestControl(cnode, 0);
    cnode.setConfiguration(config);
  }
  catch (ArgumentException& e)
  {
    std::cout << "Argument Exception: " << e.what() << std::endl;
  }
  catch (UnauthorizedAccessException &e)
  {
    std::cout << "Unauthorized Access Exception:" << e.what() << std::endl;
  }
  catch (IOException &e)
  {
    std::cout << "IO Exception: " << e.what() << std::endl;
  }
  catch (InvalidOperationException &e)
  {
    std::cout << "Invalid Operation Exception: " << e.what() << std::endl;
  }
  catch (ConfigurationException &e)
  {
    std::cout << "Configuration Exception: " << e.what() << std::endl;
  }
  catch (StreamingException &e)
  {
    std::cout << "Streaming Exception: " << e.what() << std::endl;
  }
  catch (TimeoutException &)
  {
    std::cout << "TimeoutException" << std::endl;
  }
}

// DepthSense �̃J���[�m�[�h�̃C�x���g�������̏���
void Ds325::onNewColorSample(ColorNode node, ColorNode::NewSampleReceivedData data, Ds325 *sensor)
{
  // �J���[�f�[�^��]������
  sensor->colorMutex.lock();
  if (sensor->color_compression == COMPRESSION_TYPE_MJPEG)
  {
    memcpy(sensor->colorBuffer, data.colorMap, sensor->colorCount * 3 * sizeof(GLubyte));
  }
  else
  {
    // ITU-R BT.601 / ITU-R BT.709 (1250/50/2:1)
    //
    //   R = Y + 1.402 �~ Cr
    //   G = Y - 0.344136 �~ Cb - 0.714136 �~ Cr
    //   B = Y + 1.772 �~ Cb

    // ITU - R BT.709 (1125 / 60 / 2:1)
    //
    //   R = Y + 1.5748 �~ Cr
    //   G = Y - 0.187324 �~ Cb - 0.468124 �~ Cr
    //   B = Y + 1.8556 �~ Cb

    for (int i = 0; i < sensor->colorCount; ++i)
    {
      const int iy(i * 2), iu((iy & ~3) + 1), iv(iu + 2), j(i * 3);
      const float y(static_cast<float>(data.colorMap[iy] - 16));
      const float u(static_cast<float>(data.colorMap[iu] - 128));
      const float v(static_cast<float>(data.colorMap[iv] - 128));
      const float r(y + 1.402f * v);
      const float g(y - 0.344136f * u - 0.714136f * v);
      const float b(y + 1.772f * u);
      sensor->colorBuffer[j + 0] = b > 0.0f ? static_cast<GLubyte>(b) : 0;
      sensor->colorBuffer[j + 1] = g > 0.0f ? static_cast<GLubyte>(g) : 0;
      sensor->colorBuffer[j + 2] = r > 0.0f ? static_cast<GLubyte>(r) : 0;
    }
  }
  sensor->color = sensor->colorBuffer;
  sensor->colorMutex.unlock();
}

// �f�v�X�f�[�^���擾����
GLuint Ds325::getDepth()
{
  // �f�v�X�f�[�^�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  // �f�v�X�f�[�^���X�V����Ă����
  if (depth)
  {
    // DepthSense ���f�v�X�f�[�^�̎擾���łȂ����
    if (depthMutex.try_lock())
    {
      // �e�N�X�`�����W�̃o�b�t�@�I�u�W�F�N�g���w�肷��
      glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);

      // �e�N�X�`�����W���o�b�t�@�I�u�W�F�N�g�ɓ]������
      glBufferSubData(GL_ARRAY_BUFFER, 0, depthCount * 2 * sizeof(GLfloat), uvmap);

      // �f�v�X�f�[�^���e�N�X�`���ɓ]������
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RED, GL_SHORT, depth);

      // ��x�����Ă��܂��΍X�V�����܂ő���K�v���Ȃ��̂Ńf�[�^�͕s�v
      depth = nullptr;

      // �f�v�X�f�[�^�̃��b�N���J������
      depthMutex.unlock();
    }
  }

  return depthTexture;
}

// �J�������W���擾����
GLuint Ds325::getPoint()
{
  // �J�������W�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, pointTexture);

  // �f�v�X�f�[�^���X�V����Ă����
  if (depth)
  {
    // DepthSense ���f�v�X�f�[�^�̎擾���łȂ����
    if (depthMutex.try_lock())
    {
      // �e�N�X�`�����W�̃o�b�t�@�I�u�W�F�N�g���w�肷��
      glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);

      // �e�N�X�`�����W���o�b�t�@�I�u�W�F�N�g�ɓ]������
      glBufferSubData(GL_ARRAY_BUFFER, 0, depthCount * 2 * sizeof(GLfloat), uvmap);

      // �J�������W���e�N�X�`���ɓ]������
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RGB, GL_FLOAT, point);

      // ��x�����Ă��܂��΍X�V�����܂ő���K�v���Ȃ��̂Ńf�[�^�͕s�v
      depth = nullptr;

      // �f�v�X�f�[�^�̃��b�N���J������
      depthMutex.unlock();
    }
  }

  return pointTexture;
}

// �J���[�f�[�^���擾����
GLuint Ds325::getColor()
{
  // �J���[�f�[�^�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, colorTexture);

  // �J���[�f�[�^���X�V����Ă����
  if (color)
  {
    // DepthSense ���J���[�f�[�^�̎擾���łȂ����
    if (colorMutex.try_lock())
    {
      // �J���[�f�[�^���e�N�X�`���ɓ]������
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorWidth, colorHeight, GL_BGR, GL_UNSIGNED_BYTE, color);

      // ��x�����Ă��܂��΍X�V�����܂ő���K�v���Ȃ��̂Ńf�[�^�͕s�v
      color = nullptr;

      // �J���[�f�[�^�̃��b�N���J������
      colorMutex.unlock();
    }
  }

  return colorTexture;
}

// DepthSense �̃R���e�L�X�g
Context Ds325::context;

// �f�[�^�擾�p�̃X���b�h
std::thread Ds325::worker;
