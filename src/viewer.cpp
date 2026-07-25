#include "viewer.h"

#include "dorami_model.h"
#include "gl_util.h"

// ============================================================================
// 카메라 / 입력 상태
// ============================================================================

// 마우스 드래그 변환 제어 변수
static float g_rotX = 15.0f, g_rotY = 0.0f;
static int   g_prevMX = 0, g_prevMY = 0;
static bool  g_drag = false;

// 줌(카메라 거리) 제어 변수 - 'a' 줌인, 'z' 줌아웃
static const float g_camDistInit = 6.2f;   // 초기값은 gluLookAt 기본 z
static const float g_camDistMin = 2.5f;    // 너무 가까워져 피사체를 뚫고 들어가는 것 방지
static const float g_camDistMax = 15.0f;   // 너무 멀어져 피사체가 점처럼 작아지는 것 방지
static const float g_zoomStep = 0.4f;      // 키 입력 1회당 거리 증감 폭
static const float g_rotPerPixel = 0.5f;   // 마우스 1픽셀 이동당 회전 각도

static float g_camDist = g_camDistInit;    // 카메라-피사체 기준 거리

/** @brief 카메라 거리를 허용 범위 안으로 클램프하여 과도 확대·축소 방지 */
static void clampCamDist() {
    if (g_camDist < g_camDistMin) g_camDist = g_camDistMin;
    if (g_camDist > g_camDistMax) g_camDist = g_camDistMax;
}

/** @brief 회전 각도와 줌을 초기 구도로 되돌림 ('r' 키) */
static void resetView() {
    g_rotX = 15.0f;
    g_rotY = 0.0f;
    g_camDist = g_camDistInit;
}

// ============================================================================
// GLUT 콜백
// ============================================================================

/**
 * @brief 디스플레이 더블 버퍼 씬 클리어, 역방향 더블 조명 연산 및 파트별 렌더러 연속 구동 콜백
 */
static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0, -0.9, g_camDist, 0, -0.9, 0, 0, 1, 0); // 약간 위에서 도라미를 다소곳하게 내려다보는 전면 구도 카메라 설정(g_camDist로 줌 조절)

    // 명암 음영 가독성을 극대화하기 위한 대각선 크로스 역방향 입체 배치 방식
    GLfloat lp0[] = { 2.0f, 3.5f, 4.0f, 1.0f };  // 우측 전방 주광
    GLfloat lp1[] = { -2.0f, 1.0f,-2.0f, 1.0f }; // 좌측 후방 보조광
    glLightfv(GL_LIGHT0, GL_POSITION, lp0);
    glLightfv(GL_LIGHT1, GL_POSITION, lp1);

    // 마우스 마찰 이동 각도 행렬 변환식 누적 대입
    glRotatef(g_rotX, 1, 0, 0);
    glRotatef(g_rotY, 0, 1, 0);

    drawDorami();

    glutSwapBuffers(); // 백버퍼 전면 전환을 통한 버퍼 플리커 스크린 방지
}

/**
 * @brief 종횡비 붕괴 왜곡 보정용 원근 투영 변환 및 뷰포트 영역 매핑 함수
 */
static void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(38.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief ESC, R(앵글 리셋), A(줌인)/Z(줌아웃) 감지 제어 함수
 */
static void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 27: // ESC - 정리 코드까지 도달하도록 메인 루프를 정상 종료
        glutLeaveMainLoop();
        return;
    case 'r': case 'R':
        resetView();
        break;
    case 'a': case 'A': // 줌인: 카메라 거리 감소
        g_camDist -= g_zoomStep;
        clampCamDist();
        break;
    case 'z': case 'Z': // 줌아웃: 카메라 거리 증가
        g_camDist += g_zoomStep;
        clampCamDist();
        break;
    default:
        return;
    }
    glutPostRedisplay();
}

/**
 * @brief 마우스 왼쪽 버튼 클릭 이벤트를 캡처하여 드래그 핸들러 트리거 활성화 온/오프
 */
static void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        g_drag = (state == GLUT_DOWN);
        g_prevMX = x; g_prevMY = y;
    }
}

/**
 * @brief 실시간 스크린 드래그 가감 이동 변위 오프셋에 대응하여 가중치를 행렬 회전 각도에 합산
 */
static void motion(int x, int y) {
    if (!g_drag) return;

    g_rotY += (x - g_prevMX) * g_rotPerPixel;
    g_rotX += (y - g_prevMY) * g_rotPerPixel;
    g_prevMX = x; g_prevMY = y;
    glutPostRedisplay();
}

// ============================================================================
// 초기화
// ============================================================================

void viewerInitGLState() {
    glEnable(GL_DEPTH_TEST);       // 은면 제거 알고리즘 가동
    glEnable(GL_LIGHTING);         // 고로 드 셰이딩 시스템 가동
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);   // glColor3f 파이프라인 활성화 유지용 플래그 수립
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // 글로벌 주변광 음영 환경 벡터 상숫값 주입
    GLfloat amb[] = { 0.28f, 0.28f, 0.28f, 1.0f };
    GLfloat diff0[] = { 1.00f, 1.00f, 1.00f, 1.0f };
    GLfloat diff1[] = { 0.35f, 0.35f, 0.45f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    glClearColor(0.82f, 0.74f, 0.82f, 1.0f); // 배경화면 소프트 보라색 계열 지정
}

void viewerRegisterCallbacks() {
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
}
