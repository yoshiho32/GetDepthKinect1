
#include "ComputeShader.h"


ComputeShader::ComputeShader(int width, int height, const char *source, int uniforms, int targets)
	: width(width)
	, height(height)
	, program(gg2::loadCSProgram(source))
	, uniforms(uniforms)
{

	//�A�E�g�v�b�g�p�̃e�N�X�`�����쐬����
	glGenTextures(1, &tex_A);
	glBindTexture(GL_TEXTURE_2D, tex_A);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenTextures(1, &tex_B);
	glBindTexture(GL_TEXTURE_2D, tex_B);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

}


ComputeShader::~ComputeShader() {

	// �V�F�[�_�v���O�������폜����
	glDeleteShader(program);

}

// �v�Z�����s����
const std::vector<GLuint> &ComputeShader::calculate() const
{

	glDispatchCompute(width, height, 1);
	//��f�̏������݂��I���܂ő҂�

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//std::cout << tex_output << std::endl;
	return texture;
}
