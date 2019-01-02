#include "orej.h"
#include <AR/ar.h>    

// ======== getNewTime y resetTime ==================================
// Funciones de temporizacion (única parte dependiente de ARToolKit)
double getNewTime() { return (arUtilTimer()); }
void resetTime() {  arUtilTimerReset(); }

// ======== cargarTextura ===========================================
void cargarTextura(OrjObjeto *obj, char *ruta){
  GLubyte *imagen;
  FILE *fich;
  char buf[256];
  int i, j, c, w, h;

  if(obj==NULL) return;

  if(!(fich = fopen (ruta, "r"))){
    printf("no se encuentra el archivo %s\n", ruta);
    exit(-1);
  }

  fgets (buf, 255, fich);  fgets (buf, 255, fich);  
  fscanf (fich, "%d %d\n", &obj->anchotex, &obj->altotex);
  fgets (buf, 255, fich); // Saltamos la linea del limite
  imagen = malloc (sizeof(GLubyte)*obj->anchotex*obj->altotex*3);
  
  for (i=0, w=obj->anchotex, h=obj->altotex; i<(w*h*3); i++) 
    imagen[(w*h*3)-(w*3*((i/(w*3))+1))+(i%(w*3))] = 
      (GLubyte) atoi(fgets(buf, 255, fich));
  
  glGenTextures(1, &(obj->textura));
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glBindTexture(GL_TEXTURE_2D, obj->textura);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, imagen);

  fclose (fich);
  free(imagen);
}

// ======== cargarObjeto ============================================
void cargarObjeto(OrjObjeto *obj, char *rutaorj, char *rutatex, 
		  int modo, float escala)
{
  int   i=0, linea=0;
  char  cadaux[MAXLINEA];
  float a,b,c,d,e,f;
  FILE  *fich;
  float m[16];
  
  resetTime();    

  obj->nframes = 0;         // Inicializamos algunos parametros 
  obj->frame = 0;
  obj->modo = modo;
  obj->sentido = SDIRECTO;
  obj->escala = escala;

  if(!(fich=fopen(rutaorj, "r")))
    { printf("Error: %s\n", rutaorj);  exit(-1); }
  
  // Parseamos el fichero de entrada y guardamos los datos en "obj"
  while (fgets(cadaux, MAXLINEA, fich)!=NULL) {
    linea++;
    switch(cadaux[0]){
    case '#': break;
    case 'v': sscanf(&cadaux[2], "%f %f %f", 
		     &obj->vertices[obj->nvertices].x,
		     &obj->vertices[obj->nvertices].y,
		     &obj->vertices[obj->nvertices].z);
      obj->nvertices++;
      break;
      
    case 'f': a = b = c = -1;
      sscanf(&cadaux[2], "%f %f %f",  &a, &b, &c);
      obj->caras[obj->ncaras].v1 = (int) --a;
      obj->caras[obj->ncaras].v2 = (int) --b;
      obj->caras[obj->ncaras].v3 = (int) --c;
      obj->ncaras++;
      break;

    case 't': a = b = c = d = e = f = -1;
      if(i>obj->ncaras) {
	printf("Error formato en linea: '%d'\n", linea);
	printf("valor uv no asociado a ninguna cara\n");
	exit(-1);
      }
      sscanf(&cadaux[2], "%f %f %f %f %f %f", &a,&b,&c,&d,&e,&f);
      obj->caras[i].uv1 = a;   obj->caras[i].vv1 = b; 
      obj->caras[i].uv2 = c;   obj->caras[i].vv2 = d;      
      obj->caras[i].uv3 = e;   obj->caras[i].vv3 = f;
      i++;
      break;
     case 'm':  
       sscanf(&cadaux[2],
	      "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
	      &m[0],&m[1],&m[2],&m[3],&m[4],&m[5],&m[6],&m[7],&m[8],
	      &m[9],&m[10],&m[11],&m[12],&m[13],&m[14],&m[15]); 
       memcpy (&obj->anim[obj->nframes], m, sizeof(float)*16);
       obj->nframes++;
       break;
    }
  }
  fclose (fich);

  cargarTextura (obj, rutatex);   // Asociar la textura al objeto

  obj->listmesh=glGenLists(1);    // Utilizamos listas de despliegue
  glNewList(obj->listmesh,GL_COMPILE);
  // Para cada triangulo cargamos su coordenada de UVMapping y sus
  // coordenadas del Sistema de Referencia Local. 
  for (i=0; i<obj->ncaras; i++) {
    glBegin(GL_TRIANGLES);  
    glTexCoord2f(obj->caras[i].uv3, obj->caras[i].vv3);
    glVertex3f(obj->vertices[obj->caras[i].v1].x,
	       obj->vertices[obj->caras[i].v1].y,
	       obj->vertices[obj->caras[i].v1].z);
    glTexCoord2f(obj->caras[i].uv2, obj->caras[i].vv2);
    glVertex3f(obj->vertices[obj->caras[i].v2].x,
	       obj->vertices[obj->caras[i].v2].y,
	       obj->vertices[obj->caras[i].v2].z);
    glTexCoord2f(obj->caras[i].uv1, obj->caras[i].vv1);
    glVertex3f(obj->vertices[obj->caras[i].v3].x,
	       obj->vertices[obj->caras[i].v3].y,
	       obj->vertices[obj->caras[i].v3].z);
    glEnd(); 
  }
  glEndList();
}

// ======== desplegarObjeto =========================================
void desplegarObjeto(OrjObjeto *obj, int estilo){
  long i;

  glPushMatrix();  

  glScalef (obj->escala, obj->escala, obj->escala);   // ¿Escala?

  // Si tiene animacion asociada, cambiamos el frame a reproducir
  if (obj->modo != NOANIM) {
    glMultMatrixf(obj->anim[obj->frame]);

    if (getNewTime() > (1/25.0)) {   // Consideramos 25FPS siempre!
      obj->frame += obj->sentido;
      resetTime(); 
    }

    if (obj->frame == obj->nframes) {
      switch (obj->modo) {
      case MPINGPONG: obj->sentido *= -1; obj->frame -= 2; break;
      case MSIMPLE: obj->frame--; break;
      case MBUCLE: obj->frame = 0; break;
      }
    }
    if (obj->frame == -1) {  // Solo posible en modo PINGPONG
      obj->frame = 1;  obj->sentido *= -1;
    }    
  }

  // Finalmente llamamos a la lista de despliegue estableciendo
  //  las propiedades del estilo de dibujado.
  if (estilo == EWIREFRAME) {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.8, 0.2);
    glCallList(obj->listmesh);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1);
    glColor3f(1.0, 1.0, 1.0);
    glCallList(obj->listmesh);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  if (estilo == ESOLIDO) {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, obj->textura);
    glPolygonMode(GL_FRONT, GL_FILL);
    glCallList(obj->listmesh);
    glDisable(GL_TEXTURE_2D);
  }

  glPopMatrix();
}
