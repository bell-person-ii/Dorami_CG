#include "gl_util.h"

// 모든 gluCylinder/gluSphere 호출이 재사용하는 단일 quadric 객체
static GLUquadric* g_quadObj = nullptr;

void col(float r, float g, float b) {
    glColor3f(r, g, b);
    GLfloat m[] = { r, g, b, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, m);
}

void col(const Color& c) {
    col(c.r, c.g, c.b);
}

void createSharedQuadric() {
    if (g_quadObj) return;
    g_quadObj = gluNewQuadric();
    gluQuadricNormals(g_quadObj, GLU_SMOOTH);
}

void destroySharedQuadric() {
    if (!g_quadObj) return;
    gluDeleteQuadric(g_quadObj); // 동적 할당 자원 쿼드릭 객체 해제 반환
    g_quadObj = nullptr;
}

GLUquadric* sharedQuadric() {
    return g_quadObj;
}
