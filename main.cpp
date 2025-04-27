#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern GLuint wallTex;

// IDs das texturas
GLuint doorTex, sofaTex, wallTex, stoveTex, fridgeTex, floorTex, woodTex, profTex;

float doorWidth  = 2.0f;   // largura do quad da porta (unidades OpenGL)  
float doorHeight = 2.5f;   // altura do quad da porta  


// Variáveis globais (no topo do arquivo, antes de main)
float camX      =  0.0f;      // alinhado ao centro em X
float camY      =  1.6f;      // altura dos olhos
float camZ      = -12.0f;     // bem mais longe no eixo Z negativo
float camAngleY =  3.14159f;  // 180°, olhando para dentro (+Z)
float camAngleX =  0.0f;      // nivelado

// Variável para o ângulo da porta
float doorAngle = 0.0f; // Porta inicialmente fechada

// Variável para definir um raio de colisão para a câmera (evita ficar "colado" nos objetos)
const float collisionRadius = 0.1f; // Pequeno raio para a câmera

// Função auxiliar para verificar colisão AABB (Axis-Aligned Bounding Box)
// Verifica se a posição da câmera (newX, newZ) com um raio 'radius'
// colide com uma caixa definida por min/max X/Z.
bool checkAABBCollision(float camX, float camZ, float radius,
                        float objMinX, float objMaxX, float objMinZ, float objMaxZ) {
    // Encontra o ponto mais próximo na caixa ao centro da câmera
    float closestX = fmax(objMinX, fmin(camX, objMaxX));
    float closestZ = fmax(objMinZ, fmin(camZ, objMaxZ));

    // Calcula a distância quadrada entre o centro da câmera e este ponto mais próximo
    float distanceX = camX - closestX;
    float distanceZ = camZ - closestZ;
    float distanceSquared = (distanceX * distanceX) + (distanceZ * distanceZ);

    // Se a distância for menor que o raio ao quadrado, há colisão
    return distanceSquared < (radius * radius);
}


// Função para configurar a iluminação (sem alterações)
void setupLighting() {
    // Parâmetros da luz ambiente
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    // Luz difusa
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    // Luz especular
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Configure a luz GL_LIGHT0 (a posição será definida na display, com identidade)
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Habilita a luz e o sistema de iluminação
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // Permite que as cores definidas com glColor afetem o material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// Desenha um paralelepípedo texturizado (centro na origem)
void drawTexturedBox(float w, float h, float d, GLuint tex) {
  float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glColor3f(1,1,1);
  glBegin(GL_QUADS);
    // Frente
    glTexCoord2f(0,0); glVertex3f(-hw,-hh, hd);
    glTexCoord2f(1,0); glVertex3f( hw,-hh, hd);
    glTexCoord2f(1,1); glVertex3f( hw, hh, hd);
    glTexCoord2f(0,1); glVertex3f(-hw, hh, hd);
    // Trás
    glTexCoord2f(0,0); glVertex3f( hw,-hh,-hd);
    glTexCoord2f(1,0); glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(1,1); glVertex3f(-hw, hh,-hd);
    glTexCoord2f(0,1); glVertex3f( hw, hh,-hd);
    // Esquerda
    glTexCoord2f(0,0); glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(1,0); glVertex3f(-hw,-hh, hd);
    glTexCoord2f(1,1); glVertex3f(-hw, hh, hd);
    glTexCoord2f(0,1); glVertex3f(-hw, hh,-hd);
    // Direita
    glTexCoord2f(0,0); glVertex3f( hw,-hh, hd);
    glTexCoord2f(1,0); glVertex3f( hw,-hh,-hd);
    glTexCoord2f(1,1); glVertex3f( hw, hh,-hd);
    glTexCoord2f(0,1); glVertex3f( hw, hh, hd);
    // Topo
    glTexCoord2f(0,0); glVertex3f(-hw, hh, hd);
    glTexCoord2f(1,0); glVertex3f( hw, hh, hd);
    glTexCoord2f(1,1); glVertex3f( hw, hh,-hd);
    glTexCoord2f(0,1); glVertex3f(-hw, hh,-hd);
    // Fundo
    glTexCoord2f(0,0); glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(1,0); glVertex3f( hw,-hh,-hd);
    glTexCoord2f(1,1); glVertex3f( hw,-hh, hd);
    glTexCoord2f(0,1); glVertex3f(-hw,-hh, hd);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);
}

// Funções de desenho dos objetos (sem alterações)
void drawRefrigerator() {
    glPushMatrix();
        // posiciona e escala exatamente como antes
        glTranslatef(0.0f, 1.5f, 0.0f);
        // cubo 1×3×1 texturizado com fridgeTex
        drawTexturedBox(1.0f, 3.0f, 1.0f, fridgeTex);
    glPopMatrix();
}

void drawStove() {
  glPushMatrix();
      // mesma translação de antes
      glTranslatef(0.0f, 0.4f, 0.0f);
      // corpo texturizado do fogão
      drawTexturedBox(2.0f, 0.8f, 1.0f, stoveTex);
  glPopMatrix();
}

void drawKitchenTable() {
    glPushMatrix();
      // Tampo texturizado
      glPushMatrix();
        glTranslatef(0.0f, 0.75f, 0.0f);
        drawTexturedBox(2.0f, 0.1f, 1.2f, woodTex);
      glPopMatrix();

      // Pernas texturizadas
      const float legW = 0.1f, legH = 0.75f, legD = 0.1f;
      glPushMatrix(); glTranslatef(-0.85f, 0.375f, -0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix();
      glPushMatrix(); glTranslatef( 0.85f, 0.375f, -0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix();
      glPushMatrix(); glTranslatef(-0.85f, 0.375f,  0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix();
      glPushMatrix(); glTranslatef( 0.85f, 0.375f,  0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix();
    glPopMatrix();
}

void drawChair() {
    glPushMatrix();
      // Assento texturizado
      glPushMatrix();
        glTranslatef(0.0f, 0.45f, 0.0f);
        drawTexturedBox(0.6f, 0.1f, 0.6f, woodTex);
      glPopMatrix();
      // Pernas texturizadas
      const float lw = 0.1f, lh = 0.45f, ld = 0.1f;
      glPushMatrix(); glTranslatef(-0.2f, 0.225f, -0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
      glPushMatrix(); glTranslatef( 0.2f, 0.225f, -0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
      glPushMatrix(); glTranslatef(-0.2f, 0.225f,  0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
      glPushMatrix(); glTranslatef( 0.2f, 0.225f,  0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPopMatrix();
}

void drawKitchenArea() {
    const float recuo = 0.5f;  // quanto afastar da parede
    // Geladeira: Posição (1.5, 0, 4.0)
    glPushMatrix();
        glTranslatef(1.5f, 0.0f, 4.0f);
        drawRefrigerator();
    glPopMatrix();

    // Fogão: Posição (3.5, 0, 4.0)
    glPushMatrix();
        glTranslatef(3.5f, 0.0f, 4.0f);
        drawStove();
    glPopMatrix();

    // Mesa de cozinha: antes (2.0,0,2.0), agora z=2.0-recuo
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 2.0f - recuo);
        drawKitchenTable();
    glPopMatrix();

    // Cadeiras ao redor da mesa também recuadas em Z
    // Frente: z = 1.0 → 1.0 - recuo
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 1.0f - recuo);
        drawChair();
    glPopMatrix();

    // Traseira: z = 3.0 → 3.0 - recuo
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 3.0f - recuo);
        drawChair();
    glPopMatrix();

    // Esquerda: pos original (1.0,0,2.0) → z=2.0-recuo
    glPushMatrix();
        glTranslatef(1.0f, 0.0f, 2.0f - recuo);
        drawChair();
    glPopMatrix();

    // Direita: pos original (3.0,0,2.0) → z=2.0-recuo
    glPushMatrix();
        glTranslatef(3.0f, 0.0f, 2.0f - recuo);
        drawChair();
    glPopMatrix();
}

// Desenha o sofá texturizado em duas partes
void drawSofa() {
    // Base do sofá
    glPushMatrix();
      glTranslatef(-3.0f, 0.3f, -1.0f);
      drawTexturedBox(2.0f, 0.6f, 1.0f, sofaTex);
    glPopMatrix();
    // Encosto
    glPushMatrix();
      glTranslatef(-3.0f, 0.8f, -1.0f + 0.45f);
      drawTexturedBox(2.0f, 0.8f, 0.2f, sofaTex);
    glPopMatrix();
}

void drawCenterTable() {
    glPushMatrix();
        // Tampo
        glTranslatef(-3.0f, 0.3f, -3.5f);
        drawTexturedBox(1.5f, 0.1f, 0.8f, woodTex);
    glPopMatrix();
    // Pernas
    const float lw = 0.1f, lh = 0.3f, ld = 0.1f;
    glPushMatrix(); glTranslatef(-3.6f, 0.15f, -3.85f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.4f, 0.15f, -3.85f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-3.6f, 0.15f, -3.15f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.4f, 0.15f, -3.15f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
}  

void drawTVDevice() {
    // TV - Posição (-3.5, 1.75, -4.99) - A colisão relevante é com a parede
    glPushMatrix();
        glTranslatef(-3.5f, 1.75f, -4.99f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex3f(-1.0f, -0.75f, 0.0f);
            glVertex3f( 1.0f, -0.75f, 0.0f);
            glVertex3f( 1.0f,  0.75f, 0.0f);
            glVertex3f(-1.0f,  0.75f, 0.0f);
        glEnd();
    glPopMatrix();
}

// extrai o desenho do quad da porta num helper:
void drawDoorQuad() {
    const float u0=0.25f, u1=0.75f, v0=0.0f, v1=1.0f;
    glBindTexture(GL_TEXTURE_2D, doorTex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
      glTexCoord2f(u0,v0); glVertex3f(0,       0,        0);
      glTexCoord2f(u1,v0); glVertex3f(doorWidth,0,        0);
      glTexCoord2f(u1,v1); glVertex3f(doorWidth,doorHeight,0);
      glTexCoord2f(u0,v1); glVertex3f(0,       doorHeight,0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawDoor() {
    const float roomZ = -5.0f;
    float offset = (doorAngle < 0.01f && camZ < roomZ)
                    ? -0.01f
                    : +0.01f;
    glPushMatrix();
      glTranslatef(-1.0f, 0.0f, roomZ + offset);
      glRotatef(-doorAngle * 180.0f/3.14159f, 0,1,0);
      drawDoorQuad();
    glPopMatrix();
}

void drawPicture() {
    // Tamanho do quadro
    const float w = 4.0f, h = 2.0f;
    // Ângulo de mapeamento UV completo
    const float u0 = 0.0f, u1 = 1.0f, v0 = 0.0f, v1 = 1.0f;

    glBindTexture(GL_TEXTURE_2D, profTex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        // quad centrado horizontalmente na parede de trás (z = +5)
        glTexCoord2f(u0,v0); glVertex3f(-w/2, 1.0f,  5.0f);
        glTexCoord2f(u1,v0); glVertex3f( w/2, 1.0f,  5.0f);
        glTexCoord2f(u1,v1); glVertex3f( w/2, 1.0f + h, 5.0f);
        glTexCoord2f(u0,v1); glVertex3f(-w/2, 1.0f + h, 5.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}  

void drawRoom() {
    // 1) Piso
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    const float T = 5.0f;
    glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(-5, 0, -5);
      glTexCoord2f(T, 0); glVertex3f( 5, 0, -5);
      glTexCoord2f(T, T); glVertex3f( 5, 0,  5);
      glTexCoord2f(0, T); glVertex3f(-5, 0,  5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_LIGHTING);

    // 2) Paredes
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


    const float wallH = 3.0f;
    const float picW  = 4.0f;
    const float picH  = 2.0f;
    const float picY  = 1.0f;
    
    // shift pra esquerda
    const float dx    = -2.5f;       
    const float picX0 = -picW/2 + dx;   // ponto esquerdo
    const float picX1 =  picW/2 + dx;   // ponto direito
    

    // 2.1) Parede da frente (z = -5), abrindo só o vão da porta
    {
      const float doorX0 = -1.0f, doorX1 = doorX0 + doorWidth;
      glBindTexture(GL_TEXTURE_2D, wallTex);
      if (doorAngle < 0.01f) {
        // parede inteira
        glBegin(GL_QUADS);
          glTexCoord2f(0,0); glVertex3f(-5, 0, -5);
          glTexCoord2f(1,0); glVertex3f( 5, 0, -5);
          glTexCoord2f(1,1); glVertex3f( 5,wallH,-5);
          glTexCoord2f(0,1); glVertex3f(-5,wallH,-5);
        glEnd();
      } else {
        // lado esquerdo
        glBegin(GL_QUADS);
          glTexCoord2f(0,0);                       glVertex3f(-5,0,-5);
          glTexCoord2f((doorX0+5)/10,0);           glVertex3f(doorX0,0,-5);
          glTexCoord2f((doorX0+5)/10,1);           glVertex3f(doorX0,wallH,-5);
          glTexCoord2f(0,1);                       glVertex3f(-5,wallH,-5);
        glEnd();
        // lado direito
        glBegin(GL_QUADS);
          glTexCoord2f((doorX1+5)/10,0);           glVertex3f(doorX1,0,-5);
          glTexCoord2f(1,0);                       glVertex3f( 5,0,-5);
          glTexCoord2f(1,1);                       glVertex3f( 5,wallH,-5);
          glTexCoord2f((doorX1+5)/10,1);           glVertex3f(doorX1,wallH,-5);
        glEnd();
        // acima do vão
        glBegin(GL_QUADS);
          float u0 = (doorX0+5)/10, u1 = (doorX1+5)/10;
          float v0 = doorHeight/wallH;
          glTexCoord2f(u0,v0); glVertex3f(doorX0,doorHeight,-5);
          glTexCoord2f(u1,v0); glVertex3f(doorX1,doorHeight,-5);
          glTexCoord2f(u1,1);  glVertex3f(doorX1,wallH,   -5);
          glTexCoord2f(u0,1);  glVertex3f(doorX0,wallH,   -5);
        glEnd();
      }
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    // 2.2) Parede de trás (z = +5): esquerda, quadro e direita
    // faixa esquerda
    glBindTexture(GL_TEXTURE_2D, wallTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);                          glVertex3f(-5,    0, 5);
    glTexCoord2f((picX0+5)/10,0);               glVertex3f(picX0, 0, 5);
    glTexCoord2f((picX0+5)/10,1);               glVertex3f(picX0,wallH,5);
    glTexCoord2f(0,1);                          glVertex3f(-5, wallH,5);
    glEnd();

    // quadro
    glBindTexture(GL_TEXTURE_2D, profTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(picX0,      picY,      5);
    glTexCoord2f(1,0); glVertex3f(picX1,      picY,      5);
    glTexCoord2f(1,1); glVertex3f(picX1,picY+picH,      5);
    glTexCoord2f(0,1); glVertex3f(picX0,picY+picH,      5);
    glEnd();

    // faixa de baixo (sob o quadro)
    glBindTexture(GL_TEXTURE_2D, wallTex);
    glBegin(GL_QUADS);
    float u0 = (picX0+5.0f)/10.0f,  u1 = (picX1+5.0f)/10.0f;
    float v1 = picY / wallH;  // parte até o rodapé do quadro
    glTexCoord2f(u0,0); glVertex3f(picX0,    0, 5);
    glTexCoord2f(u1,0); glVertex3f(picX1,    0, 5);
    glTexCoord2f(u1,v1);glVertex3f(picX1, picY, 5);
    glTexCoord2f(u0,v1);glVertex3f(picX0, picY, 5);
    glEnd();

    // faixa direita
    glBindTexture(GL_TEXTURE_2D, wallTex);
    glBegin(GL_QUADS);
    glTexCoord2f((picX1+5)/10,0); glVertex3f(picX1,     0, 5);
    glTexCoord2f(1,0);            glVertex3f( 5,        0, 5);
    glTexCoord2f(1,1);            glVertex3f( 5,    wallH, 5);
    glTexCoord2f((picX1+5)/10,1); glVertex3f(picX1, wallH, 5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // 2.3) Laterais (x = ±5)
    glBindTexture(GL_TEXTURE_2D, wallTex);
    // esquerda
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(-5,0, 5);
      glTexCoord2f(1,0); glVertex3f(-5,0,-5);
      glTexCoord2f(1,1); glVertex3f(-5,3,-5);
      glTexCoord2f(0,1); glVertex3f(-5,3, 5);
    glEnd();
    // direita
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f( 5,0,-5);
      glTexCoord2f(1,0); glVertex3f( 5,0, 5);
      glTexCoord2f(1,1); glVertex3f( 5,3, 5);
      glTexCoord2f(0,1); glVertex3f( 5,3,-5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // 3) Restaura e desenha porta em cima de tudo
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_LIGHTING);
    drawDoor();
}

// Carrega uma textura e retorna seu GLuint
GLuint loadTexture(const char *filename) {
    int w, h, n;
    unsigned char *data = stbi_load(filename, &w, &h, &n, 0);
    if (!data) {
        fprintf(stderr, "Erro abrindo %s\n", filename);
        return 0;
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (n == 3)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,  w, h, GL_RGB,  GL_UNSIGNED_BYTE, data);
    else
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Configura Posição da Luz
    glPushMatrix();
        glLoadIdentity();
        GLfloat lightPosition[] = { 0.0f, 2.8f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glPopMatrix();

    // Configura Câmera
    gluLookAt(camX, camY, camZ,
        camX + sinf(camAngleY) * cosf(camAngleX),
        camY + sinf(camAngleX),
        camZ - cosf(camAngleY) * cosf(camAngleX),
        0.0, 1.0, 0.0);

    // Desenha a Cena
    drawRoom();
    drawSofa();
    drawCenterTable();
    drawTVDevice();
    drawKitchenArea();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w/(float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}


// ==============================================================
// FUNÇÃO DE VERIFICAÇÃO DE COLISÃO ATUALIZADA
// ==============================================================
bool checkCollision(float newX, float newZ) {
    // Limites da sala (considerando o raio da câmera para não atravessar)
    const float roomMinX = -5.0f + collisionRadius;
    const float roomMaxX =  5.0f - collisionRadius;
    const float roomMinZ = -5.0f + collisionRadius;
    const float roomMaxZ =  5.0f - collisionRadius;

    // --- 1. Colisão com Paredes ---
    if (newX < roomMinX || newX > roomMaxX || newZ < roomMinZ || newZ > roomMaxZ) {
        // Lógica especial para a parede frontal (porta e TV)
        if (newZ < roomMinZ && newX > -1.0f && newX < 1.0f) { // Área da porta
             if (doorAngle > 0.01f) { // Porta está aberta?
                 // Permite entrar/sair apenas se estiver perto da porta
                 // (Não implementado aqui para simplicidade, pode ser adicionado)
                 return false; // Permite a passagem pela porta aberta
             } else {
                 return true; // Colisão com a porta fechada
             }
        }
        // Adicione aqui a lógica para a abertura da TV se quiser permitir passar por ela
        // if (newZ < roomMinZ && newX > -4.5f && newX < -2.5f && camY > 1.0f && camY < 2.5f) { ... }

        return true; // Colisão com qualquer outra parte das paredes externas
    }

    // --- 2. Colisão com Objetos Internos ---
    // Define as bounding boxes (MinX, MaxX, MinZ, MaxZ) para cada objeto
    // As dimensões são calculadas a partir das funções de desenho (posição + escala)

    // Geladeira: pos(1.5, 4.0), scale(1, 1) -> X(1.0 a 2.0), Z(3.5 a 4.5)
    if (checkAABBCollision(newX, newZ, collisionRadius, 1.0f, 2.0f, 3.5f, 4.5f)) return true;

    // Fogão: pos(3.5, 4.0), scale(2, 1) -> X(2.5 a 4.5), Z(3.5 a 4.5)
    if (checkAABBCollision(newX, newZ, collisionRadius, 2.5f, 4.5f, 3.5f, 4.5f)) return true;

    // Mesa de Cozinha (Tampo): pos(2.0, 2.0), scale(2.0, 1.2) -> X(1.0 a 3.0), Z(1.4 a 2.6)
    if (checkAABBCollision(newX, newZ, collisionRadius, 1.0f, 3.0f, 1.4f, 2.6f)) return true;

    // Cadeira Frontal: pos(2.0, 1.0), scale(0.6, 0.6) -> X(1.7 a 2.3), Z(0.7 a 1.3)
    if (checkAABBCollision(newX, newZ, collisionRadius, 1.7f, 2.3f, 0.7f, 1.3f)) return true;

    // Cadeira Traseira: pos(2.0, 3.0), scale(0.6, 0.6) -> X(1.7 a 2.3), Z(2.7 a 3.3)
    if (checkAABBCollision(newX, newZ, collisionRadius, 1.7f, 2.3f, 2.7f, 3.3f)) return true;

    // Cadeira Esquerda: pos(1.0, 2.0), scale(0.6, 0.6) -> X(0.7 a 1.3), Z(1.7 a 2.3)
    if (checkAABBCollision(newX, newZ, collisionRadius, 0.7f, 1.3f, 1.7f, 2.3f)) return true;

    // Cadeira Direita: pos(3.0, 2.0), scale(0.6, 0.6) -> X(2.7 a 3.3), Z(1.7 a 2.3)
    if (checkAABBCollision(newX, newZ, collisionRadius, 2.7f, 3.3f, 1.7f, 2.3f)) return true;

    // Sofá (Base): pos(-3.0, -1.0), scale(2.0, 1.0) -> X(-4.0 a -2.0), Z(-1.5 a -0.5)
    if (checkAABBCollision(newX, newZ, collisionRadius, -4.0f, -2.0f, -1.5f, -0.5f)) return true;

    // Mesa de Centro (Tampo): pos(-3.0, -3.5), scale(1.5, 0.8) -> X(-3.75 a -2.25), Z(-3.9 a -3.1)
    if (checkAABBCollision(newX, newZ, collisionRadius, -3.75f, -2.25f, -3.9f, -3.1f)) return true;


    // --- 3. Sem Colisão ---
    return false;
}


// Função de teclado (sem alterações na lógica principal, apenas usa a nova checkCollision)
void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f; // Diminuir um pouco a velocidade pode ajudar com colisões
    float newX = camX;
    float newZ = camZ;

    switch (key) {
        case 'w':
            newX += sinf(camAngleY) * cosf(camAngleX) * moveSpeed; // Considera inclinação X
            newZ += -cosf(camAngleY) * cosf(camAngleX) * moveSpeed; // Considera inclinação X
            break;
        case 's':
            newX -= sinf(camAngleY) * cosf(camAngleX) * moveSpeed;
            newZ -= -cosf(camAngleY) * cosf(camAngleX) * moveSpeed;
            break;
        case 'a': // Movimento lateral strafe
            newX -= cosf(camAngleY) * moveSpeed;
            newZ -= sinf(camAngleY) * moveSpeed;
            break;
        case 'd': // Movimento lateral strafe
            newX += cosf(camAngleY) * moveSpeed;
            newZ += sinf(camAngleY) * moveSpeed;
            break;
        case 'o':  // Abrir/fechar a porta
            if (doorAngle < 0.01f) {
                doorAngle = 3.14159f / 2.0f; // Abre 90 graus
            } else {
                doorAngle = 0.0f; // Fecha
            }
            break;
        case 27:  // ESC para sair
            exit(0);
            break;
    }

    // Verifica colisão ANTES de atualizar a posição
    if (!checkCollision(newX, newZ)) {
        camX = newX;
        camZ = newZ;
    } else {
       // Opcional: Tentar deslizar ao longo da parede/objeto
       // Tenta mover apenas em X
       if (!checkCollision(newX, camZ)) {
           camX = newX;
       }
       // Tenta mover apenas em Z
       else if (!checkCollision(camX, newZ)) {
           camZ = newZ;
       }
       // Se ambos colidem, não move nada
    }


    glutPostRedisplay();
}

// Função specialKeys (sem alterações)
void specialKeys(int key, int x, int y) {
    float angleSpeed = 0.05f; // Reduzir velocidade de rotação
    float verticalLimit = 1.5f; // Limite para olhar para cima/baixo (radianos)

    switch(key) {
        case GLUT_KEY_LEFT:
            camAngleY -= angleSpeed;
            break;
        case GLUT_KEY_RIGHT:
            camAngleY += angleSpeed;
            break;
        case GLUT_KEY_UP:       // Olhar para cima
            camAngleX += angleSpeed;
            if (camAngleX > verticalLimit) camAngleX = verticalLimit; // Limita ângulo
            break;
        case GLUT_KEY_DOWN:      // Olhar para baixo
            camAngleX -= angleSpeed;
             if (camAngleX < -verticalLimit) camAngleX = -verticalLimit; // Limita ângulo
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ambiente Simulado (Casa) - Texturas Minecraft");

    glEnable(GL_DEPTH_TEST);
    setupLighting();

    glEnable(GL_TEXTURE_2D);
    doorTex = loadTexture("images/porta.png");
    sofaTex = loadTexture("images/textura-sofa.png");
    wallTex = loadTexture("images/parede.png");
    stoveTex = loadTexture("images/fornalha-acesa.png");
    fridgeTex = loadTexture("images/geladeira.png");
    floorTex  = loadTexture("images/piso.png");
    woodTex = loadTexture("images/madeira.png");
    profTex = loadTexture("images/professor (1).png");


    // re-bind para configurar wrap REPEAT
    glBindTexture(GL_TEXTURE_2D, floorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}