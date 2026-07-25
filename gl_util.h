#pragma once

// 렌더링 파트 전반에서 공유하는 OpenGL 보조 도구 모음
// (색상/재질 동기화, 공용 quadric 자원, 조명 임시 해제 스코프)

#include <freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** @brief 파트 색상을 이름으로 다루기 위한 단순 RGB 묶음 */
struct Color {
    float r, g, b;
};

/**
 * @brief 단색 그리기(glColor)와 광원 재질(Material) 상태를 동시 동기화
 * @note  raw glColor3f 대신 항상 이 함수를 사용해야 조명 아래에서 색이 올바르게 나온다.
 */
void col(float r, float g, float b);
void col(const Color& c);

// ---- 프로그램 전체가 공유하는 GLU quadric 자원 --------------------------------

/** @brief 공용 quadric 생성 (main 초기화 시 1회) */
void createSharedQuadric();
/** @brief 공용 quadric 해제 (종료 시 1회) */
void destroySharedQuadric();
/** @brief gluCylinder/gluSphere 호출에 넘길 공용 quadric 핸들 */
GLUquadric* sharedQuadric();

/**
 * @brief 평면 라인 아트가 음영에 먹히지 않도록 조명을 잠시 끄는 스코프 가드
 * @note  생성 시 glDisable(GL_LIGHTING), 스코프 종료 시 자동 복구되어
 *        enable/disable 짝이 어긋날 여지를 없앤다.
 */
class ScopedLightingOff {
public:
    ScopedLightingOff() { glDisable(GL_LIGHTING); }
    ~ScopedLightingOff() { glEnable(GL_LIGHTING); }
    ScopedLightingOff(const ScopedLightingOff&) = delete;
    ScopedLightingOff& operator=(const ScopedLightingOff&) = delete;
};
