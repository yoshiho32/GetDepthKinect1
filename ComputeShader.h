#pragma once

// �E�B���h�E�֘A�̏���
#include "Window.h"

//�V�F�[�_�[�̃R���p�C���֌W
#include "Shader.h"

// ��`
#include "Rect.h"

// �W�����C�u����
#include <vector>

class ComputeShader : public gg2 {

	// �v�Z���ʂ�ۑ�����t���[���o�b�t�@�̃^�[�Q�b�g�Ɏg���e�N�X�`��
	std::vector<GLuint> texture;

	// �����_�����O�^�[�Q�b�g
	std::vector<GLenum> bufs;

	// �t���[���o�b�t�@�I�u�W�F�N�g�̃T�C�Y
	const GLsizei width, height;

	// �v�Z�p�̃V�F�[�_�v���O����
	const GLuint program;

	// �v�Z�p�̃V�F�[�_�v���O�����Ŏg�p���Ă���T���v���� uniform �ϐ��̐�
	const int uniforms;

public:


	//�R���s���[�g�V�F�[�_�[�̌v�Z���ʂ�����texture
	GLuint tex_output;

	//�C���X�g���N�^
	ComputeShader(int width, int height, const char *source, int uniforms = 1, int targets = 1);

	// �f�X�g���N�^
	virtual ~ComputeShader();


	// �v�Z�p�̃V�F�[�_�v���O�����̎g�p���J�n����
	void use() const
	{
		glUseProgram(program);
	}

	// �v�Z�����s����
	const std::vector<GLuint> &calculate() const;

	//program���Ăяo��
	GLuint setprogram() {
		return program;
	}

};