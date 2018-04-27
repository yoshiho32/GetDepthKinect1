#pragma once

//
// �[�x�Z���T�֘A�̏���
//

// Kinect �֘A
#include <Windows.h>
#include <Kinect.h>

// �[�x�Z���T�֘A�̊��N���X
#include "DepthCamera.h"

class KinectV2 : public DepthCamera
{
	// �Z���T�̎��ʎq
	static IKinectSensor *sensor;

	// �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ������
	GLfloat(*position)[3];

	// �J���[�f�[�^
	IColorFrameReader *colorReader;

	// �J���[�f�[�^�̕ϊ��ɗp����ꎞ������
	GLubyte *color;

	// ���W�̃}�b�s���O
	ICoordinateMapper *coordinateMapper;

	// �R�s�[�R���X�g���N�^ (�R�s�[�֎~)
	KinectV2(const KinectV2 &w);

	// ��� (����֎~)
	KinectV2 &operator=(const KinectV2 &w);

public:

	int counter;

	// �f�v�X�f�[�^
	IDepthFrameReader *depthReader;

	// �R���X�g���N�^
	KinectV2();

	// �f�X�g���N�^
	virtual ~KinectV2();

	// �f�v�X�f�[�^���擾����
	GLuint getDepth();

	// �J�������W���擾����
	GLuint getPoint() const;

	// �J���[�f�[�^���擾����
	GLuint getColor() const;

	// �f�v�X�f�[�^�̈���擾����
	//GLuint takeOneDepth() const;

};
