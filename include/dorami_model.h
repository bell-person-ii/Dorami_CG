#pragma once

// 도라미 캐릭터 모델의 리소스 로딩과 파트별 렌더링

/**
 * @brief 모델이 사용하는 리소스(얼굴 텍스처)를 로드
 * @param faceTexturePath 24-bit BMP 얼굴 텍스처 경로. 로드 실패 시 흰색 얼굴로 대체된다.
 */
void doramiLoadAssets(const char* faceTexturePath);

/** @brief 로드한 모델 리소스 해제 */
void doramiReleaseAssets();

// 파트별 렌더러 (머리 -> 상체 -> 하체 순서로 호출한다)
void drawDoramiHead();
void drawDoramiUpperBody();
void drawDoramiLowerBody();

/** @brief 세 파트를 순서대로 그려 캐릭터 전체를 렌더링 */
void drawDorami();
