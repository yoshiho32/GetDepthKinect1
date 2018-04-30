#include "KinectV2.h"

//
// �[�x�Z���T�֘A�̏���
//

// �W�����C�u����
#include <cassert>
#include <fstream>
#include <iostream>
#include "glm/glm.hpp"

// Kinect �֘A
#pragma comment(lib, "Kinect20.lib")

// �v���s�\�_�̃f�t�H���g����
const GLfloat maxDepth(10.0f);

#define filename "result2.txt"
#define POINT_POS 10000
const float variance = 0.01;

// �R���X�g���N�^
KinectV2::KinectV2()
{
	// �Z���T���擾����
	if (sensor == NULL && GetDefaultKinectSensor(&sensor) == S_OK)
	{
		HRESULT hr;

		counter = 0;
		// �Z���T�̎g�p���J�n����
		hr = sensor->Open();
		assert(hr == S_OK);

		// �f�v�X�f�[�^�̓ǂݍ��ݐݒ�
		IDepthFrameSource *depthSource;
		hr = sensor->get_DepthFrameSource(&depthSource);
		assert(hr == S_OK);
		hr = depthSource->OpenReader(&depthReader);
		assert(hr == S_OK);
		IFrameDescription *depthDescription;
		hr = depthSource->get_FrameDescription(&depthDescription);
		assert(hr == S_OK);
		depthSource->Release();

		// �f�v�X�f�[�^�̃T�C�Y�𓾂�
		depthDescription->get_Width(&depthWidth);
		depthDescription->get_Height(&depthHeight);
		depthDescription->Release();

		// �J���[�f�[�^�̓ǂݍ��ݐݒ�
		IColorFrameSource *colorSource;
		hr = sensor->get_ColorFrameSource(&colorSource);
		assert(hr == S_OK);
		hr = colorSource->OpenReader(&colorReader);
		assert(hr == S_OK);
		IFrameDescription *colorDescription;
		hr = colorSource->get_FrameDescription(&colorDescription);
		assert(hr == S_OK);
		colorSource->Release();

		// �J���[�f�[�^�̃T�C�Y�𓾂�
		colorDescription->get_Width(&colorWidth);
		colorDescription->get_Height(&colorHeight);
		colorDescription->Release();

		// ���W�̃}�b�s���O
		hr = sensor->get_CoordinateMapper(&coordinateMapper);
		assert(hr == S_OK);

		// depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
		makeTexture();

		// �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ���������m�ۂ���
		position = new GLfloat[depthCount][3];

		// �J���[�f�[�^��ϊ�����p����ꎞ���������m�ۂ���
		color = new GLubyte[colorCount * 4];
	}
}

// �f�X�g���N�^
KinectV2::~KinectV2()
{
	if (getActivated() > 0)
	{
		// �f�[�^�ϊ��p�̃��������폜����
		delete[] position;
		delete[] color;

		// �Z���T���J������
		colorReader->Release();
		depthReader->Release();
		coordinateMapper->Release();
		sensor->Close();
		sensor->Release();

		// �Z���T���J���������Ƃ��L�^����
		sensor = NULL;
	}
}


// �d�ݕt����f�l�̍��v�Əd�݂̍��v�����߂�
void f(glm::vec3 *csum, glm::vec3 *wsum, const glm::vec3 base, const glm::vec3 c, const glm::vec4 color, const float w)
{
	float step = 1.0;
	if (c.x < 0.1) step = 0.0;
	float step2 = 1.0;
	if (base.r < 0.1) step2 = 0.0;

	glm::vec3 d = glm::vec3(abs(color.x - base.x), abs(color.y - base.y), abs(color.z - base.z));
	glm::vec3 e = glm::vec3(exp(d.x * d.x / variance * (float)(-0.5)) * w + exp(d.y * d.y / variance * (float)(-0.5)) * w + exp(d.z * d.z / variance * (float)(-0.5)) * w, 0, 0);
	csum[0] += c.x * e.x * step * step2;
	wsum[0] += e.x * step * step2;
    //std::cout <<color.x <<"  "<<base.x << std::endl;

}

// �f�v�X�f�[�^���擾����
GLuint KinectV2::getDepth()
{

	//�������ݗp�t�@�C��
	const char *fileName = filename;
	std::ofstream ofs(fileName, std::ios::app);
	if (!ofs) {
		std::cout << "cant open '" << fileName << "' ." << std::endl;
		std::cin.get();
		return 0;
	}

	// �f�v�X�̃e�N�X�`�����w�肷��
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	// ���̃f�v�X�̃t���[���f�[�^���������Ă����
	IDepthFrame *depthFrame;
	if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
	{
		// �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
		UINT depthSize;
		UINT16 *depthBuffer;
		UINT16 depthDSPBuffer;
		depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

		// �J���[�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
		glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
		ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
		coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		IColorFrame *colorFrame;

		if (colorReader->AcquireLatestFrame(&colorFrame) == S_OK)
		{
			
			// �J���[�f�[�^���擾���� RGBA �`���ɕϊ�����
			colorFrame->CopyConvertedFrameDataToArray(colorCount * 4,
				static_cast<BYTE *>(color), ColorImageFormat::ColorImageFormat_Bgra);

			// �J���[�t���[�����J������
			colorFrame->Release();
			//�Ȃ����J���[���g���Ȃ�

		}


		glm::vec3 csum = glm::vec3(depthBuffer[POINT_POS], 0, 0);
		glm::vec3 wsum = glm::vec3(1.0);
		glm::vec3 base = glm::vec3((float)color[POINT_POS * 4],(float)color[POINT_POS * 4  + 1],(float)color[POINT_POS * 4  + 2]);

		int Miss_num = 0;
		//glm::vec3(color[ POINT_POS * 4  - 8 - 8 * depthHeight], 0, 0);
		//std::cout << (float)color[POINT_POS * 4 - 8 - 8 * depthHeight]<< " " <<(float)color[POINT_POS * 4 - 8 - 8 * depthHeight]<<" "<< (float)color[POINT_POS * 4 - 8 - 8 * depthHeight + 2] << std::endl;
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 2 - 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 8 - 8 * depthHeight], (float)color[POINT_POS * 4  - 8 - 8 * depthHeight + 1], (float)color[POINT_POS * 4  - 8 - 8 * depthHeight + 2], 0), 0.018315639);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 1 - 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 4 - 8 * depthHeight], (float)color[POINT_POS * 4  - 4 - 8 * depthHeight + 1], (float)color[POINT_POS * 4  - 4 - 8 * depthHeight + 2], 0), 0.082084999);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS     - 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4      - 8 * depthHeight], (float)color[POINT_POS * 4      - 8 * depthHeight + 1], (float)color[POINT_POS * 4      - 8 * depthHeight + 2], 0), 0.135335283);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 1 - 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 4 - 8 * depthHeight], (float)color[POINT_POS * 4  + 4 - 8 * depthHeight + 1], (float)color[POINT_POS * 4  + 4 - 8 * depthHeight + 2], 0), 0.082084999);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 2 - 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 8 - 8 * depthHeight], (float)color[POINT_POS * 4  + 8 - 8 * depthHeight + 1], (float)color[POINT_POS * 4  + 8 - 8 * depthHeight + 2], 0), 0.018315639);

		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 2 - 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 8 - 4 * depthHeight], (float)color[POINT_POS * 4  - 8 - 4 * depthHeight + 1], (float)color[POINT_POS * 4  - 8 - 4 * depthHeight + 2], 0), 0.082084999);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 1 - 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 4 - 4 * depthHeight], (float)color[POINT_POS * 4  - 4 - 4 * depthHeight + 1], (float)color[POINT_POS * 4  - 4 - 4 * depthHeight + 2], 0), 0.367879441);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS     - 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4       -4 * depthHeight], (float)color[POINT_POS * 4      - 4 * depthHeight + 1], (float)color[POINT_POS * 4      - 4 * depthHeight + 2], 0), 0.60653066);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 1 - 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 4 - 4 * depthHeight], (float)color[POINT_POS * 4  + 4 - 4 * depthHeight + 1], (float)color[POINT_POS * 4  + 4 - 4 * depthHeight + 2], 0), 0.367879441);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 2 - 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 8 - 4 * depthHeight], (float)color[POINT_POS * 4  + 8 - 4 * depthHeight + 1], (float)color[POINT_POS * 4  + 8 - 4 * depthHeight + 2], 0), 0.082084999);

		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 2 - 0 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 8 - 0 * depthHeight], (float)color[POINT_POS * 4  - 8 - 0 * depthHeight + 1], (float)color[POINT_POS * 4  - 8 - 0 * depthHeight + 2], 0), 0.135335283);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 1 - 0 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 4 - 0 * depthHeight], (float)color[POINT_POS * 4  - 4 - 0 * depthHeight + 1], (float)color[POINT_POS * 4  - 4 - 0 * depthHeight + 2], 0),0.60653066);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 1 - 0 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 4 - 0 * depthHeight], (float)color[POINT_POS * 4  + 4 - 0 * depthHeight + 1], (float)color[POINT_POS * 4  + 4 - 0 * depthHeight + 2], 0), 0.60653066);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 2 - 0 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 8 - 0 * depthHeight], (float)color[POINT_POS * 4  - 8 - 0 * depthHeight + 1], (float)color[POINT_POS * 4  + 8 - 0 * depthHeight + 2], 0), 0.135335283);

		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 2 + 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 8 + 4 * depthHeight], (float)color[POINT_POS * 4  - 8 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  - 8 + 4 * depthHeight + 2], 0), 0.082084999);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 1 + 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 4 + 4 * depthHeight], (float)color[POINT_POS * 4  - 4 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  - 4 + 4 * depthHeight + 2], 0), 0.367879441);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS     + 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4      + 4 * depthHeight], (float)color[POINT_POS * 4      + 4 * depthHeight + 1], (float)color[POINT_POS * 4      + 4 * depthHeight + 2], 0), 0.60653066);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 1 + 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 4 + 4 * depthHeight], (float)color[POINT_POS * 4  + 4 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  + 4 + 4 * depthHeight + 2], 0), 0.367879441);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 2 + 1 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 8 + 4 * depthHeight], (float)color[POINT_POS * 4  + 8 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  + 8 + 4 * depthHeight + 2], 0), 0.082084999);

		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 2 + 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 8 + 8 * depthHeight], (float)color[POINT_POS * 4  - 8 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  - 8 + 8 * depthHeight + 2], 0), 0.018315639);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS - 1 + 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  - 4 + 8 * depthHeight], (float)color[POINT_POS * 4  - 4 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  - 4 + 8 * depthHeight + 2], 0), 0.082084999);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS     + 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4      + 8 * depthHeight], (float)color[POINT_POS * 4      + 4 * depthHeight + 1], (float)color[POINT_POS * 4      + 8 * depthHeight + 2], 0), 0.135335283);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 1 + 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 4 + 8 * depthHeight], (float)color[POINT_POS * 4  + 4 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  + 4 + 8 * depthHeight + 2], 0), 0.082084999);
		f(&csum, &wsum, base, glm::vec3(depthBuffer[POINT_POS + 2 + 2 * depthHeight], 0, 0), glm::vec4((float)color[POINT_POS * 4  + 8 + 8 * depthHeight], (float)color[POINT_POS * 4  + 8 + 4 * depthHeight + 1], (float)color[POINT_POS * 4  + 8 + 8 * depthHeight + 2], 0), 0.018315639);

		float smoothed = csum.x / wsum.x;

		if (counter < 1000) {

			ofs << depthBuffer[POINT_POS]<< " " << smoothed << std::endl;
			counter++;
		//	std::cout << csum.r << std::endl;

		}
		if (counter == 1000) std::cout << "end sampling" << std::endl;

		// �f�v�X�f�[�^���e�N�X�`���ɓ]������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RED, GL_UNSIGNED_SHORT, depthBuffer);

		// �f�v�X�t���[�����J������
		depthFrame->Release();
		
	}

	ofs.close();

	return depthTexture;
}
/*
GLuint KinectV2::takeOneDepth() const {

// ���̃f�v�X�̃t���[���f�[�^���������Ă����
IDepthFrame *depthFrame;
if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
{
// �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
UINT depthSize;
UINT16 *depthBuffer;
depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

return depthBuffer[200];
}


};*/


// �J�������W���擾����
GLuint KinectV2::getPoint() const
{
	// �J�������W�̃e�N�X�`�����w�肷��
	glBindTexture(GL_TEXTURE_2D, pointTexture);

	// ���̃f�v�X�̃t���[���f�[�^���������Ă����
	IDepthFrame *depthFrame;
	if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
	{
		// �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
		UINT depthSize;
		UINT16 *depthBuffer;
		depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

		// �J���[�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
		glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
		ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
		coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// �J�������W�ւ̕ϊ��e�[�u���𓾂�
		UINT32 entry;
		PointF *table;
		coordinateMapper->GetDepthFrameToCameraSpaceTable(&entry, &table);

		// ���ׂĂ̓_�ɂ���
		for (unsigned int i = 0; i < entry; ++i)
		{
			// �f�v�X�l�̒P�ʂ����[�g���Ɋ��Z����W��
			static const GLfloat zScale(-0.001f);

			// ���̓_�̃f�v�X�l�𓾂�
			const unsigned short d(depthBuffer[i]);

			// �f�v�X�l�̒P�ʂ����[�g���Ɋ��Z���� (�v���s�\�_�� maxDepth �ɂ���)
			const GLfloat z(d == 0 ? -maxDepth : GLfloat(d) * zScale);

			// ���̓_�̃X�N���[����̈ʒu�����߂�
			const GLfloat x(table[i].X);
			const GLfloat y(-table[i].Y);

			// ���̓_�̃J�������W�����߂�
			position[i][0] = x * z;
			position[i][1] = y * z;
			position[i][2] = z;
		}

		// �J�������W��]������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RGB, GL_FLOAT, position);

		// �e�[�u�����J������
		CoTaskMemFree(table);

		// �f�v�X�t���[�����J������
		depthFrame->Release();
	}

	return pointTexture;
}

// �J���[�f�[�^���擾����
GLuint KinectV2::getColor() const
{
	// �J���[�̃e�N�X�`�����w�肷��
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// ���̃J���[�ƃf�v�X�̃t���[���f�[�^���������Ă����
	//IDepthFrame *depthFrame;
	IColorFrame *colorFrame;
	if (colorReader->AcquireLatestFrame(&colorFrame) == S_OK/* && depthReader->AcquireLatestFrame(&depthFrame) == S_OK*/)
	{
		// �J���[�f�[�^���擾���� RGBA �`���ɕϊ�����
		colorFrame->CopyConvertedFrameDataToArray(colorCount * 4,
			static_cast<BYTE *>(color), ColorImageFormat::ColorImageFormat_Bgra);

		// �J���[�t���[�����J������
		colorFrame->Release();

		// �J���[�f�[�^���e�N�X�`���ɓ]������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorWidth, colorHeight, GL_BGRA, GL_UNSIGNED_BYTE, color);
	}

	return colorTexture;
}

// �Z���T�̎��ʎq
IKinectSensor *KinectV2::sensor(NULL);

