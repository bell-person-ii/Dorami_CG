#include "dorami_model.h"

#include "bmp_texture.h"
#include "gl_util.h"

#include <cmath>

// ============================================================================
// 모델 리소스
// ============================================================================

static GLuint g_faceTex = 0;

// ============================================================================
// 파트 색상 팔레트 (같은 색이 여러 파트에 재사용되므로 이름으로 고정)
// ============================================================================

static const Color kBodyYellow = { 0.97f, 0.84f, 0.10f }; // 머리/몸통/팔다리 바탕
static const Color kWhite = { 1.00f, 1.00f, 1.00f }; // 배, 손, 발, 꽃잎
static const Color kFaceWhite = { 0.99f, 0.98f, 0.96f }; // 텍스처 로드 실패 시 얼굴 대체색
static const Color kBowRed = { 0.82f, 0.08f, 0.10f }; // 리본 날개
static const Color kBowKnotRed = { 0.72f, 0.05f, 0.07f }; // 리본 매듭(날개보다 한 톤 어둡게)
static const Color kBellGold = { 0.90f, 0.76f, 0.12f }; // 목/가슴 방울
static const Color kCollarBlue = { 0.25f, 0.80f, 0.92f }; // 하늘색 목띠
static const Color kPatternRed = { 0.85f, 0.10f, 0.10f }; // 배 주머니 격자무늬
static const Color kTailRed = { 0.80f, 0.10f, 0.10f }; // 꼬리 중앙 방울
static const Color kOutlineBlack = { 0.00f, 0.00f, 0.00f }; // 주머니 외곽 마감선
static const Color kInkBlack = { 0.10f, 0.10f, 0.10f }; // 펜던트 분할선, 꼬리 기둥

// ============================================================================
// 머리
// ============================================================================

/**
 * @brief 비대칭 타원 구체 3개를 조립하여 머리 리본 조형
 */
static void drawBow() {
    glPushMatrix();
    glTranslatef(0.00f, 0.63f, -0.79f); // 머리 후방 상단 앵커 포인트로 이동
    glRotatef(180.0f, 0, 1, 0);
    glRotatef(-39.0f, 1, 0, 0);

    // 좌/우 리본 날개 (X축 부호만 반전된 대칭 구조)
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        col(kBowRed);
        glTranslatef(0.46f * side, 0.19f, 0.0f);
        glScalef(0.72f, 0.99f, 0.36f);
        glutSolidSphere(0.50f, 24, 16);
        glPopMatrix();
    }

    // 중앙 리본 매듭
    glPushMatrix();
    col(kBowKnotRed);
    glScalef(0.30f, 0.28f, 0.26f);
    glutSolidSphere(0.50f, 16, 12);
    glPopMatrix();

    glPopMatrix();
}

/**
 * @brief 정면 얼굴 타원에 BMP 텍스처를 오브젝트 선형 투영으로 매핑
 * @note  텍스처 생성 파이프라인과 ENV 모드를 사용 후 원상 복구해야
 *        이후 파트에 텍스처가 잘못 묻지 않는다.
 */
static void drawFaceTexturedSphere() {
    // 오브젝트 선형 공간(OBJECT_LINEAR) 기준 1:1 정면 텍스처 투영 투사 정의
    GLfloat sPlane[] = { 0.5f, 0.0f, 0.0f, 0.5f };
    GLfloat tPlane[] = { 0.0f, 0.5f, 0.0f, 0.5f };

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_faceTex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // 외부 음영 필터를 무시하고 텍스처 본래 색 강제 유지

    glutSolidSphere(1.0f, 64, 64);

    // 사용이 끝난 텍스처 생성 파이프라인 및 환경 레지스터 완전 초기화
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

/**
 * @brief 머리 본체, 얼굴 타원형 이미지 텍스처 맵핑 및 목걸이 파트
 */
void drawDoramiHead() {
    glPushMatrix();

    // [A] 노란색 머리 바탕 구체 (텍스처 영향 완벽 격리 차단)
    glDisable(GL_TEXTURE_2D);
    col(kBodyYellow);
    glutSolidSphere(1.0f, 48, 48);

    // [B] 정면 흰색 얼굴 타원 및 매핑 렌더링
    glPushMatrix();
    glTranslatef(0.0f, 0.04f, 0.37f);
    glScalef(0.897f, 0.877f, 0.64f);

    if (g_faceTex) {
        drawFaceTexturedSphere();
    }
    else {
        col(kFaceWhite);
        glutSolidSphere(1.0f, 32, 32);
    }
    glPopMatrix();

    // [C] 소품 조립
    drawBow();

    // 하늘색 목띠
    col(kCollarBlue);
    glPushMatrix();
    glTranslatef(0.0f, -0.76f, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);
    gluCylinder(sharedQuadric(), 0.70f, 0.70f, 0.14f, 32, 2);
    glPopMatrix();

    // 목걸이 방울
    glPushMatrix();
    col(kBellGold);
    glTranslatef(0.0f, -0.86f, 0.62f);
    glScalef(1.0f, 1.1f, 0.85f);
    glutSolidSphere(0.08f, 14, 14);
    glPopMatrix();

    glPopMatrix();
}

// ============================================================================
// 상체
// ============================================================================

/**
 * @brief 하얀 배 구체 생성 및 삼각함수 구면 좌표계를 이용한 불균일 체크무늬 사선 추적 함수
 */
static void drawDoramiPocketCombined() {
    const float whiteBallRadius = 0.55f;
    const int lon_div = 40;
    const int lat_div = 40;

    // [A] 바탕 하얀 복부 구체
    glPushMatrix();
    col(kWhite);
    glTranslatef(0.0f, -0.05f, 0.29f); // 몸통 앞쪽으로 전진시켜 원화 입체감 강조
    glutSolidSphere(whiteBallRadius, 40, 40);
    glPopMatrix();

    // [B] 내부 격자무늬 드로잉 (그늘짐 방지를 위한 조명 오프 연산 적용)
    ScopedLightingOff lightingOff;
    glLineWidth(2.5f);

    const float lineRadius = whiteBallRadius * 1.008f; // 배 표면에 칼같이 피팅되도록 미세 마진을 둔 3D 곡률 반지름
    const float pocketYTop = -0.05f;                   // 주머니 가로 테두리선 마감 높이
    const float maxLon = M_PI * 0.19f;                 // 반원 주머니의 가로 폭 진폭 제한선

    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.29f);

    col(kPatternRed);

    const float topLat = asin(pocketYTop / lineRadius);
    const float bottomLat = asin(-0.55f / lineRadius); // 사선이 아랫배 바닥 종단까지 완전 도달하게 하는 한계점

    // 원화 고유의 "두 줄씩 짝을 짓는 불균일 이중 격자 패턴" 유도를 위한 주기적 오프셋 가중치 인덱스
    const int lineIndices[] = { -6, -5,  -1, 0,  4, 5 };
    const int numPairs = sizeof(lineIndices) / sizeof(lineIndices[0]);

    // slashDir = +1 이면 슬래시( / ), -1 이면 역슬래시( \ ) 방향 무늬
    for (int slashDir = 1; slashDir >= -1; slashDir -= 2) {
        for (int k = 0; k < numPairs; k++) {
            float baseOffset = (float)lineIndices[k];
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i <= lat_div; ++i) {
                float lat = topLat + (bottomLat - topLat) * (float)i / lat_div;
                // 구면 기하학 기반 X축 사선 궤적 이동식 (부호 반전으로 대칭 교차)
                float lon = (baseOffset / 5.5f) * maxLon + slashDir * (topLat - lat) * 1.2f;
                float boundLat = topLat - (M_PI * 0.23f) * sqrt(1.0f - (lon / maxLon) * (lon / maxLon)); // 반원 경계 트래킹 역산식

                if (lon >= -maxLon && lon <= maxLon && lat >= boundLat && lat <= topLat) {
                    glVertex3f(lineRadius * cos(lat) * sin(lon), lineRadius * sin(lat), lineRadius * cos(lat) * cos(lon));
                }
            }
            glEnd();
        }
    }

    // [C] 주머니 가로 및 반원 외곽 검은 테두리 마감선
    col(kOutlineBlack);
    glLineWidth(3.0f);

    // 상단 가로선 마감
    glBegin(GL_LINE_STRIP);
    for (float lon = -maxLon; lon <= maxLon; lon += 0.02f) {
        glVertex3f(lineRadius * cos(asin(pocketYTop / lineRadius)) * sin(lon), pocketYTop, lineRadius * cos(asin(pocketYTop / lineRadius)) * cos(lon));
    }
    glEnd();

    // 반원 외곽 곡선 마감
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j <= lon_div; ++j) {
        float lon = -maxLon + (2.0f * maxLon) * (float)j / lon_div;
        float currentLat = asin(pocketYTop / lineRadius) - (M_PI * 0.23f) * sqrt(1.0f - (lon / maxLon) * (lon / maxLon));
        float y = lineRadius * sin(currentLat);
        if (y <= pocketYTop) {
            glVertex3f(lineRadius * cos(currentLat) * sin(lon), y, lineRadius * cos(currentLat) * cos(lon));
        }
    }
    glEnd();

    glPopMatrix();
}

/**
 * @brief 옆구리를 따라 아래로 떨어지는 팔 한쪽과 끝단의 하얀 왕손
 * @param side -1 = 왼팔, +1 = 오른팔 (좌우 완벽 대칭 매핑)
 */
static void drawDoramiArm(float side) {
    glPushMatrix();
    // 어깨 단면 탈골을 막기 위해 시작 좌표를 몸통 내부계 구체 중심 라인으로 조절
    glTranslatef(0.48f * side, -0.15f, 0.05f);

    // 기본 Z축 정면을 바라보는 실린더 축을 -> 옆구리(Y축 회전)로 돌린 후 -> 아래(X축 회전)로 꺾는 2단 구조
    glRotatef(90.0f * side, 0.0f, 1.0f, 0.0f);
    glRotatef(35.0f, 1.0f, 0.0f, 0.0f);

    col(kBodyYellow);
    gluCylinder(sharedQuadric(), 0.185, 0.145, 0.65, 24, 10); // 신장 비율 포션을 키워 길이를 0.65로 연장

    // 하얀색 왕손 (연장된 팔 끝단인 로컬 Z축 0.65 오프셋 자리에 누적 마운트)
    glTranslatef(0.0f, 0.0f, 0.65f);
    col(kWhite);
    glutSolidSphere(0.20f, 20, 20); // 반지름 0.20f로 볼륨을 키워 원화 싱크로율 보정
    glPopMatrix();
}

/**
 * @brief 가슴 몸통 조립 및 옆구리를 따라 아래로 떨어지는 양팔과 확대된 하얀 동글손 구현 함수
 */
void drawDoramiUpperBody() {
    glPushMatrix();
    glTranslatef(0.0f, -0.76f, 0.0f); // 상체 영역 전체 원점을 목띠 아래로 격리 고정

    // [A] 노란 몸통 구체 및 주머니 연동
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, 0.0f);
    col(kBodyYellow);
    glPushMatrix();
    glScalef(1.02f, 1.02f, 1.02f);
    glutSolidSphere(0.72f, 40, 40);
    glPopMatrix();

    glPushMatrix();
    drawDoramiPocketCombined();
    glPopMatrix();
    glPopMatrix();

    // [B] 가슴 펜던트 방울 및 센터 횡단 분할 라인
    glPushMatrix();
    col(kBellGold);
    glTranslatef(0.0f, -0.06f, 0.68f);
    glScalef(1.0f, 1.1f, 0.85f);
    glutSolidSphere(0.08f, 16, 16);

    {
        ScopedLightingOff lightingOff;
        col(kInkBlack);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(-0.08f, 0.0f, 0.01f); glVertex3f(0.08f, 0.0f, 0.01f);
        glEnd();
    }
    glPopMatrix();

    // [C] 좌우 팔 및 왕손
    drawDoramiArm(-1.0f);
    drawDoramiArm(+1.0f);

    glPopMatrix();
}

// ============================================================================
// 하체
// ============================================================================

/**
 * @brief 엉덩이 후방 검은 원통 기둥 + 5엽 꽃잎 + 중앙 빨간 방울로 구성된 꼬리
 */
static void drawDoramiTail() {
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.0f);
    glRotatef(180, 0.0, 1.0, 0.0);

    // 검은색 원통형 꼬리 연결선 대대적 전개
    glPushMatrix();
    col(kInkBlack);
    glTranslatef(0.0f, -0.05f, 0.0f);
    gluCylinder(sharedQuadric(), 0.012, 0.012, 0.32, 12, 5);
    glPopMatrix();

    // 꼬리 실린더 종단점(Z=0.32) 자리에 흰색 꽃잎 5엽 루프 회전 전개 및 중앙 빨간 방울 꼬리 고정
    glTranslatef(0.0f, -0.05f, 0.32f);
    col(kWhite);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glRotatef(i * 72.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, 0.045f, 0.0f);
        glScalef(1.0f, 1.0f, 0.3f); // 납작한 단면 꽃잎 형성을 위한 압축 변환
        glutSolidSphere(0.025, 15, 15);
        glPopMatrix();
    }
    col(kTailRed);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.015f);
    glutSolidSphere(0.035, 20, 20);
    glPopMatrix();
    glPopMatrix();
}

/**
 * @brief 다리 실린더 한 짝과 다리 정중앙선에 맞춰 정렬된 납작한 발
 * @param side   -1 = 왼쪽, +1 = 오른쪽 (X축 좌표 부호)
 * @param yawDeg 다리 기둥의 Y축 비틀림 각도 (좌우가 서로 다른 손보정 값)
 * @param stacks gluCylinder 스택 분할 수
 */
static void drawDoramiLeg(float side, float yawDeg, int stacks) {
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.0f);

    // 다리 기둥
    col(kBodyYellow);
    glPushMatrix();
    glTranslatef(0.09f * side, -0.05f, 0.0f);
    glRotatef(83, 1.0, 0.0, 0.0);
    glRotatef(yawDeg, 0.0, 1.0, 0.0);
    gluCylinder(sharedQuadric(), 0.091, 0.072, 0.30, 30, stacks);
    glPopMatrix();

    // 발 타원 구체 (다리 뼈대 관절 정중앙 수렴 일치화 보정 완료)
    col(kWhite);
    glPushMatrix();
    glTranslatef(0.115f * side, -0.35f, 0.04f); // 발이 안쪽으로 쏠리던 탈조 현상 해결값 적용
    glScalef(1.1f, 0.5f, 1.2f);                 // 땅바닥에 납작한 원화 형태로 납작화 압축 변환
    glutSolidSphere(0.08, 30, 30);
    glPopMatrix();
    glPopMatrix();
}

/**
 * @brief 골반 구체, 꽃 피는 5엽 꼬리 디자인 및 쏠림이 보정된 다리와 발 결합 함수
 */
void drawDoramiLowerBody() {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0.0f, -1.78f, 0.0f);
    glScalef(2.6f, 2.6f, 2.6f); // 상체 포션에 핏하도록 전체 하체 등신대 등급 비율 스케일링

    // 1. 골반 지지 구체
    glPushMatrix();
    col(kBodyYellow);
    glTranslatef(0.0, 0.18, 0.0);
    glRotatef(270.0, 1.0, 0.0, 0.0);
    gluSphere(sharedQuadric(), 0.209, 30, 20);
    glPopMatrix();

    // 2. 꼬리 조립
    drawDoramiTail();

    // 3~4. 좌/우 다리와 발 (기둥 비틀림 각도만 좌우가 다르다)
    drawDoramiLeg(-1.0f, 355.0f, 10);
    drawDoramiLeg(+1.0f, 3.0f, 30);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

// ============================================================================
// 리소스 및 전체 렌더링
// ============================================================================

void doramiLoadAssets(const char* faceTexturePath) {
    g_faceTex = loadBMP(faceTexturePath);
}

void doramiReleaseAssets() {
    if (!g_faceTex) return;
    glDeleteTextures(1, &g_faceTex);
    g_faceTex = 0;
}

void drawDorami() {
    drawDoramiHead();
    drawDoramiUpperBody();
    drawDoramiLowerBody();
}
