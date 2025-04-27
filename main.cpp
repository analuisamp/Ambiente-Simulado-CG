#include <GL/glut.h> // GLUT: cria janela, trata teclado/mouse, gerencia loop de redesenho
#include <cmath>  // Funções matemáticas (seno, cosseno, mínimo, máximo)
#include <cstdlib> // Funções utilitárias como exit()

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"  // Carrega PNG/JPG do disco para memória 

//----------------------------------------------------------------------
// 1) Declarações de texturas
//----------------------------------------------------------------------

// IDs que o OpenGL vai preencher para cada textura
GLuint doorTex, sofaTex, wallTex, stoveTex, fridgeTex, floorTex, woodTex, profTex;

//----------------------------------------------------------------------
// 2) Parâmetros geométricos e da câmera
//----------------------------------------------------------------------

// Largura e altura (em unidades OpenGL) do quad que desenha a porta
float doorWidth  = 2.0f;    
float doorHeight = 2.5f;  

// Posição da "câmera" no mundo 3D
float camX      =  0.0f;   // eixo X (esquerda-direita)
float camY      =  1.6f;   // eixo Y (altura do olho)
float camZ      = -12.0f;  // eixo Z (profundidade) 

// Orientação da câmera:
//  - camAngleY: rotação em volta do eixo Y (gira na horizontal)
//  - camAngleX: inclinação para cima/baixo
float camAngleY = 3.14159f;  // 180° (olha para o +Z)
float camAngleX = 0.0f;      // sem inclinação

// Ângulo atual da porta (0 = fechada, π/2 = aberta)
float doorAngle = 0.0f;

// Raio usado para colisão
const float collisionRadius = 0.1f;

//----------------------------------------------------------------------
// 3) Função de detecção de colisão AABB
//----------------------------------------------------------------------

// Verifica se um ponto (camX, camZ) “colide” com uma caixa alinhada
// aos eixos (objMinX→objMaxX, objMinZ→objMaxZ), expandida em 'radius'.
bool checkAABBCollision(float camX, float camZ, float radius,
                        float objMinX, float objMaxX, float objMinZ, float objMaxZ) {

 // 1) Encontra o ponto (closestX, closestZ) na caixa mais próximo da câmera
    float closestX = fmax(objMinX, fmin(camX, objMaxX));
    float closestZ = fmax(objMinZ, fmin(camZ, objMaxZ));

     // 2) Distância da câmera até esse ponto
    float distanceX = camX - closestX;
    float distanceZ = camZ - closestZ;
    float distanceSquared = (distanceX * distanceX) + (distanceZ * distanceZ);

    // 3) Se menor que radius² → colisão
    return distanceSquared < (radius * radius);
}

//----------------------------------------------------------------------
// 4) Configuração da iluminação
//----------------------------------------------------------------------

void setupLighting() {
    // Define luz ambiente (não vem de lugar nenhum em específico)
    GLfloat lightAmbient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
    // Define luz difusa (que incide e espalha)
    GLfloat lightDiffuse[]  = {0.8f, 0.8f, 0.8f, 1.0f};
    // Define luz especular (reflexos brilhantes)
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Aplica cada componente à GL_LIGHT0
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_LIGHT0);    // ativa a luz 0
    glEnable(GL_LIGHTING);  // liga o sistema de iluminação

    // Faz com que glColor* influencie o material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

//----------------------------------------------------------------------
// 5) Desenha um paralelepípedo texturizado
//----------------------------------------------------------------------

void drawTexturedBox(float w, float h, float d, GLuint tex) {

    // Calcula meia-largura / meia-altura / meia-profundidade
    float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;

    // 1) Liga a textura 'tex'
    glBindTexture(GL_TEXTURE_2D, tex);
    // 2) Modo de aplicação: substitui cor do pixel pela textura
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // 3) Garantir cor branca para não modular a textura
    glColor3f(1.0f, 1.0f, 1.0f);

    // 4) Começa a desenhar 6 faces (QUADS)
    glBegin(GL_QUADS);
        // — Frente (z = +hd)
      glTexCoord2f(0,0); glVertex3f(-hw, -hh,  hd);
      glTexCoord2f(1,0); glVertex3f( hw, -hh,  hd);
      glTexCoord2f(1,1); glVertex3f( hw,  hh,  hd);
      glTexCoord2f(0,1); glVertex3f(-hw,  hh,  hd);

      // — Trás (z = -hd)
      glTexCoord2f(0,0); glVertex3f( hw, -hh, -hd);
      glTexCoord2f(1,0); glVertex3f(-hw, -hh, -hd);
      glTexCoord2f(1,1); glVertex3f(-hw,  hh, -hd);
      glTexCoord2f(0,1); glVertex3f( hw,  hh, -hd);

      // — Esquerda (x = -hw)
      glTexCoord2f(0,0); glVertex3f(-hw, -hh, -hd);
      glTexCoord2f(1,0); glVertex3f(-hw, -hh,  hd);
      glTexCoord2f(1,1); glVertex3f(-hw,  hh,  hd);
      glTexCoord2f(0,1); glVertex3f(-hw,  hh, -hd);

      // — Direita (x = +hw)
      glTexCoord2f(0,0); glVertex3f( hw, -hh,  hd);
      glTexCoord2f(1,0); glVertex3f( hw, -hh, -hd);
      glTexCoord2f(1,1); glVertex3f( hw,  hh, -hd);
      glTexCoord2f(0,1); glVertex3f( hw,  hh,  hd);

      // — Topo (y = +hh)
      glTexCoord2f(0,0); glVertex3f(-hw, hh,  hd);
      glTexCoord2f(1,0); glVertex3f( hw, hh,  hd);
      glTexCoord2f(1,1); glVertex3f( hw, hh, -hd);
      glTexCoord2f(0,1); glVertex3f(-hw, hh, -hd);

      // — Fundo (y = -hh)
      glTexCoord2f(0,0); glVertex3f(-hw, -hh, -hd);
      glTexCoord2f(1,0); glVertex3f( hw, -hh, -hd);
      glTexCoord2f(1,1); glVertex3f( hw, -hh,  hd);
      glTexCoord2f(0,1); glVertex3f(-hw, -hh,  hd);

    glEnd();
    // 5) Desliga a textura
    glBindTexture(GL_TEXTURE_2D, 0);
}

// =======================================================================
// 6) Desenho de Móveis Individuais
// =======================================================================

void drawRefrigerator() {
    glPushMatrix(); // 1) Empilha a matriz de transformações

        // 2) Translada o sistema de coordenadas para a posição da geladeira:
        //    x=0, y=1.5 (metade da altura da caixa), z=0
        glTranslatef(0.0f, 1.5f, 0.0f);  // Move até onde a geladeira deve ficar

        // 3) Desenha um paralelepípedo 1×3×1 usando a textura fridgeTex
        drawTexturedBox(1.0f, 3.0f, 1.0f, fridgeTex);
    glPopMatrix();  // 4) Desempilha (restaura) a matriz anterior
}

void drawStove() {
  glPushMatrix();    // salva matriz
      glTranslatef(0.0f, 0.4f, 0.0f);
      // desenha o corpo do fogão 2×0.8×1 com textura stoveTex
      drawTexturedBox(2.0f, 0.8f, 1.0f, stoveTex);
  glPopMatrix(); // restaura matriz
}

void drawKitchenTable() {
    glPushMatrix(); // contexto global da mesa
       // Tampo da mesa
      glPushMatrix();
        // sobe o tampo até y=0.75
        glTranslatef(0.0f, 0.75f, 0.0f);
        // desenha tampo 2×0.1×1.2 com textura de madeira
        drawTexturedBox(2.0f, 0.1f, 1.2f, woodTex);
      glPopMatrix();    // volta contexto da mesa

    // Pernas da mesa
      const float legW = 0.1f, legH = 0.75f, legD = 0.1f;
      glPushMatrix(); glTranslatef(-0.85f, 0.375f, -0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix(); // perna dianteira-esquerda
      glPushMatrix(); glTranslatef( 0.85f, 0.375f, -0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix(); // dianteira-direita
      glPushMatrix(); glTranslatef(-0.85f, 0.375f,  0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix(); // traseira-esquerda
      glPushMatrix(); glTranslatef( 0.85f, 0.375f,  0.45f); drawTexturedBox(legW,legH,legD, woodTex); glPopMatrix(); // traseira-direita
    glPopMatrix();  // fecha contexto da mesa inteira
}

void drawChair() {
    glPushMatrix();  // contexto da cadeira
      // Assento texturizado
      glPushMatrix();
         // eleva o assento até y=0.45
         glTranslatef(0.0f, 0.45f, 0.0f);
         // desenha assento 0.6×0.1×0.6
         drawTexturedBox(0.6f, 0.1f, 0.6f, woodTex);
      glPopMatrix();
      
      // Pernas da cadeira
      const float lw = 0.1f, lh = 0.45f, ld = 0.1f;
      glPushMatrix(); glTranslatef(-0.2f, 0.225f, -0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();  // perna 1
      glPushMatrix(); glTranslatef( 0.2f, 0.225f, -0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();  // perna 2
      glPushMatrix(); glTranslatef(-0.2f, 0.225f,  0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();  // perna 3
      glPushMatrix(); glTranslatef( 0.2f, 0.225f,  0.2f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();  // perna 4
    glPopMatrix(); // fim da cadeira
}

// =======================================================================
// 7) Montagem da Área da Cozinha (vários móveis juntos)
// =======================================================================

void drawKitchenArea() {
    const float recuo = 0.5f;  // distância da parede

    // Geladeira
    glPushMatrix();
        glTranslatef(1.5f, 0.0f, 4.0f); // posiciona
        drawRefrigerator(); // chama desenho da geladeira
    glPopMatrix();

    // Os outros seguem a mesma lógica
    // Fogão
    glPushMatrix();
        glTranslatef(3.5f, 0.0f, 4.0f); 
        drawStove();
    glPopMatrix();

    // Mesa de cozinha
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 2.0f - recuo);
        drawKitchenTable();
    glPopMatrix();

    // Cadeiras ao redor da mesa
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 1.0f - recuo);
        drawChair();
    glPopMatrix();

    // Traseira
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 3.0f - recuo);
        drawChair();
    glPopMatrix();

    // Esquerda
    glPushMatrix();
        glTranslatef(1.0f, 0.0f, 2.0f - recuo);
        drawChair();
    glPopMatrix();

    // Direita
    glPushMatrix();
        glTranslatef(3.0f, 0.0f, 2.0f - recuo);
        drawChair();
    glPopMatrix();
}

// =======================================================================
// 8) Outros Móveis: Sofá, Mesa de Centro e TV
// =======================================================================

void drawSofa() {
    // Base do sofá
    glPushMatrix();
      glTranslatef(-3.0f, 0.3f, -1.0f);
      drawTexturedBox(2.0f, 0.6f, 1.0f, sofaTex);
    glPopMatrix();
    // Encosto do sofá, deslocado 0.45 em Z em relação à base
    glPushMatrix();
      glTranslatef(-3.0f, 0.8f, -1.0f + 0.45f);
      drawTexturedBox(2.0f, 0.8f, 0.2f, sofaTex);
    glPopMatrix();
}

void drawCenterTable() {
    glPushMatrix();
        // Tampo da mesinha de centro
        glTranslatef(-3.0f, 0.3f, -3.5f);
        drawTexturedBox(1.5f, 0.1f, 0.8f, woodTex);
    glPopMatrix();
    
    // 4 perninhas da mesinha
    const float lw = 0.1f, lh = 0.3f, ld = 0.1f;
    glPushMatrix(); glTranslatef(-3.6f, 0.15f, -3.85f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.4f, 0.15f, -3.85f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-3.6f, 0.15f, -3.15f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.4f, 0.15f, -3.15f); drawTexturedBox(lw,lh,ld, woodTex); glPopMatrix();
}  

void drawTVDevice() {
    // posiciona a TV quase encostada na parede de trás (z = -4.99)
    glPushMatrix();
        glTranslatef(-3.5f, 1.75f, -4.99f);
        glColor3f(0.0f, 0.0f, 0.0f);     // cor preta
        glBegin(GL_QUADS);              // desenha um quad simples
            glVertex3f(-1.0f, -0.75f, 0.0f);
            glVertex3f( 1.0f, -0.75f, 0.0f);
            glVertex3f( 1.0f,  0.75f, 0.0f);
            glVertex3f(-1.0f,  0.75f, 0.0f);
        glEnd();
    glPopMatrix();
}

// =======================================================================
// 9) Porta e Quadro (detalhes da parede)
// =======================================================================

void drawDoorQuad() {
    // define sub-região da textura da porta
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
    const float roomZ = -5.0f;   // parede frontal em z=-5  
    
    // pequeno offset para evitar z-fighting com a parede
    float offset = (doorAngle < 0.01f && camZ < roomZ)
                    ? -0.01f
                    : +0.01f;
    glPushMatrix();
      glTranslatef(-1.0f, 0.0f, roomZ + offset); // posiciona base da porta
      glRotatef(-doorAngle * 180.0f/3.14159f, 0,1,0); // gira no eixo Y
      drawDoorQuad(); // desenha o quad texturizado
    glPopMatrix();
}

void drawPicture() {
    const float w = 4.0f, h = 2.0f;     // dimensões do quadro
    const float u0 = 0.0f, u1 = 1.0f, v0 = 0.0f, v1 = 1.0f; // mapeamento completo da textura

    glBindTexture(GL_TEXTURE_2D, profTex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        // quadro centralizado em x (−w/2 → +w/2), fixo em z=+5 (parede traseira)
        glTexCoord2f(u0,v0); glVertex3f(-w/2, 1.0f,  5.0f);
        glTexCoord2f(u1,v0); glVertex3f( w/2, 1.0f,  5.0f);
        glTexCoord2f(u1,v1); glVertex3f( w/2, 1.0f + h, 5.0f);
        glTexCoord2f(u0,v1); glVertex3f(-w/2, 1.0f + h, 5.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}  

// =======================================================================
// 5) Desenha todo o ambiente: piso, teto, paredes e porta
// =======================================================================

void drawRoom() {
    // Parâmetros fixos das paredes e do quadro
    const float wallH = 3.0f;                     // altura das paredes
    const float picW  = 4.0f;                     // largura do quadro
    const float picH  = 2.0f;                     // altura do quadro
    const float picY  = 1.0f;                     // altura do ponto inferior do quadro
    
    // deslocamento em X para centralizar o quadro
    const float dx    = -2.5f;
    const float picX0 = -picW/2 + dx;             // X da borda esquerda do quadro
    const float picX1 =  picW/2 + dx;             // X da borda direita do quadro

    // 1) Piso
    glDisable(GL_LIGHTING);                       // desliga iluminação para o piso
    glEnable(GL_TEXTURE_2D);                      // habilita texturas
    glBindTexture(GL_TEXTURE_2D, floorTex);       // seleciona textura do piso
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    const float T = 5.0f;                         // fator de repetição da textura
    glBegin(GL_QUADS);
      // mapeia (0,0)→(T,T) em um quad 10×10 no chão
      glTexCoord2f(0, 0); glVertex3f(-5, 0, -5);
      glTexCoord2f(T, 0); glVertex3f( 5, 0, -5);
      glTexCoord2f(T, T); glVertex3f( 5, 0,  5);
      glTexCoord2f(0, T); glVertex3f(-5, 0,  5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);              // desassocia textura
    glEnable(GL_LIGHTING);                        // reativa iluminação

    // 2) Teto
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);        // usa textura de parede no teto
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);
        // quad horizontal em y = wallH
        glTexCoord2f(0, 0); glVertex3f(-5, wallH, -5);
        glTexCoord2f(1, 0); glVertex3f( 5, wallH, -5);
        glTexCoord2f(1, 1); glVertex3f( 5, wallH,  5);
        glTexCoord2f(0, 1); glVertex3f(-5, wallH,  5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_LIGHTING);

    // 3) Paredes
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // 3.1) Parede da frente (z = -5), com vão dinâmico para a porta
    {
      const float doorX0 = -1.0f;
      const float doorX1 = doorX0 + doorWidth;
      glBindTexture(GL_TEXTURE_2D, wallTex);
      if (doorAngle < 0.01f) {
        // sem porta aberta: desenha a parede inteira
        glBegin(GL_QUADS);
          glTexCoord2f(0,0); glVertex3f(-5,      0, -5);
          glTexCoord2f(1,0); glVertex3f( 5,      0, -5);
          glTexCoord2f(1,1); glVertex3f( 5, wallH, -5);
          glTexCoord2f(0,1); glVertex3f(-5, wallH, -5);
        glEnd();
      } else {
        // parede esquerda do vão
        glBegin(GL_QUADS);
          glTexCoord2f(0,0);                       glVertex3f(-5,0,-5);
          glTexCoord2f((doorX0+5)/10,0);           glVertex3f(doorX0,0,-5);
          glTexCoord2f((doorX0+5)/10,1);           glVertex3f(doorX0,wallH,-5);
          glTexCoord2f(0,1);                       glVertex3f(-5,wallH,-5);
        glEnd();
        // parede direita do vão
        glBegin(GL_QUADS);
          glTexCoord2f((doorX1+5)/10,0);           glVertex3f(doorX1,0,-5);
          glTexCoord2f(1,0);                       glVertex3f( 5,0,-5);
          glTexCoord2f(1,1);                       glVertex3f( 5,wallH,-5);
          glTexCoord2f((doorX1+5)/10,1);           glVertex3f(doorX1,wallH,-5);
        glEnd();
        // acima do vão da porta
        glBegin(GL_QUADS);
          float u0 = (doorX0+5)/10, u1 = (doorX1+5)/10;
          float v0 = doorHeight / wallH;
          glTexCoord2f(u0,v0); glVertex3f(doorX0,doorHeight,-5);
          glTexCoord2f(u1,v0); glVertex3f(doorX1,doorHeight,-5);
          glTexCoord2f(u1,1);  glVertex3f(doorX1,wallH,   -5);
          glTexCoord2f(u0,1);  glVertex3f(doorX0,wallH,   -5);
        glEnd();
      }
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    // 3.2) Parede de trás (z = +5): trecho esquerdo, quadro e trecho direito
    // — trecho esquerdo
    glBindTexture(GL_TEXTURE_2D, wallTex);
    glBegin(GL_QUADS);
      glTexCoord2f(0,0);                          glVertex3f(-5,    0, 5);
      glTexCoord2f((picX0+5)/10,0);               glVertex3f(picX0, 0, 5);
      glTexCoord2f((picX0+5)/10,1);               glVertex3f(picX0,wallH,5);
      glTexCoord2f(0,1);                          glVertex3f(-5, wallH,5);
    glEnd();

    // — quadro
    glBindTexture(GL_TEXTURE_2D, profTex);
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(picX0,      picY,      5);
      glTexCoord2f(1,0); glVertex3f(picX1,      picY,      5);
      glTexCoord2f(1,1); glVertex3f(picX1,picY+picH,      5);
      glTexCoord2f(0,1); glVertex3f(picX0,picY+picH,      5);
    glEnd();

    // — faixa abaixo do quadro
    glBindTexture(GL_TEXTURE_2D, wallTex);
    glBegin(GL_QUADS);
      float u0b = (picX0+5)/10, u1b = (picX1+5)/10;
      float v1  = picY / wallH;
      glTexCoord2f(u0b, 0); glVertex3f(picX0,    0, 5);
      glTexCoord2f(u1b, 0); glVertex3f(picX1,    0, 5);
      glTexCoord2f(u1b,v1); glVertex3f(picX1, picY, 5);
      glTexCoord2f(u0b,v1); glVertex3f(picX0, picY, 5);
    glEnd();

    // — trecho direito
    glBindTexture(GL_TEXTURE_2D, wallTex);
    glBegin(GL_QUADS);
      glTexCoord2f((picX1+5)/10,0); glVertex3f(picX1,     0, 5);
      glTexCoord2f(1,0);            glVertex3f( 5,        0, 5);
      glTexCoord2f(1,1);            glVertex3f( 5,    wallH, 5);
      glTexCoord2f((picX1+5)/10,1); glVertex3f(picX1, wallH, 5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // 3.3) Laterais (x = ±5)
    glBindTexture(GL_TEXTURE_2D, wallTex);
    // – lateral esquerda
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(-5,0, 5);
      glTexCoord2f(1,0); glVertex3f(-5,0,-5);
      glTexCoord2f(1,1); glVertex3f(-5,3,-5);
      glTexCoord2f(0,1); glVertex3f(-5,3, 5);
    glEnd();
    // – lateral direita
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f( 5,0,-5);
      glTexCoord2f(1,0); glVertex3f( 5,0, 5);
      glTexCoord2f(1,1); glVertex3f( 5,3, 5);
      glTexCoord2f(0,1); glVertex3f( 5,3,-5);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // 4) Desenha a porta sobre a parede (reaplica iluminação e textura de porta)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_LIGHTING);
    drawDoor();
}

// =======================================================================
// 6) Carrega uma imagem do disco e cria uma textura OpenGL
// =======================================================================
GLuint loadTexture(const char *filename) {
    int w, h, n;
    unsigned char *data = stbi_load(filename, &w, &h, &n, 0);
    if (!data) {
        fprintf(stderr, "Erro abrindo %s\n", filename);
        return 0;
    }
    GLuint tex;
    glGenTextures(1, &tex);                    // gera ID de textura
    glBindTexture(GL_TEXTURE_2D, tex);
    // configura wrapping e filtros
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // envia dados para GPU, gerando mipmaps
    if (n == 3)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,  w, h, GL_RGB,  GL_UNSIGNED_BYTE, data);
    else
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

// =======================================================================
// 7) Callback de exibição: limpas buffers, posiciona luz e câmera, desenha tudo
// =======================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // posiçãoda luz em mundo (luz fixa)
    glPushMatrix();
        glLoadIdentity();
        GLfloat lightPos[] = { 0.0f, 2.8f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glPopMatrix();

    // configura câmera: olhar de (camX,camY,camZ) → ponto à frente
    gluLookAt(
        camX, camY, camZ,
        camX + sinf(camAngleY)*cosf(camAngleX),
        camY + sinf(camAngleX),
        camZ - cosf(camAngleY)*cosf(camAngleX),
        0.0,1.0,0.0
    );

    // desenha elementos da cena
    drawRoom();
    drawSofa();
    drawCenterTable();
    drawTVDevice();
    drawKitchenArea();

    glutSwapBuffers();                         // troca buffers (double buffering)
}

// =======================================================================
// 8) Callback de redimensionamento de janela
// =======================================================================
void reshape(int w, int h) {
    if (h == 0) h = 1;                         // evita divisão por zero
    glViewport(0, 0, w, h);                   // define área de renderização
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // perspectiva de 60°, aspect=w/h, plano próximo 0.1, distante 100
    gluPerspective(60.0, (float)w/(float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// =======================================================================
// 9) Verifica colisão contra paredes e objetos internos
// =======================================================================
bool checkCollision(float newX, float newZ) {
    // limites internos da sala ajustados pelo raio da câmera
    const float roomMinX = -5.0f + collisionRadius;
    const float roomMaxX =  5.0f - collisionRadius;
    const float roomMinZ = -5.0f + collisionRadius;
    const float roomMaxZ =  5.0f - collisionRadius;

    // colisão com paredes externas?
    if (newX < roomMinX || newX > roomMaxX || newZ < roomMinZ || newZ > roomMaxZ) {
        // permite atravessar a porta se aberta
        if (newZ < roomMinZ && newX > -1.0f && newX < 1.0f && doorAngle > 0.01f)
            return false;
        return true;    // caso contrário, colisão
    }

    // verifica colisão contra cada objeto com AABB
    if (checkAABBCollision(newX,newZ,collisionRadius, 1.0f,2.0f,3.5f,4.5f)) return true; // geladeira
    if (checkAABBCollision(newX,newZ,collisionRadius, 2.5f,4.5f,3.5f,4.5f)) return true; // fogão
    if (checkAABBCollision(newX,newZ,collisionRadius, 1.0f,3.0f,1.4f,2.6f)) return true; // mesa cozinha
    // ... (demais objetos)
    return false;       // sem colisão
}

// =======================================================================
// 10) Callbacks de teclado para movimento e interação
// =======================================================================
void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;
    float newX = camX, newZ = camZ;

    switch (key) {
        case 'w': newX += sinf(camAngleY)*cosf(camAngleX)*moveSpeed;
                  newZ += -cosf(camAngleY)*cosf(camAngleX)*moveSpeed; break;
        case 's': newX -= sinf(camAngleY)*cosf(camAngleX)*moveSpeed;
                  newZ -= -cosf(camAngleY)*cosf(camAngleX)*moveSpeed; break;
        case 'a': newX -= cosf(camAngleY)*moveSpeed; newZ -= sinf(camAngleY)*moveSpeed; break;
        case 'd': newX += cosf(camAngleY)*moveSpeed; newZ += sinf(camAngleY)*moveSpeed; break;
        case 'o': // abre/fecha a porta
                  doorAngle = (doorAngle<0.01f ? 3.14159f/2.0f : 0.0f);
                  break;
        case 27: exit(0);  // ESC sai
    }

    // aplica movimento se não colidir
    if (!checkCollision(newX,newZ)) {
        camX=newX; camZ=newZ;
    } else {
        // tenta deslizar em X ou Z
        if (!checkCollision(newX,camZ)) camX=newX;
        else if (!checkCollision(camX,newZ)) camZ=newZ;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float angleSpeed = 0.05f, verticalLimit = 1.5f;
    switch(key) {
        case GLUT_KEY_LEFT:  camAngleY -= angleSpeed; break;
        case GLUT_KEY_RIGHT: camAngleY += angleSpeed; break;
        case GLUT_KEY_UP:    camAngleX = fmin(camAngleX+angleSpeed, verticalLimit); break;
        case GLUT_KEY_DOWN:  camAngleX = fmax(camAngleX-angleSpeed, -verticalLimit); break;
    }
    glutPostRedisplay();
}

// =======================================================================
// 11) Função principal: inicializa GLUT, carrega texturas e entra no loop
// =======================================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ambiente Simulado (Casa) - Texturas Minecraft");

    glEnable(GL_DEPTH_TEST);    // teste de profundidade para ocultar faces traseiras
    setupLighting();            // configura luzes

    glEnable(GL_TEXTURE_2D);    // habilita texturas em todo o programa
    // carrega cada textura uma única vez
    doorTex   = loadTexture("images/porta.png");
    sofaTex   = loadTexture("images/textura-sofa.png");
    wallTex   = loadTexture("images/parede.png");
    stoveTex  = loadTexture("images/fornalha-acesa.png");
    fridgeTex = loadTexture("images/geladeira.png");
    floorTex  = loadTexture("images/piso.png");
    woodTex   = loadTexture("images/madeira.png");
    profTex   = loadTexture("images/professor (1).png");

    // faz o piso repetir
    glBindTexture(GL_TEXTURE_2D, floorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    // registra callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();             // entra no loop principal do GLUT
    return 0;
}