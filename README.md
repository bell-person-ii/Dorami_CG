# Dorami 3D Model Viewer

OpenGL 고정 기능 파이프라인(immediate mode)과 GLU/GLUT 기본 도형만으로
캐릭터 **도라미**를 모델링한 컴퓨터그래픽스 과제 프로그램입니다.

구(球), 원기둥, 원환 등 기본 프리미티브를 계층적 변환(`glPushMatrix`/`glPopMatrix`)으로
조합해 머리·상체·하체를 구성하고, 조명·재질·텍스처 매핑을 적용했습니다.

> 프로젝트 이름이 `MetaHuman_CG`이지만 Unreal MetaHuman과는 무관합니다.

## 주요 구현 내용

- **계층적 모델링** — 신체 부위별 렌더러가 각자의 변환 행렬을 스코프로 관리
- **조명** — 우측 전방 주광 + 좌측 후방 보조광의 2점 대각 배치로 입체감 강조
- **재질 동기화** — `glColor3f`와 `GL_AMBIENT_AND_DIFFUSE` 재질을 함께 설정하는 `col()` 헬퍼
- **텍스처 매핑** — 얼굴 표정을 24-bit BMP로 로드해 `GL_OBJECT_LINEAR` 자동 좌표 생성으로 머리 구에 투영
- **절차적 패턴** — 배 부분의 주머니/체크 무늬를 구면 좌표계에서 계산해 생성
- **인터랙션** — 마우스 드래그 회전, 키보드 줌 및 시점 초기화

## 빌드 및 실행

Visual Studio 2022 (`v143` 툴셋) 프로젝트입니다. CMake는 사용하지 않습니다.

1. `MetaHuman_CG.sln`을 Visual Studio 2022로 엽니다.
2. 구성을 **Debug | x64** 로 설정합니다.
3. `Ctrl+Shift+B` 로 빌드, `F5` 로 실행합니다.

명령줄에서 빌드하려면:

```bash
msbuild MetaHuman_CG.sln /p:Configuration=Debug /p:Platform=x64
```

### 사전 준비: freeglut

`.vcxproj`에 freeglut 경로가 하드코딩되어 있습니다.

| 항목 | 경로 |
| --- | --- |
| 포함 디렉터리 | `C:\freeglut\include\GL` |
| 라이브러리 디렉터리 | `C:\freeglut\lib\x64` |
| 링크 대상 | `freeglut.lib` |

- freeglut을 `C:\freeglut` 에 설치하거나, `.vcxproj`의 `IncludePath` / `LibraryPath`를 본인 환경에 맞게 수정하세요.
- 실행 시 `freeglut.dll` 이 PATH에 있거나 실행 파일과 같은 폴더에 있어야 합니다.
- Win32 구성도 x64 라이브러리 경로를 가리키므로 **x64 빌드만 정상 링크**됩니다.

### 작업 디렉터리 주의

얼굴 텍스처를 `assets/face_texture.bmp` 상대 경로로 읽습니다. 프로젝트 루트를
작업 디렉터리로 두고 실행하지 않으면 텍스처가 적용되지 않고 흰색으로 표시됩니다.

## 조작법

| 입력 | 동작 |
| --- | --- |
| 마우스 왼쪽 드래그 | 모델 회전 |
| `A` | 줌 인 |
| `Z` | 줌 아웃 |
| `R` | 회전·줌 초기화 |
| `Esc` | 종료 |

줌은 카메라 거리 `2.5 ~ 15.0` 범위로 제한됩니다.

## 프로젝트 구조

```
Dorami_CG/
├─ MetaHuman_CG.sln            솔루션
├─ MetaHuman_CG.vcxproj        프로젝트 설정 (freeglut 경로 포함)
├─ assets/
│  └─ face_texture.bmp         얼굴 텍스처 (24-bit 비압축 BMP)
├─ include/                    헤더 모음 (프로젝트 IncludePath에 등록됨)
│  ├─ viewer.h
│  ├─ dorami_model.h
│  ├─ gl_util.h
│  └─ bmp_texture.h
└─ src/
   ├─ main.cpp
   ├─ viewer.cpp
   ├─ dorami_model.cpp
   ├─ gl_util.cpp
   └─ bmp_texture.cpp
```

| 파일 | 역할 |
| --- | --- |
| `src/main.cpp` | 진입점 — GLUT 초기화, 모듈 연결, 메인 루프, 리소스 정리 |
| `include/viewer.h` + `src/viewer.cpp` | 카메라·입력 상태, 전역 GL/조명 설정, GLUT 콜백 전체 |
| `include/dorami_model.h` + `src/dorami_model.cpp` | 캐릭터 팔레트 상수와 모든 파트 렌더러 |
| `include/gl_util.h` + `src/gl_util.cpp` | 공용 헬퍼 — `col()`, 공유 quadric, `ScopedLightingOff` |
| `include/bmp_texture.h` + `src/bmp_texture.cpp` | `loadBMP()` — 24-bit 비압축 BMP 전용 로더 |

렌더링은 신체 부위 기준으로 하향식 구성됩니다.

```
display()                      카메라·조명 설정 후 회전 적용
└─ drawDorami()
   ├─ drawDoramiHead()         머리 구 + 얼굴 텍스처, 리본, 목띠, 방울
   ├─ drawDoramiUpperBody()    몸통, 가슴 방울, 양팔
   │  └─ drawDoramiPocketCombined()   배 부분 구와 주머니 무늬
   └─ drawDoramiLowerBody()    골반, 꼬리, 양다리
```

## 개발 메모

- 모든 소스는 **UTF-8 with BOM** 으로 저장되어 있습니다. 한국어 주석이 MSVC에서 CP949로 잘못 해석되는 것을 막기 위한 것이므로, 파일을 추가할 때도 BOM을 유지하세요.
- 색상은 `glColor3f` 대신 `col()` 을 사용해야 조명 아래에서 올바르게 음영 처리됩니다.
- 배 무늬처럼 음영 없이 그려야 하는 평면 요소는 `ScopedLightingOff` 가드를 사용합니다.
- 위치·크기 값은 대부분 눈으로 맞춘 상수입니다. 파트를 옮길 때는 부모 변환이 아니라 해당 파트의 `glPushMatrix` 블록 안 값을 수정하세요.
- `.cpp`/`.h` 를 추가하면 `MetaHuman_CG.vcxproj` 와 `.vcxproj.filters` 양쪽에 모두 등록해야 합니다.
