#include "KinectV2.h"

//
// 深度センサ関連の処理
//

// 標準ライブラリ
#include <cassert>
#include <fstream>
#include <iostream>
#include "glm/glm.hpp"

// Kinect 関連
#pragma comment(lib, "Kinect20.lib")

// 計測不能点のデフォルト距離
const GLfloat maxDepth(10.0f);

#define filename "result2.txt"
#define POINT_POS 10000
const float variance = 0.01;

// コンストラクタ
KinectV2::KinectV2()
{
	// センサを取得する
	if (sensor == NULL && GetDefaultKinectSensor(&sensor) == S_OK)
	{
		HRESULT hr;

		counter = 0;
		// センサの使用を開始する
		hr = sensor->Open();
		assert(hr == S_OK);

		// デプスデータの読み込み設定
		IDepthFrameSource *depthSource;
		hr = sensor->get_DepthFrameSource(&depthSource);
		assert(hr == S_OK);
		hr = depthSource->OpenReader(&depthReader);
		assert(hr == S_OK);
		IFrameDescription *depthDescription;
		hr = depthSource->get_FrameDescription(&depthDescription);
		assert(hr == S_OK);
		depthSource->Release();

		// デプスデータのサイズを得る
		depthDescription->get_Width(&depthWidth);
		depthDescription->get_Height(&depthHeight);
		depthDescription->Release();

		// カラーデータの読み込み設定
		IColorFrameSource *colorSource;
		hr = sensor->get_ColorFrameSource(&colorSource);
		assert(hr == S_OK);
		hr = colorSource->OpenReader(&colorReader);
		assert(hr == S_OK);
		IFrameDescription *colorDescription;
		hr = colorSource->get_FrameDescription(&colorDescription);
		assert(hr == S_OK);
		colorSource->Release();

		// カラーデータのサイズを得る
		colorDescription->get_Width(&colorWidth);
		colorDescription->get_Height(&colorHeight);
		colorDescription->Release();

		// 座標のマッピング
		hr = sensor->get_CoordinateMapper(&coordinateMapper);
		assert(hr == S_OK);

		// depthCount と colorCount を計算してテクスチャとバッファオブジェクトを作成する
		makeTexture();

		// デプスデータからカメラ座標を求めるときに用いる一時メモリを確保する
		position = new GLfloat[depthCount][3];

		// カラーデータを変換する用いる一時メモリを確保する
		color = new GLubyte[colorCount * 4];
	}
}

// デストラクタ
KinectV2::~KinectV2()
{
	if (getActivated() > 0)
	{
		// データ変換用のメモリを削除する
		delete[] position;
		delete[] color;

		// センサを開放する
		colorReader->Release();
		depthReader->Release();
		coordinateMapper->Release();
		sensor->Close();
		sensor->Release();

		// センサを開放したことを記録する
		sensor = NULL;
	}
}


// 重み付き画素値の合計と重みの合計を求める
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

// デプスデータを取得する
GLuint KinectV2::getDepth()
{

	//書き込み用ファイル
	const char *fileName = filename;
	std::ofstream ofs(fileName, std::ios::app);
	if (!ofs) {
		std::cout << "cant open '" << fileName << "' ." << std::endl;
		std::cin.get();
		return 0;
	}

	// デプスのテクスチャを指定する
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	// 次のデプスのフレームデータが到着していれば
	IDepthFrame *depthFrame;
	if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
	{
		// デプスデータのサイズと格納場所を得る
		UINT depthSize;
		UINT16 *depthBuffer;
		UINT16 depthDSPBuffer;
		depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

		// カラーのテクスチャ座標を求めてバッファオブジェクトに転送する
		glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
		ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
		coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		IColorFrame *colorFrame;

		if (colorReader->AcquireLatestFrame(&colorFrame) == S_OK)
		{
			
			// カラーデータを取得して RGBA 形式に変換する
			colorFrame->CopyConvertedFrameDataToArray(colorCount * 4,
				static_cast<BYTE *>(color), ColorImageFormat::ColorImageFormat_Bgra);

			// カラーフレームを開放する
			colorFrame->Release();
			//なぜかカラーがトレない

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

		// デプスデータをテクスチャに転送する
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RED, GL_UNSIGNED_SHORT, depthBuffer);

		// デプスフレームを開放する
		depthFrame->Release();
		
	}

	ofs.close();

	return depthTexture;
}
/*
GLuint KinectV2::takeOneDepth() const {

// 次のデプスのフレームデータが到着していれば
IDepthFrame *depthFrame;
if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
{
// デプスデータのサイズと格納場所を得る
UINT depthSize;
UINT16 *depthBuffer;
depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

return depthBuffer[200];
}


};*/


// カメラ座標を取得する
GLuint KinectV2::getPoint() const
{
	// カメラ座標のテクスチャを指定する
	glBindTexture(GL_TEXTURE_2D, pointTexture);

	// 次のデプスのフレームデータが到着していれば
	IDepthFrame *depthFrame;
	if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
	{
		// デプスデータのサイズと格納場所を得る
		UINT depthSize;
		UINT16 *depthBuffer;
		depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

		// カラーのテクスチャ座標を求めてバッファオブジェクトに転送する
		glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
		ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
		coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// カメラ座標への変換テーブルを得る
		UINT32 entry;
		PointF *table;
		coordinateMapper->GetDepthFrameToCameraSpaceTable(&entry, &table);

		// すべての点について
		for (unsigned int i = 0; i < entry; ++i)
		{
			// デプス値の単位をメートルに換算する係数
			static const GLfloat zScale(-0.001f);

			// その点のデプス値を得る
			const unsigned short d(depthBuffer[i]);

			// デプス値の単位をメートルに換算する (計測不能点は maxDepth にする)
			const GLfloat z(d == 0 ? -maxDepth : GLfloat(d) * zScale);

			// その点のスクリーン上の位置を求める
			const GLfloat x(table[i].X);
			const GLfloat y(-table[i].Y);

			// その点のカメラ座標を求める
			position[i][0] = x * z;
			position[i][1] = y * z;
			position[i][2] = z;
		}

		// カメラ座標を転送する
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RGB, GL_FLOAT, position);

		// テーブルを開放する
		CoTaskMemFree(table);

		// デプスフレームを開放する
		depthFrame->Release();
	}

	return pointTexture;
}

// カラーデータを取得する
GLuint KinectV2::getColor() const
{
	// カラーのテクスチャを指定する
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// 次のカラーとデプスのフレームデータが到着していれば
	//IDepthFrame *depthFrame;
	IColorFrame *colorFrame;
	if (colorReader->AcquireLatestFrame(&colorFrame) == S_OK/* && depthReader->AcquireLatestFrame(&depthFrame) == S_OK*/)
	{
		// カラーデータを取得して RGBA 形式に変換する
		colorFrame->CopyConvertedFrameDataToArray(colorCount * 4,
			static_cast<BYTE *>(color), ColorImageFormat::ColorImageFormat_Bgra);

		// カラーフレームを開放する
		colorFrame->Release();

		// カラーデータをテクスチャに転送する
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorWidth, colorHeight, GL_BGRA, GL_UNSIGNED_BYTE, color);
	}

	return colorTexture;
}

// センサの識別子
IKinectSensor *KinectV2::sensor(NULL);

