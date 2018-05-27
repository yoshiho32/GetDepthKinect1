#pragma once

#include <fstream>
#include <vector>
#include <iostream>

class gg2 {
public:
	//プログラムオブジェクトのリンク結果を表示
	GLboolean _printProgramInfolog(GLuint program) {
		//リンク結果を取得
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) std::cerr << "LINK ERROR" << std::endl;

		//シェーダのリンク時のログの長さを取得する
		GLsizei bufSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

		if (bufSize > 1) {
			//シェーダのリンク時のログの内容を取得
			std::vector<GLchar> infoLog(bufSize);
			GLsizei length;
			glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
			std::cerr << &infoLog[0] << std::endl;
		}

		return static_cast<GLboolean>(status);

	}

	//シェーダーオブジェクトのコンパイル結果を表示する
	// shader:シェーダオブジェクト名
	// str   :コンパイルエラーが発生した場所を示す文字列
	GLboolean _printShaderInfoLog(GLuint shader, const char *str) {
		//コンパイル結果の取得
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) std::cerr << "COMPILER ERROR IN " << str << std::endl;

		//シェーダのコンパイル時のログの長さを取得する
		GLsizei bufSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

		if (bufSize > 1) {
			//シェーダのコンパイル時のログの内容を取得する
			std::vector<GLchar> infoLog(bufSize);
			GLsizei length;
			glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
			std::cerr << &infoLog[0] << std::endl;
		}

		return static_cast<GLboolean>(status);
	}

	//シェーダーのプログラムの読み取り
	bool _readShaderSource(const char *name, std::vector<GLchar> &buffer) {
		//ファイル名がnull
		if (name == NULL) {
			std::cerr << "シェーダーぬるぽ" << std::endl;
			return false;
		}

		//ソースを開く
		std::ifstream file(name, std::ios::binary);
		if (file.fail()) {
			//開けなかった
			std::cerr << "cant open shader file : " << name << std::endl;
			return false;
		}

		//ファイルのサイズを図る
		file.seekg(0L, std::ios::end);
		GLsizei length = static_cast<GLsizei>(file.tellg());

		//ファイルサイズのメモリ確保
		buffer.resize(length + 1);

		//ファイルを先頭から読み込む
		file.seekg(0L, std::ios::beg);
		file.read(buffer.data(), length);
		buffer[length] = '\0';

		//ファイルが読み込めなかった
		if (file.fail()) {
			std::cerr << "couldnt read shader file : " << name << std::endl;
			file.close();
			return false;
		}

		//読み込めた
		file.close();
		return true;
	}

	//読み込んだシェーダーからコンピュートシェーダーを作る
	GLuint createCSProgram(const char *csrc)
	{

		//空のプログラムオブジェクトを生成する
		const GLuint program(glCreateProgram());

		//文字列が取得できた場合
		if (csrc != NULL) {
			//コンピュートシェーダーのオブジェクトを作成
			const GLuint cobj(glCreateShader(GL_COMPUTE_SHADER));
			//シェーダオブジェクトにコード内の文字列を格納
			glShaderSource(cobj, 1, &csrc, NULL);
			//コンパイルする
			glCompileShader(cobj);

			//コード内の文字列をデバッグ(エラーなら表示)
			if (_printShaderInfoLog(cobj, csrc)) {
				//プログラムオブジェクトにシェーダオブジェクトをアタッチ
				glAttachShader(program, cobj);

			}
			//シェーダオブジェクトを削除
			glDeleteShader(cobj);
		}

		//プログラムオブジェクトをリンク
		glLinkProgram(program);

		return program;
	}


	//シェーダーを読み込んでプログラムオブジェクトを作成する
	GLuint loadCSProgram(const char *comp) {
		//シェーダーのソースファイルを読み込む
		std::vector<GLchar> csrc;
		const bool cstat(_readShaderSource(comp, csrc));

		//プログラムオブジェクトを作成
		return cstat ? createCSProgram(csrc.data()) : 0;
	}

};