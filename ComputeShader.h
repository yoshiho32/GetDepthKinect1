#pragma once

// ウィンドウ関連の処理
#include "Window.h"

//シェーダーのコンパイル関係
#include "Shader.h"

// 矩形
#include "Rect.h"

// 標準ライブラリ
#include <vector>

class ComputeShader : public gg2 {

	// 計算結果を保存するフレームバッファのターゲットに使うテクスチャ
	std::vector<GLuint> texture;

	// レンダリングターゲット
	std::vector<GLenum> bufs;

	// フレームバッファオブジェクトのサイズ
	const GLsizei width, height;

	// 計算用のシェーダプログラム
	const GLuint program;

	// 計算用のシェーダプログラムで使用しているサンプラの uniform 変数の数
	const int uniforms;

public:


	//コンピュートシェーダーの計算結果が入るtexture
	GLuint tex_A;
	GLuint tex_B;

	//インストラクタ
	ComputeShader(int width, int height, const char *source, int uniforms = 1, int targets = 1);

	// デストラクタ
	virtual ~ComputeShader();


	// 計算用のシェーダプログラムの使用を開始する
	void use() const
	{
		glUseProgram(program);
	}

	// 計算を実行する
	const std::vector<GLuint> &calculate() const;

	//programを呼び出す
	GLuint setprogram() {
		return program;
	}

};