// 도라미 3D 모델 뷰어 - 프로그램 진입점
//
// 모듈 구성
//   gl_util      : 색상/재질 동기화, 공용 quadric, 조명 스코프 가드
//   bmp_texture  : 24-bit 비압축 BMP 텍스처 로더
//   dorami_model : 캐릭터 파트별 렌더링(머리 / 상체 / 하체)
//   viewer       : 카메라·조명 상태와 GLUT 이벤트 콜백

#include "dorami_model.h"
#include "gl_util.h"
#include "viewer.h"

// 실행 작업 디렉터리 기준 상대 경로 (프로젝트 루트에서 실행해야 얼굴 텍스처가 적용된다)
static const char* kFaceTexturePath = "assets/face_texture.bmp";

/** @brief 창이 닫히는 시점(GL 컨텍스트 유효) 에 GPU 리소스 반환 */
static void onWindowClose() {
    doramiReleaseAssets();
}

/**
 * @brief 메인 초기화 셋업, 전역 광원 환경 설정 및 이벤트 디스패처 무한 루프 가동 엔트리
 */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Dorami 3D OpenGL Project Final Master");

    // 창이 닫혀도 glutMainLoop가 반환되어 아래 정리 코드까지 도달하게 한다(freeglut 확장)
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    viewerInitGLState();

    createSharedQuadric();
    doramiLoadAssets(kFaceTexturePath);

    viewerRegisterCallbacks();
    glutCloseFunc(onWindowClose);

    glutMainLoop();

    destroySharedQuadric();
    return 0;
}
