#pragma once

// 24-bit 비압축 BMP 전용 텍스처 로더

#include <freeglut.h>

/**
 * @brief 24-bit 비압축 BMP 이미지를 파싱하여 2D 텍스처 객체로 업로드
 * @param path 실행 작업 디렉터리 기준 상대/절대 경로
 * @return 생성된 텍스처 ID, 실패 시 0
 * @note   24-bit 비압축 BMP만 해석하며 BGR->RGB 순서를 직접 뒤집는다.
 *         교체 텍스처도 반드시 같은 포맷이어야 한다.
 */
GLuint loadBMP(const char* path);
