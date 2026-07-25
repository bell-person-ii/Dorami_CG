#define _CRT_SECURE_NO_WARNINGS // Visual Studio 입출력 함수 보안 경고 방지

#include "bmp_texture.h"

#include <cstdio>
#include <cstdlib>

GLuint loadBMP(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) { fprintf(stderr, "loadBMP: 파일 열기 실패 -> %s\n", path); return 0; }

    unsigned char hdr[54];
    if (fread(hdr, 1, 54, fp) != 54 || hdr[0] != 'B' || hdr[1] != 'M') {
        fprintf(stderr, "loadBMP: BMP 헤더가 아님 -> %s\n", path);
        fclose(fp); return 0;
    }

    int dataOff = *(int*)&hdr[10];
    int imgW = *(int*)&hdr[18];
    int imgH = *(int*)&hdr[22];
    if (imgW <= 0 || imgH <= 0) {
        fprintf(stderr, "loadBMP: 지원하지 않는 이미지 크기 (%d x %d)\n", imgW, imgH);
        fclose(fp); return 0;
    }

    int rowSize = (imgW * 3 + 3) & ~3; // 4바이트 정렬을 고려한 패딩 정렬
    size_t rawSize = (size_t)rowSize * imgH;

    fseek(fp, dataOff, SEEK_SET);
    unsigned char* raw = new unsigned char[rawSize];
    if (fread(raw, 1, rawSize, fp) != rawSize) {
        fprintf(stderr, "loadBMP: 픽셀 데이터가 잘려 있음 -> %s\n", path);
        delete[] raw; fclose(fp); return 0;
    }
    fclose(fp);

    // BGR 배열 구조를 OpenGL 표준인 RGB 규격으로 정렬 순서 반전
    unsigned char* rgb = new unsigned char[(size_t)imgW * imgH * 3];
    for (int y = 0; y < imgH; y++) {
        for (int x = 0; x < imgW; x++) {
            int s = y * rowSize + x * 3;
            int d = (y * imgW + x) * 3;
            rgb[d + 0] = raw[s + 2];
            rgb[d + 1] = raw[s + 1];
            rgb[d + 2] = raw[s + 0];
        }
    }
    delete[] raw;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);

    delete[] rgb;
    return tex;
}
