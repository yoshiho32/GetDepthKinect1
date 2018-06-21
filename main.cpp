//
// Kinect (v2) のデプスマップ取得
//

// 標準ライブラリ
#include <Windows.h>

// ウィンドウ関連の処理
#include "Window.h"

// センサ関連の処理
#include "KinectV2.h"

// 描画に用いるメッシュ
#include "Mesh.h"

// 計算に用いるシェーダ
#include "Calculate.h"

//カルマンフィルターの計算に用いるコンピュートシェーダ
#include "ComputeShader.h"

// 頂点位置の生成をシェーダ (position.frag) で行うなら 1
#define GENERATE_POSITION 1
#define SOBEL_FILTER 0

//
// メインプログラム
//
int main()
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // GLFW の初期化に失敗した
    MessageBox(NULL, TEXT("GLFW の初期化に失敗しました。"), TEXT("すまんのう"), MB_OK);
    return EXIT_FAILURE;
  }

  // プログラム終了時には GLFW を終了する
  atexit(glfwTerminate);

  // OpenGL Version 3.2 Core Profile を選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // ウィンドウを開く
  Window window(640, 480, "Depth Map Viewer");
  if (!window.get())
  {
    // ウィンドウが作成できなかった
    MessageBox(NULL, TEXT("GLFW のウィンドウが開けませんでした。"), TEXT("すまんのう"), MB_OK);
    return EXIT_FAILURE;
  }

  // 深度センサを有効にする
  KinectV2 sensor;
  if (sensor.getActivated() == 0)
  {
    // センサが使えなかった
    MessageBox(NULL, TEXT("深度センサを有効にできませんでした。"), TEXT("すまんのう"), MB_OK);
    return EXIT_FAILURE;
  }

  // 深度センサの解像度
  int width, height;
  sensor.getDepthResolution(&width, &height);

  // 描画に使うメッシュ
  const Mesh mesh(width, height, sensor.getCoordBuffer());

  // 描画用のシェーダ
  GgSimpleShader simple("simple.vert", "simple.frag");

  // デプスデータから頂点位置を計算するシェーダ
  const Calculate position(width, height, "position.frag");

  // 頂点位置から法線ベクトルを計算するシェーダ
  const Calculate normal(width, height, "normal.frag");

#ifdef SOBEL_FILTER
  //微分フィルター用のコンピュートシェーダ
  ComputeShader sobel(width, height, "sobel.comp");
#endif

  //カルマンフィルター用のコンピュートシェーダ
  ComputeShader kalman(width, height, "kalman.comp");

  //カルマンフィルタ用変数
  const float Q(0.0001f); 
  const float R(0.01f);
  float P(0.0f);
  float K(0.0f);
  GLint KLoc = glGetUniformLocation(kalman.setprogram(), "K");
  int SwitchKalman(0);

  // 背景色を設定する
  glClearColor(background[0], background[1], background[2], background[3]);

  // 隠面消去処理を有効にする
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // ウィンドウが開いている間くり返し描画する
  while (!window.shouldClose())
  {
#if GENERATE_POSITION

	//カルマンフィルターの計算ーーーーーーーーーーーーーーーーーーーーー
	// カルマンフィルター用変数の計算
	K = (P + Q) / (P + Q + R);
	P = R * (P + Q) / (R + P + Q);

	kalman.use();
	//uniform変数KとしてGPUにデータを渡す
	glUniform1f(KLoc, K);

	//depthデータの転送
	glUniform1i(0, 0);
	glActiveTexture(GL_TEXTURE0);
	sensor.getDepth();

	//テクスチャを入れ替えて計算する
	glActiveTexture(GL_TEXTURE1);
	if (SwitchKalman) {
		//テクスチャの設定
		glBindImageTexture(1, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//計算結果を入れる場所を設定
		glActiveTexture(GL_TEXTURE2);
		glBindImageTexture(2, kalman.tex_A, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//std::cout << "A" << std::endl;
	}
	else {
		//テクスチャの設定
		glBindImageTexture(1, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//計算結果を入れる場所を設定
		glActiveTexture(GL_TEXTURE2);
		glBindImageTexture(2, kalman.tex_B, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//std::cout << "B" << std::endl;
	}
	//処理の実行
	kalman.calculate();
	//カルマンフィルター終わりーーーーーーーーーーーーーーーーーーー

#ifdef SOBEL_FILTER
	//微分フィルター
	sobel.use();
	glActiveTexture(GL_TEXTURE0);
	if (SwitchKalman) {
		glBindImageTexture(0, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}
	else {
		glBindImageTexture(0, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindImageTexture(1, sobel.tex_A, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	sobel.calculate();
#endif

    // 頂点位置の計算
    position.use();

	//処理済みのデプスデータを渡す

    glActiveTexture(GL_TEXTURE0);
	//交互に入ってる場所を参照して、計算した予測位置を渡す
	if (SwitchKalman) {
		glBindImageTexture(0, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}
	else {
		glBindImageTexture(0, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	}

	glUniform1i(2, 2);
	glActiveTexture(GL_TEXTURE2);
	sensor.getColor();

	const std::vector<GLuint> &positionTexture(position.calculate());

    // 法線ベクトルの計算
    normal.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positionTexture[0]);
    const std::vector<GLuint> &normalTexture(normal.calculate());

#else
    // 法線ベクトルの計算
    normal.use();
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    sensor.getPoint();
    const std::vector<GLuint> &normalTexture(normal.calculate());
#endif

    // 画面消去
    window.clear();

    // 描画用のシェーダプログラムの使用開始
    simple.use();
    simple.loadMatrix(window.getMp(), window.getMw());
    simple.setLight(light);
    simple.setMaterial(material);
	 
    // テクスチャ
#if GENERATE_POSITION
    glUniform1i(0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture[0]);

#endif
	//法線ベクトルの受け渡し
    glUniform1i(1, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture[0]);
	//色情報の受け渡し
	glUniform1i(2, 2);
    glActiveTexture(GL_TEXTURE2);
    sensor.getColor();

	//デプス情報の受け渡し
	glActiveTexture(GL_TEXTURE3);
	//交互に入ってる場所を参照して、計算した予測位置を渡す
	if (SwitchKalman) {
		glBindImageTexture(3, kalman.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		SwitchKalman--;
	}
	else {
		glBindImageTexture(3, kalman.tex_B, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		SwitchKalman++;
	}

#ifdef SOBEL_FILTER
	glActiveTexture(GL_TEXTURE4);
	glBindImageTexture(4, sobel.tex_A, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
#endif

    // 図形描画
    mesh.draw();

    // バッファを入れ替える
    window.swapBuffers();
  }
}
