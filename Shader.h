#pragma once

#include <fstream>
#include <vector>
#include <iostream>

class gg2 {
public:
	//�v���O�����I�u�W�F�N�g�̃����N���ʂ�\��
	GLboolean _printProgramInfolog(GLuint program) {
		//�����N���ʂ��擾
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) std::cerr << "LINK ERROR" << std::endl;

		//�V�F�[�_�̃����N���̃��O�̒������擾����
		GLsizei bufSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

		if (bufSize > 1) {
			//�V�F�[�_�̃����N���̃��O�̓��e���擾
			std::vector<GLchar> infoLog(bufSize);
			GLsizei length;
			glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
			std::cerr << &infoLog[0] << std::endl;
		}

		return static_cast<GLboolean>(status);

	}

	//�V�F�[�_�[�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
	// shader:�V�F�[�_�I�u�W�F�N�g��
	// str   :�R���p�C���G���[�����������ꏊ������������
	GLboolean _printShaderInfoLog(GLuint shader, const char *str) {
		//�R���p�C�����ʂ̎擾
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) std::cerr << "COMPILER ERROR IN " << str << std::endl;

		//�V�F�[�_�̃R���p�C�����̃��O�̒������擾����
		GLsizei bufSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

		if (bufSize > 1) {
			//�V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
			std::vector<GLchar> infoLog(bufSize);
			GLsizei length;
			glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
			std::cerr << &infoLog[0] << std::endl;
		}

		return static_cast<GLboolean>(status);
	}

	//�V�F�[�_�[�̃v���O�����̓ǂݎ��
	bool _readShaderSource(const char *name, std::vector<GLchar> &buffer) {
		//�t�@�C������null
		if (name == NULL) {
			std::cerr << "�V�F�[�_�[�ʂ��" << std::endl;
			return false;
		}

		//�\�[�X���J��
		std::ifstream file(name, std::ios::binary);
		if (file.fail()) {
			//�J���Ȃ�����
			std::cerr << "cant open shader file : " << name << std::endl;
			return false;
		}

		//�t�@�C���̃T�C�Y��}��
		file.seekg(0L, std::ios::end);
		GLsizei length = static_cast<GLsizei>(file.tellg());

		//�t�@�C���T�C�Y�̃������m��
		buffer.resize(length + 1);

		//�t�@�C����擪����ǂݍ���
		file.seekg(0L, std::ios::beg);
		file.read(buffer.data(), length);
		buffer[length] = '\0';

		//�t�@�C�����ǂݍ��߂Ȃ�����
		if (file.fail()) {
			std::cerr << "couldnt read shader file : " << name << std::endl;
			file.close();
			return false;
		}

		//�ǂݍ��߂�
		file.close();
		return true;
	}

	//�ǂݍ��񂾃V�F�[�_�[����R���s���[�g�V�F�[�_�[�����
	GLuint createCSProgram(const char *csrc)
	{

		//��̃v���O�����I�u�W�F�N�g�𐶐�����
		const GLuint program(glCreateProgram());

		//�����񂪎擾�ł����ꍇ
		if (csrc != NULL) {
			//�R���s���[�g�V�F�[�_�[�̃I�u�W�F�N�g���쐬
			const GLuint cobj(glCreateShader(GL_COMPUTE_SHADER));
			//�V�F�[�_�I�u�W�F�N�g�ɃR�[�h���̕�������i�[
			glShaderSource(cobj, 1, &csrc, NULL);
			//�R���p�C������
			glCompileShader(cobj);

			//�R�[�h���̕�������f�o�b�O(�G���[�Ȃ�\��)
			if (_printShaderInfoLog(cobj, csrc)) {
				//�v���O�����I�u�W�F�N�g�ɃV�F�[�_�I�u�W�F�N�g���A�^�b�`
				glAttachShader(program, cobj);

			}
			//�V�F�[�_�I�u�W�F�N�g���폜
			glDeleteShader(cobj);
		}

		//�v���O�����I�u�W�F�N�g�������N
		glLinkProgram(program);

		return program;
	}


	//�V�F�[�_�[��ǂݍ���Ńv���O�����I�u�W�F�N�g���쐬����
	GLuint loadCSProgram(const char *comp) {
		//�V�F�[�_�[�̃\�[�X�t�@�C����ǂݍ���
		std::vector<GLchar> csrc;
		const bool cstat(_readShaderSource(comp, csrc));

		//�v���O�����I�u�W�F�N�g���쐬
		return cstat ? createCSProgram(csrc.data()) : 0;
	}

};