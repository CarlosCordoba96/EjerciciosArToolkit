#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ======== definicion de constantes ================================
#define MAXPUTOS 50000    // Numero maximo de vertices del modelo
#define MAXLINEA 2000     // Numero maximo de aristas del modelo
#define MAXFRAMES 200     // Numero maximo de frames de la animacion

#define SDIRECTO  1       // Sentido de reproduccion anim. normal
#define SINVERSO -1       // Sentido de reproduccion anim. inverso

#define NOANIM    0       // Sin animacion
#define MSIMPLE   1       // Una iteracion en la reproduccion
#define MBUCLE    2       // Bucle infinito con sentido normal
#define MPINGPONG 3       // Reproduccion efecto "ping-pong"

#define EWIREFRAME 1      // Estilo de representacion alambrico
#define ESOLIDO    0     // Estilo de representacion solido

typedef struct{  // ======== OrjVertice =============================
  float x;       // Define coordenadas para un vertice del modelo
  float y;
  float z;
} OrjVertice;

typedef struct{  // ======== OrjCara ================================
  int v1;        // Indices de vertices (indexado!) y coordenadas UV
  float uv1, vv1;
  int v2; 
  float uv2, vv2;
  int v3; 
  float uv3, vv3;
} OrjCara;

typedef struct{  // ======== OrjObjeto ==============================
  // GEOMETRIA Propiedades de Geometria (vertices, caras...)
  OrjCara    caras[MAXPUTOS];         // Vector de caras
  OrjVertice vertices[MAXPUTOS];      // Vector de vertices
  int        ncaras;                  // Numero de caras 
  int        nvertices;               // Numero de vertices
  int        listmesh;                // Para usar glCallList
  float      escala;                  // Escala de dibujado
  // TEXTURA Todos los modelos tienen asociada una textura
  int        textura;                 // Identificador de Textura
  int        anchotex;                // Ancho (pixeles) Textura
  int        altotex;                 // Alto (pixeles) Textura
  // ANIMACION Propiedades de animacion
  float      anim[MAXFRAMES][16];     // Vector de matrices 4x4
  int        nframes;                 // Numero de frames anim.
  int        frame;                   // Frame actual
  int        modo;                    // Modo de animacion
  int        sentido;                 // Sentido de reproduccion
} OrjObjeto;

// ======== cargarObjeto ============================================
// obj:     Puntero a una zona de memoria reservada de tipo OrjObjeto 
// rutaorj: Ruta del fichero .orj
// rutatex: Ruta del fichero de textura .ppm de textura asociado
// modo:    Modo de reproducir la animación. Puede ser:
//          NOANIM (Sin Animación). Se ignora el valor de "sentido"
//          MSIMPLE (Simple). Reproduce una única vez la animación.
//          MBUCLE (Bucle). Bucle infinito de reproducción.
//          MPINGPONG. Como MBUCLE pero vuelve en sentido inverso
// escala:  Escala de representación uniforme.
// ==================================================================        
void cargarObjeto(OrjObjeto *obj, char *rutaorj, char *rutatex, 
		  int modo, float escala);

// ======== desplegarObjeto =========================================
// obj:     Puntero a una zona de memoria reservada de tipo OrjObjeto 
// estilo:  Estilo de representacion. Puede ser:
//          EWIREFRAME: Muestra el objeto en modo alambrico.
//          ESOLIDO: Muestra el objeto con texturas en modo solido
// ==================================================================        
void desplegarObjeto(OrjObjeto *obj, int estilo);
