#pragma once

// 카메라/조명 상태와 GLUT 이벤트 콜백 (마우스 회전, a/z 줌, r 리셋, ESC 종료)

/** @brief 깊이 테스트, 조명, 재질, 배경색 등 전역 렌더 상태 초기화 */
void viewerInitGLState();

/** @brief display/reshape/keyboard/mouse/motion 콜백을 GLUT에 등록 */
void viewerRegisterCallbacks();
