# Ambiente Simulado (Casa)

## Visão Geral
Este projeto em C++ usa OpenGL (GLUT) e stb_image para criar uma cena 3D interativa que simula o interior de uma casa. A aplicação desenha piso, teto, paredes, móveis (sofá, mesa, cadeiras, geladeira, fogão, mesa de centro, TV) e uma porta que pode ser aberta/fechada. Há também detecção de colisão e iluminação básica.

---

## Funcionalidades
- **Navegação livre**: mover-se com W/A/S/D e olhar com as setas.  
- **Porta interativa**: abre e fecha com a tecla `O`, sem atravessar a parede.  
- **Detecção de colisão (AABB)**: impede que a câmera atravesse paredes e móveis.  
- **Texturização**: todas as superfícies usam texturas carregadas via stb_image.  
- **Iluminação básica**: luz ambiente, difusa e especular via GL_LIGHT0.  

---

## Pré-requisitos
- **GLUT** 
- **OpenGL**
- **stb_image**  
- Compilador C++ compatível

---

## Como Compilar

### Linux / macOS
```bash
g++ main.cpp -o ambiente -lGL -lGLU -lglut

./ambiente
