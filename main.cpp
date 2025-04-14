#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// Variáveis globais para a posição e orientação da câmera
float camX = 0.0f, camY = 1.6f, camZ = 5.0f; // Posição inicial da câmera
float camAngleY = 0.0f;  // Ângulo de rotação no eixo Y
float camAngleX = 0.0f;  // Ângulo de rotação no eixo X

// Variável para o ângulo da porta
float doorAngle = 0.0f; // Porta inicialmente fechada

bool isInside = false; // Indica se o usuário está dentro da sala

// Função para configurar a iluminação
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

void drawRefrigerator() {
    glPushMatrix();
        glColor3f(0.9f, 0.9f, 0.9f);
        // Primeiro translade para que a base fique no chão, depois escala.
        glTranslatef(0.0f, 1.5f, 0.0f); // Eleva 1.5 (metade de 3.0)
        glScalef(1.0f, 3.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
}

void drawStove() {
    glPushMatrix();
        glColor3f(0.3f, 0.3f, 0.3f);
        // Eleva para que a base fique no chão (0.4 de altura)
        glTranslatef(0.0f, 0.4f, 0.0f);
        // Aumenta a largura para 2.0 unidades, mantendo profundidade 1.0 e altura 0.8
        glScalef(2.0f, 0.8f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
}

void drawKitchenTable() {
    glPushMatrix();
        // Desenha o tampo da mesa
        glPushMatrix();
            // Eleva o tampo para 0.75 (altura do tampo)
            glTranslatef(0.0f, 0.75f, 0.0f);
            // Dimensões: 2.0 de largura, 0.1 de altura, 1.2 de profundidade
            glScalef(2.0f, 0.1f, 1.2f);
            glColor3f(0.55f, 0.27f, 0.07f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Desenha as pernas da mesa
        glColor3f(0.55f, 0.27f, 0.07f);
        // Perna frontal esquerda:
        glPushMatrix();
            glTranslatef(-0.85f, 0.375f, -0.45f);
            glScalef(0.1f, 0.75f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Perna frontal direita:
        glPushMatrix();
            glTranslatef(0.85f, 0.375f, -0.45f);
            glScalef(0.1f, 0.75f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Perna traseira esquerda:
        glPushMatrix();
            glTranslatef(-0.85f, 0.375f, 0.45f);
            glScalef(0.1f, 0.75f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Perna traseira direita:
        glPushMatrix();
            glTranslatef(0.85f, 0.375f, 0.45f);
            glScalef(0.1f, 0.75f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

void drawChair() {
    glPushMatrix();
        // Assento da cadeira
        glPushMatrix();
            glTranslatef(0.0f, 0.45f, 0.0f); // Assento a 0.45 m do chão
            glScalef(0.6f, 0.1f, 0.6f);
            glColor3f(0.5f, 0.3f, 0.0f);
            glutSolidCube(1.0);
        glPopMatrix();
        // Pernas da cadeira
        glColor3f(0.5f, 0.3f, 0.0f);
        glPushMatrix();
            glTranslatef(-0.2f, 0.225f, -0.2f);
            glScalef(0.1f, 0.45f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.2f, 0.225f, -0.2f);
            glScalef(0.1f, 0.45f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-0.2f, 0.225f, 0.2f);
            glScalef(0.1f, 0.45f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.2f, 0.225f, 0.2f);
            glScalef(0.1f, 0.45f, 0.1f);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

void drawKitchenArea() {
    // Geladeira:
    glPushMatrix();
        // Coloca a geladeira mais para o lado: (1.5,0,4.0)
        glTranslatef(1.5f, 0.0f, 4.0f);
        drawRefrigerator();
    glPopMatrix();
    
    // Fogão:
    glPushMatrix();
        // Põe o fogão no meio: (3.5,0,4.0)
        glTranslatef(3.5f, 0.0f, 4.0f);
        drawStove();
    glPopMatrix();
    
    
    // Mesa de cozinha (mais recuada, por ex. (2.0,0,2.0))
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 2.0f);
        drawKitchenTable();
    glPopMatrix();
    
    // Cadeiras ao redor da mesa
    // Cadeira frontal
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 1.0f);
        drawChair();
    glPopMatrix();
    // Cadeira traseira
    glPushMatrix();
        glTranslatef(2.0f, 0.0f, 3.0f);
        drawChair();
    glPopMatrix();
    // Cadeira esquerda
    glPushMatrix();
        glTranslatef(1.0f, 0.0f, 2.0f);
        drawChair();
    glPopMatrix();
    // Cadeira direita
    glPushMatrix();
        glTranslatef(3.0f, 0.0f, 2.0f);
        drawChair();
    glPopMatrix();
}

// Função que desenha um sofá simples no lado esquerdo da sala
void drawSofa() {
    glPushMatrix();
        // Posiciona o sofá: por exemplo, perto do canto esquerdo, um pouco deslocado da parede
        glTranslatef(-3.0f, 0.0f, -1.0f);
        
        // Desenha o tampo do sofá: um cuboide achatado
        glPushMatrix();
            glTranslatef(0.0f, 0.3f, 0.0f); // Eleva o tampo para que fique sobre o piso
            glScalef(2.0f, 0.6f, 1.0f);      // Largura, altura e profundidade
            glColor3f(0.7f, 0.2f, 0.2f);      // Cor (um tom de vermelho escuro)
            glutSolidCube(1.0);
        glPopMatrix();
        
        // Desenha o encosto do sofá
        glPushMatrix();
            // Posiciona o encosto na parte de trás do sofá
            glTranslatef(0.0f, 0.8f, 0.45f);
            glScalef(2.0f, 0.8f, 0.2f);
            glColor3f(0.7f, 0.2f, 0.2f);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}
// Desenha a mesa de centro (mesa de centro) em frente ao sofá
void drawCenterTable() {
    glPushMatrix();
        // Reposiciona a mesa de centro; por exemplo, em (-3, 0.3, -3.5)
        glTranslatef(-3.0f, 0.3f, -3.5f);
        glScalef(1.5f, 0.1f, 0.8f); // Dimensões do tampo
        glColor3f(0.6f, 0.3f, 0.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    
    // Pernas da mesa de centro
    glColor3f(0.6f, 0.3f, 0.0f);
    // Perna frontal esquerda
    glPushMatrix();
        glTranslatef(-3.0f - 0.6f, 0.15f, -3.5f - 0.35f);
        glScalef(0.1f, 0.3f, 0.1f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Perna frontal direita
    glPushMatrix();
        glTranslatef(-3.0f + 0.6f, 0.15f, -3.5f - 0.35f);
        glScalef(0.1f, 0.3f, 0.1f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Perna traseira esquerda
    glPushMatrix();
        glTranslatef(-3.0f - 0.6f, 0.15f, -3.5f + 0.35f);
        glScalef(0.1f, 0.3f, 0.1f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Perna traseira direita
    glPushMatrix();
        glTranslatef(-3.0f + 0.6f, 0.15f, -3.5f + 0.35f);
        glScalef(0.1f, 0.3f, 0.1f);
        glutSolidCube(1.0);
    glPopMatrix();
}

// Função que desenha a TV dentro do buraco da parede frontal (na parte esquerda)
void drawTVDevice() {
    glPushMatrix();
        // Posiciona a TV para que ela fique centralizada no buraco:
        // O buraco para a TV vai de x = -4.5 a -2.5, de y = 1.0 a 2.5, na parede z = -5.
        // O centro é em (-3.5, 1.75). Colocamos a TV um pouco à frente da parede (z = -4.99)
        glTranslatef(-3.5f, 1.75f, -4.99f);
        glColor3f(0.0f, 0.0f, 0.0f); // TV preta
        glBegin(GL_QUADS);
            // Desenha um quad com largura 2 e altura 1.5
            glVertex3f(-1.0f, -0.75f, 0.0f);
            glVertex3f(1.0f, -0.75f, 0.0f);
            glVertex3f(1.0f, 0.75f, 0.0f);
            glVertex3f(-1.0f, 0.75f, 0.0f);
        glEnd();
    glPopMatrix();
}

void drawDoor() {
    glPushMatrix();
    
    // Posiciona a porta: a parede frontal está em z = -5
    // Colocamos o pivô (a lateral esquerda) em (-1, 0, -5+0.01) para evitar z-fighting
    glTranslatef(-1.0f, 0.0f, -5.0f + 0.01f);
    
    // Aplica a rotação invertida para que a porta abra para dentro da sala
    glRotatef(-doorAngle * (180.0f / 3.14159f), 0.0f, 1.0f, 0.0f);
    
    // Define a cor da porta (marrom)
    glColor3f(0.4f, 0.2f, 0.0f);
    
    // Desenha a porta como um quadrilátero (largura = 2, altura = 2.5)
    glBegin(GL_QUADS);
       glVertex3f(0.0f, 0.0f, 0.0f);       
       glVertex3f(2.0f, 0.0f, 0.0f);
       glVertex3f(2.0f, 2.5f, 0.0f);
       glVertex3f(0.0f, 2.5f, 0.0f);
    glEnd();
    
    glPopMatrix();
}

// Função atualizada para desenhar a sala com recortes na parede frontal para a porta e para a TV
void drawRoom() {
    // Piso
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
       glVertex3f(-5.0f, 0.0f, -5.0f);
       glVertex3f(5.0f, 0.0f, -5.0f);
       glVertex3f(5.0f, 0.0f, 5.0f);
       glVertex3f(-5.0f, 0.0f, 5.0f);
    glEnd();
    
    // --- Parede frontal (z = -5) ---
    glColor3f(0.8f, 0.8f, 0.8f);
    
    // Segmento A: Esquerda, x de -5 a -4.5, y 0 a 3
    glBegin(GL_QUADS);
       glVertex3f(-5.0f, 0.0f, -5.0f);
       glVertex3f(-4.5f, 0.0f, -5.0f);
       glVertex3f(-4.5f, 3.0f, -5.0f);
       glVertex3f(-5.0f, 3.0f, -5.0f);
    glEnd();
    
    // Segmento B: Recorte para a TV
    // Parte inferior do recorte: x de -4.5 a -2.5, y de 0 a 1.0
    glBegin(GL_QUADS);
       glVertex3f(-4.5f, 0.0f, -5.0f);
       glVertex3f(-2.5f, 0.0f, -5.0f);
       glVertex3f(-2.5f, 1.0f, -5.0f);
       glVertex3f(-4.5f, 1.0f, -5.0f);
    glEnd();
    // Parte superior do recorte: x de -4.5 a -2.5, y de 2.5 a 3.0
    glBegin(GL_QUADS);
       glVertex3f(-4.5f, 2.5f, -5.0f);
       glVertex3f(-2.5f, 2.5f, -5.0f);
       glVertex3f(-2.5f, 3.0f, -5.0f);
       glVertex3f(-4.5f, 3.0f, -5.0f);
    glEnd();
    
    // Segmento C: Parte esquerda do muro que preenche o espaço entre o recorte da TV e a porta,
    // x de -2.5 a -1, y 0 a 3
    glBegin(GL_QUADS);
       glVertex3f(-2.5f, 0.0f, -5.0f);
       glVertex3f(-1.0f, 0.0f, -5.0f);
       glVertex3f(-1.0f, 3.0f, -5.0f);
       glVertex3f(-2.5f, 3.0f, -5.0f);
    glEnd();
    
    // Área da porta (central): x de -1 a 1.
    // Deixa o espaço da porta vazio (de y = 0 a 2.5) e desenha somente o lintel (de y = 2.5 a 3)
    glBegin(GL_QUADS);
       glVertex3f(-1.0f, 2.5f, -5.0f);
       glVertex3f(1.0f, 2.5f, -5.0f);
       glVertex3f(1.0f, 3.0f, -5.0f);
       glVertex3f(-1.0f, 3.0f, -5.0f);
    glEnd();
    
    // Segmento D: Parte direita da parede frontal, x de 1 a 5, y 0 a 3
    glBegin(GL_QUADS);
       glVertex3f(1.0f, 0.0f, -5.0f);
       glVertex3f(5.0f, 0.0f, -5.0f);
       glVertex3f(5.0f, 3.0f, -5.0f);
       glVertex3f(1.0f, 3.0f, -5.0f);
    glEnd();
    
    // Desenha a porta (no recorte central da parede)
    drawDoor();
    
    // --- Outras paredes ---
    // Parede traseira (z = 5)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
       glVertex3f(-5.0f, 0.0f, 5.0f);
       glVertex3f(5.0f, 0.0f, 5.0f);
       glVertex3f(5.0f, 3.0f, 5.0f);
       glVertex3f(-5.0f, 3.0f, 5.0f);
    glEnd();
    
    // Parede esquerda (x = -5)
    glBegin(GL_QUADS);
       glVertex3f(-5.0f, 0.0f, -5.0f);
       glVertex3f(-5.0f, 0.0f, 5.0f);
       glVertex3f(-5.0f, 3.0f, 5.0f);
       glVertex3f(-5.0f, 3.0f, -5.0f);
    glEnd();
    
    // Parede direita (x = 5) – desenhada sem recorte
    glBegin(GL_QUADS);
       glVertex3f(5.0f, 0.0f, -5.0f);
       glVertex3f(5.0f, 0.0f, 5.0f);
       glVertex3f(5.0f, 3.0f, 5.0f);
       glVertex3f(5.0f, 3.0f, -5.0f);
    glEnd();
    
    // Teto (y = 3)
    glColor3f(0.7f, 0.7f, 0.9f);
    glBegin(GL_QUADS);
       glVertex3f(-5.0f, 3.0f, -5.0f);
       glVertex3f(5.0f, 3.0f, -5.0f);
       glVertex3f(5.0f, 3.0f, 5.0f);
       glVertex3f(-5.0f, 3.0f, 5.0f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use o modo MODELVIEW e limpe a matriz
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Defina a luz fixa com a matriz identidade
    glPushMatrix();           // Salva a matriz atual
        glLoadIdentity();     // Garante que estamos na identidade
        // Defina uma luz direcional (w = 0) fixa – aqui a luz vem de cima e da frente
        GLfloat lightPosition[] = { 0.0f, 5.0f, 5.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glPopMatrix();            // Restaura a matriz
    
    // Agora aplique a transformação da câmera (gluLookAt)
    gluLookAt(camX, camY, camZ, 
        camX + sinf(camAngleY) * cosf(camAngleX), 
        camY + sinf(camAngleX), 
        camZ - cosf(camAngleY) * cosf(camAngleX), 
        0.0, 1.0, 0.0);
              
    // Desenha a cena
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

// Função para verificar colisões com paredes e objetos
bool checkCollision(float newX, float newZ) {
    // Limites da sala
    const float roomMinX = -5.0f;
    const float roomMaxX = 5.0f;
    const float roomMinZ = -5.0f;
    const float roomMaxZ = 5.0f;
    
    // Verifica se está tentando atravessar paredes
    if (newX < roomMinX || newX > roomMaxX || newZ < roomMinZ || newZ > roomMaxZ) {
        // Só permite atravessar a parede frontal (z = -5) pela porta
        if (newZ < roomMinZ) {
            // Verifica se está na área da porta (-1 < x < 1) e a porta está aberta
            if (doorAngle > 0.01f && newX > -1.0f && newX < 1.0f) {
                isInside = !isInside; // Alterna entre dentro e fora
                return false; // Permite a passagem
            }
            return true; // Colisão com parede frontal fora da porta
        }
        return true; // Colisão com outras paredes
    }
    
    // Verifica colisão com objetos (implementação básica)
    if (isInside) {
        // Geladeira
        if (newX > 1.0f && newX < 2.0f && newZ > 3.5f && newZ < 4.5f) return true;
        // Fogão
        if (newX > 3.0f && newX < 4.0f && newZ > 3.5f && newZ < 4.5f) return true;
        // Mesa de cozinha
        if (newX > 1.0f && newX < 3.0f && newZ > 1.5f && newZ < 2.5f) return true;
        // Sofá
        if (newX > -4.0f && newX < -2.0f && newZ > -1.5f && newZ < -0.5f) return true;
        // Mesa de centro
        if (newX > -3.75f && newX < -2.25f && newZ > -3.9f && newZ < -3.1f) return true;
    }
    
    return false; // Sem colisão
}

void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.2f;
    float newX = camX;
    float newZ = camZ;
    
    switch (key) {
        case 'w':
            newX += sinf(camAngleY) * moveSpeed;
            newZ += -cosf(camAngleY) * moveSpeed;
            break;
        case 's':
            newX -= sinf(camAngleY) * moveSpeed;
            newZ -= -cosf(camAngleY) * moveSpeed;
            break;
        case 'a':
            newX -= cosf(camAngleY) * moveSpeed;
            newZ -= sinf(camAngleY) * moveSpeed;
            break;
        case 'd':
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
    
    // Verifica colisão antes de atualizar a posição
    if (!checkCollision(newX, newZ)) {
        camX = newX;
        camZ = newZ;
    }
    
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float angleSpeed = 0.1f;
    switch(key) {
        case GLUT_KEY_LEFT:
            camAngleY -= angleSpeed;
            break;
        case GLUT_KEY_RIGHT:
            camAngleY += angleSpeed;
            break;
        case GLUT_KEY_UP:       // Olhar para cima
            camAngleX += angleSpeed;
            break;
        case GLUT_KEY_DOWN:      // Olhar para baixo
            camAngleX -= angleSpeed;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ambiente Simulado (Casa) - Base");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Configura a iluminação
    setupLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}