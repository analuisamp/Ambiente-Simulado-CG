#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint doorTex;            // ID da textura carregada no GPU  
float doorWidth  = 2.0f;   // largura do quad da porta (unidades OpenGL)  
float doorHeight = 2.5f;   // altura do quad da porta  


// Variáveis globais para a posição e orientação da câmera
float camX = 0.0f, camY = 1.6f, camZ = 5.0f; // Posição inicial da câmera
float camAngleY = 0.0f;  // Ângulo de rotação no eixo Y
float camAngleX = 0.0f;  // Ângulo de rotação no eixo X

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

// Funções de desenho dos objetos (sem alterações)
void drawRefrigerator() {
    glPushMatrix();
        glColor3f(0.9f, 0.9f, 0.9f);
        glTranslatef(0.0f, 1.5f, 0.0f); // Eleva 1.5 (metade de 3.0)
        glScalef(1.0f, 3.0f, 1.0f); // Largura 1, Altura 3, Profundidade 1
        glutSolidCube(1.0);
    glPopMatrix();
}

void drawStove() {
    glPushMatrix();
        glColor3f(0.3f, 0.3f, 0.3f);
        glTranslatef(0.0f, 0.4f, 0.0f); // Eleva para base no chão
        glScalef(2.0f, 0.8f, 1.0f); // Largura 2, Altura 0.8, Profundidade 1
        glutSolidCube(1.0);
    glPopMatrix();
}

void drawKitchenTable() {
    glPushMatrix();
        // Tampo da mesa
        glPushMatrix();
            glTranslatef(0.0f, 0.75f, 0.0f); // Centro do tampo
            glScalef(2.0f, 0.1f, 1.2f); // Largura 2, Altura 0.1, Profundidade 1.2
            glColor3f(0.55f, 0.27f, 0.07f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Pernas (não relevantes para colisão simples com a câmera no alto)
        glColor3f(0.55f, 0.27f, 0.07f);
        glPushMatrix(); glTranslatef(-0.85f, 0.375f, -0.45f); glScalef(0.1f, 0.75f, 0.1f); glutSolidCube(1.0); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.85f, 0.375f, -0.45f); glScalef(0.1f, 0.75f, 0.1f); glutSolidCube(1.0); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.85f, 0.375f,  0.45f); glScalef(0.1f, 0.75f, 0.1f); glutSolidCube(1.0); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.85f, 0.375f,  0.45f); glScalef(0.1f, 0.75f, 0.1f); glutSolidCube(1.0); glPopMatrix();
    glPopMatrix();
}

void drawChair() {
    glPushMatrix();
        // Assento da cadeira
        glPushMatrix();
            glTranslatef(0.0f, 0.45f, 0.0f); // Centro do assento
            glScalef(0.6f, 0.1f, 0.6f); // Largura 0.6, Altura 0.1, Profundidade 0.6
            glColor3f(0.5f, 0.3f, 0.0f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Pernas (não relevantes para colisão simples)
         glColor3f(0.5f, 0.3f, 0.0f);
        glPushMatrix(); glTranslatef(-0.2f, 0.225f, -0.2f); glScalef(0.1f, 0.45f, 0.1f); glutSolidCube(1.0); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.2f, 0.225f, -0.2f); glScalef(0.1f, 0.45f, 0.1f); glutSolidCube(1.0); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.2f, 0.225f,  0.2f); glScalef(0.1f, 0.45f, 0.1f); glutSolidCube(1.0); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.2f, 0.225f,  0.2f); glScalef(0.1f, 0.45f, 0.1f); glutSolidCube(1.0); glPopMatrix();
    glPopMatrix();
}

void drawKitchenArea() {
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

    // Mesa de cozinha: Posição (2.0, 0, 2.0)
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 2.0f);
        drawKitchenTable();
    glPopMatrix();

    // Cadeiras ao redor da mesa
    // Cadeira frontal: Posição (2.0, 0, 1.0)
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 1.0f);
        drawChair();
    glPopMatrix();
    // Cadeira traseira: Posição (2.0, 0, 3.0)
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 3.0f);
        drawChair();
    glPopMatrix();
    // Cadeira esquerda: Posição (1.0, 0, 2.0)
    glPushMatrix();
        glTranslatef(1.0f, 0.0f, 2.0f);
        drawChair();
    glPopMatrix();
    // Cadeira direita: Posição (3.0, 0, 2.0)
    glPushMatrix();
        glTranslatef(3.0f, 0.0f, 2.0f);
        drawChair();
    glPopMatrix();
}

void drawSofa() {
    // Sofá: Posição (-3.0, 0, -1.0)
    glPushMatrix();
        glTranslatef(-3.0f, 0.0f, -1.0f);
        // Base do sofá
        glPushMatrix();
            glTranslatef(0.0f, 0.3f, 0.0f); // Centro da base
            glScalef(2.0f, 0.6f, 1.0f); // Largura 2, Altura 0.6, Profundidade 1
            glColor3f(0.7f, 0.2f, 0.2f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Encosto (não relevante para colisão simples)
        glPushMatrix();
            glTranslatef(0.0f, 0.8f, 0.45f);
            glScalef(2.0f, 0.8f, 0.2f);
            glColor3f(0.7f, 0.2f, 0.2f);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

void drawCenterTable() {
    // Mesa de Centro: Posição do tampo (-3.0, 0.3, -3.5)
    glPushMatrix();
        glTranslatef(-3.0f, 0.3f, -3.5f); // Centro do tampo
        glScalef(1.5f, 0.1f, 0.8f); // Largura 1.5, Altura 0.1, Profundidade 0.8
        glColor3f(0.6f, 0.3f, 0.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Pernas (não relevantes para colisão simples)
    glColor3f(0.6f, 0.3f, 0.0f);
    glPushMatrix(); glTranslatef(-3.0f - 0.6f, 0.15f, -3.5f - 0.35f); glScalef(0.1f, 0.3f, 0.1f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(-3.0f + 0.6f, 0.15f, -3.5f - 0.35f); glScalef(0.1f, 0.3f, 0.1f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(-3.0f - 0.6f, 0.15f, -3.5f + 0.35f); glScalef(0.1f, 0.3f, 0.1f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(-3.0f + 0.6f, 0.15f, -3.5f + 0.35f); glScalef(0.1f, 0.3f, 0.1f); glutSolidCube(1.0); glPopMatrix();
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

void drawDoor() {
    glPushMatrix();
      // posiciona e gira
      glTranslatef(-1,0,-5.0f+0.01f);
      glRotatef(-doorAngle*180.0f/3.14159f,0,1,0);
  
      // vincula só o ID já criado
      glBindTexture(GL_TEXTURE_2D, doorTex);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glColor3f(1,1,1);
  
      // UV recortadas para eliminar bordas pretas
      const float u0 = 0.25f, u1 = 0.75f, v0 = 0.0f, v1 = 1.0f;
  
      // quad 2×2.5
      glBegin(GL_QUADS);
        glTexCoord2f(u0,v0); glVertex3f(0,      0,       0);
        glTexCoord2f(u1,v0); glVertex3f(doorWidth, 0,       0);
        glTexCoord2f(u1,v1); glVertex3f(doorWidth, doorHeight,0);
        glTexCoord2f(u0,v1); glVertex3f(0,      doorHeight,0);
      glEnd();
  
      glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
  }

// --- Função de Desenho da Sala e Display (sem alterações significativas) ---
void drawRoom() {
    // Piso
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
       glVertex3f(-5.0f, 0.0f, -5.0f); glVertex3f(5.0f, 0.0f, -5.0f); glVertex3f(5.0f, 0.0f, 5.0f); glVertex3f(-5.0f, 0.0f, 5.0f);
    glEnd();

    // --- Parede frontal (z = -5) --- Detalhes omitidos para brevidade, são iguais aos anteriores
     glColor3f(0.8f, 0.8f, 0.8f);
    // Segmento A: Esquerda
    glBegin(GL_QUADS); glVertex3f(-5.0f, 0.0f, -5.0f); glVertex3f(-4.5f, 0.0f, -5.0f); glVertex3f(-4.5f, 3.0f, -5.0f); glVertex3f(-5.0f, 3.0f, -5.0f); glEnd();
    // Segmento B: Recorte para a TV (inferior e superior)
    glBegin(GL_QUADS); glVertex3f(-4.5f, 0.0f, -5.0f); glVertex3f(-2.5f, 0.0f, -5.0f); glVertex3f(-2.5f, 1.0f, -5.0f); glVertex3f(-4.5f, 1.0f, -5.0f); glEnd();
    glBegin(GL_QUADS); glVertex3f(-4.5f, 2.5f, -5.0f); glVertex3f(-2.5f, 2.5f, -5.0f); glVertex3f(-2.5f, 3.0f, -5.0f); glVertex3f(-4.5f, 3.0f, -5.0f); glEnd();
    // Segmento C: Entre TV e Porta
    glBegin(GL_QUADS); glVertex3f(-2.5f, 0.0f, -5.0f); glVertex3f(-1.0f, 0.0f, -5.0f); glVertex3f(-1.0f, 3.0f, -5.0f); glVertex3f(-2.5f, 3.0f, -5.0f); glEnd();
    // Lintel da Porta
    glBegin(GL_QUADS); glVertex3f(-1.0f, 2.5f, -5.0f); glVertex3f( 1.0f, 2.5f, -5.0f); glVertex3f( 1.0f, 3.0f, -5.0f); glVertex3f(-1.0f, 3.0f, -5.0f); glEnd();
    // Segmento D: Direita da Porta
    glBegin(GL_QUADS); glVertex3f( 1.0f, 0.0f, -5.0f); glVertex3f( 5.0f, 0.0f, -5.0f); glVertex3f( 5.0f, 3.0f, -5.0f); glVertex3f( 1.0f, 3.0f, -5.0f); glEnd();

    // Desenha a porta
    drawDoor();

    // --- Outras paredes ---
    // Parede traseira (z = 5)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS); glVertex3f(-5.0f, 0.0f, 5.0f); glVertex3f(5.0f, 0.0f, 5.0f); glVertex3f(5.0f, 3.0f, 5.0f); glVertex3f(-5.0f, 3.0f, 5.0f); glEnd();
    // Parede esquerda (x = -5)
    glBegin(GL_QUADS); glVertex3f(-5.0f, 0.0f, -5.0f); glVertex3f(-5.0f, 0.0f, 5.0f); glVertex3f(-5.0f, 3.0f, 5.0f); glVertex3f(-5.0f, 3.0f, -5.0f); glEnd();
    // Parede direita (x = 5)
    glBegin(GL_QUADS); glVertex3f( 5.0f, 0.0f, -5.0f); glVertex3f( 5.0f, 0.0f, 5.0f); glVertex3f( 5.0f, 3.0f, 5.0f); glVertex3f( 5.0f, 3.0f, -5.0f); glEnd();
    // Teto (y = 3)
    glColor3f(0.7f, 0.7f, 0.9f);
    glBegin(GL_QUADS); glVertex3f(-5.0f, 3.0f, -5.0f); glVertex3f(5.0f, 3.0f, -5.0f); glVertex3f(5.0f, 3.0f, 5.0f); glVertex3f(-5.0f, 3.0f, 5.0f); glEnd();
}

void loadTexture(const char *filename) {
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        fprintf(stderr, "Erro abrindo %s\n", filename);
        return;
    }

    // Gera e vincula o ID de textura
    glGenTextures(1, &doorTex);
    glBindTexture(GL_TEXTURE_2D, doorTex);

    // Parâmetros de como tratar as bordas e a filtragem
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Envia os dados para a GPU e gera mipmaps (níveis de detalhe)
    if (channels == 3)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,  width, height, GL_RGB,  GL_UNSIGNED_BYTE, data);
    else
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    glutCreateWindow("Ambiente Simulado (Casa) - Colisões Melhoradas");

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND); // Blend não é estritamente necessário aqui
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupLighting(); // Configura iluminação

    glEnable(GL_TEXTURE_2D);  
    loadTexture("images/porta-minecraft-invertida.png");        

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}