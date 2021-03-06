#include "KinectV1.h"

//
// 深度センサ関連の処理
//

// 標準ライブラリ
#include <cassert>

// Kinect 関連
#pragma comment(lib, "Kinect10.lib")

// 計測不能点のデフォルト距離
const GLfloat maxDepth(8.0f);

// コンストラクタ
KinectV1::KinectV1()
  : DepthCamera(DEPTH_W, DEPTH_H, COLOR_W, COLOR_H)
  , nextColorFrameEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
  , nextDepthFrameEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
{
  HRESULT hr(S_OK);

  // 最初のインスタンスを生成するときだけ
  if (getActivated() == 0)
  {
    // 接続されているセンサの数を調べる
    hr = NuiGetSensorCount(&connected);
  }

  // センサが接続されており使用台数が接続台数に達していなければ
  if (hr == S_OK && getActivated() < connected)
  {
    // センサの使用を開始する
    hr = NuiCreateSensorByIndex(getActivated(), &sensor);

    // センサが使用可能であれば
    if (hr == S_OK && sensor->NuiStatus() == S_OK)
    {
      // センサを初期化する (カラーとデプスを取得する)
      hr = sensor->NuiInitialize(
        NUI_INITIALIZE_FLAG_USES_COLOR |
        NUI_INITIALIZE_FLAG_USES_DEPTH |
        0);
      assert(hr == S_OK);

      // センサの仰角を初期化する
      hr = sensor->NuiCameraElevationSetAngle(0L);
      assert(hr == S_OK);

      // デプスストリームの取得設定
      hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, DEPTH_RESOLUTION,
        0, 2, nextDepthFrameEvent, &depthStream);
      assert(hr == S_OK);

      // カラーストリームの取得設定
      hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, COLOR_RESOLUTION,
        0, 2, nextColorFrameEvent, &colorStream);
      assert(hr == S_OK);

      // depthCount と colorCount を計算してテクスチャとバッファオブジェクトを作成する
      makeTexture();

      // デプスデータからカメラ座標を求めるときに用いる一時メモリを確保する
      position = new GLfloat[depthCount][3];
    }
  }
}

// デストラクタ
KinectV1::~KinectV1()
{
  // イベントハンドルを閉じる
  CloseHandle(nextDepthFrameEvent);
  CloseHandle(nextColorFrameEvent);

  // センサが有効になっていたら
  if (getActivated() > 0)
  {
    // データ変換用のメモリを削除する
    delete[] position;

    // センサをシャットダウンする
    sensor->NuiShutdown();
  }
}

// データを取得する
void KinectV1::getImage(HANDLE event, HANDLE stream,
  GLuint texture, GLsizei width, GLsizei height, GLenum format, GLenum type) const
{
  // カラーのテクスチャを指定する
  glBindTexture(GL_TEXTURE_2D, texture);

  // 次のフレームデータが到着していれば
  if (WaitForSingleObject(event, 0) != WAIT_TIMEOUT)
  {
    // 取得したフレームデータの格納場所
    NUI_IMAGE_FRAME frame;

    // フレームデータの格納場所を frame に取得する
    if (sensor->NuiImageStreamGetNextFrame(stream, 0, &frame) == S_OK)
    {
      // これから処理完了までデータが変更されないようにロックする
      NUI_LOCKED_RECT rect;
      frame.pFrameTexture->LockRect(0, &rect, NULL, 0);

      // ロックに成功したら
      if (rect.Pitch)
      {
        // 取得したのがデプスデータかカラーデータであれば
        if (texture == depthTexture || texture == colorTexture)
        {
          // pBits に入っているデータをテクスチャに転送する
          glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, rect.pBits);
        }

        // デプスデータかカメラ座標を取得するときは
        if (texture == depthTexture || texture == pointTexture)
        {
          // カラーデータのテクスチャ座標を求めてバッファオブジェクトに転送する
          glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
          GLfloat (*const texcoord)[2](static_cast<GLfloat (*)[2]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
          for (int i = 0; i < depthCount; ++i)
          {
            // デプスの画素位置からカラーの画素位置を求める
            LONG x, y;
            sensor->NuiImageGetColorPixelCoordinatesFromDepthPixel(COLOR_RESOLUTION,
              NULL, i % depthWidth, i / depthWidth, reinterpret_cast<USHORT *>(rect.pBits)[i], &x, &y);

            // カラーデータのテクスチャ座標に変換する
            texcoord[i][0] = GLfloat(x) + 0.5f;
            texcoord[i][1] = GLfloat(y) + 0.5f;
          }
          glUnmapBuffer(GL_ARRAY_BUFFER);

          // カメラ座標を取得するときは
          if (texture == pointTexture)
          {
            // すべての点について
            for (int i = 0; i < depthCount; ++i)
            {
              // デプス値の単位をメートルに換算する係数
              static const GLfloat zScale(-0.001f / GLfloat(1 << NUI_IMAGE_PLAYER_INDEX_SHIFT));

              // その点のデプス値を得る
              const unsigned short d(reinterpret_cast<USHORT *>(rect.pBits)[i]);

              // デプス値の単位をメートルに換算する (計測不能点は maxDepth にする)
              const GLfloat z(d == 0 ? -maxDepth : GLfloat(d) * zScale);

              // レンズの画角にもとづくスケールを求める
              const GLfloat s(NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS * z);

              // その点のスクリーン上の位置を求める
              const GLfloat x(float(i % depthWidth - depthWidth / 2) + 0.5f);
              const GLfloat y(float(i / depthWidth - depthHeight / 2) + 0.5f);

              // その点のカメラ座標を求める
              position[i][0] = x * s;
              position[i][1] = y * s;
              position[i][2] = z;
            }

            // カメラ座標をテクスチャに転送する
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RGB, GL_FLOAT, position);
          }
        }

        // ロックしたデータを開放する
        const HRESULT hr(sensor->NuiImageStreamReleaseFrame(stream, &frame));
        assert(hr == S_OK);
      }
    }
  }
}
