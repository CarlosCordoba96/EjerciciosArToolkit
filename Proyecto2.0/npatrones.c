#include <GL/glut.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <math.h>

// ==== Definicion de estructuras ===================================
struct TObject{
  int id;                      // Identificador del patron
  int visible;                 // Es visible el objeto?
  double width;                // Ancho del patron
  double center[2];            // Centro del patron
  double patt_trans[3][4];     // Matriz asociada al patron
  void (* drawme)(float,float,float);       // Puntero a funcion drawme
  char color;
  float colour[3];
};

struct TObject *objects = NULL;
int nobjects = 0;
double dist02;
double dist12;
void print_error (char *error) {  printf("%s\n", error); exit(0); }

// ==== addObject (Anade objeto a la lista de objetos) ==============
void addObject(char *p, double w, double c[2], void (*drawme)(float,float,float))
{
  int pattid;

  if((pattid=arLoadPatt(p)) < 0)
    print_error ("Error en carga de patron\n");

  nobjects++;
  objects = (struct TObject *)
    realloc(objects, sizeof(struct TObject)*nobjects);

  objects[nobjects-1].id = pattid;
  objects[nobjects-1].width = w;
  objects[nobjects-1].center[0] = c[0];
  objects[nobjects-1].center[1] = c[1];
  objects[nobjects-1].drawme = drawme;

}

// ==== draw****** (Dibujado especifico de cada objeto) =============
void drawteapot(float r,float g,float b) {
GLfloat material[4];
    material[0]=r;
		material[1]=g;
		material[2]=b;
    material[3]=1.0;

  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  glTranslatef(0.0, 0.0, 40.0);
  glutSolidCube(80.0);

}

void angletorgb(float h,float * r,float *g, float *b){
float s=1;
float v=1;

float h60 = h/60.0;
float h60f =floor(h60);

int num2=h60f;
float hi = num2 % 6;
float fe = h60-h60f;

float p = v*(1-s);

float q = v*(1- fe * s);
float t= v*(1 - (1-fe)*s);

	if(hi==0){
		*r=v;
		*g=t;
		*b=p;
	} else if(hi==1){
		*r=q;
		*g=v;
		*b=p;

	}else if(hi==2){
		*r=p;
		*g=v;
		*b=t;
	}else if(hi==3){
		*r=p;
		*g=q;
		*b=v;
	}else if(hi==4){
		*r=t;
		*g=p;
		*b=v;
	}else if(hi==5){
		*r=v;
		*g=p;
		*b=q;
	}
}

void drawCenter(float r,float g,float b) {
	GLfloat material[4];
    float numero=1.0;

    if (dist02>250 && dist12>250) {//las dos marcas están lejos
      material[0]=0.0;
      material[1]=0.0;
      material[2]=0.0;
    }else if (dist02<250 && dist12>250) {//la marca 0 esta mas cerca
      material[0]=objects[0].colour[0];
      material[1]=objects[0].colour[1];
      material[2]=objects[0].colour[2];
    }else if (dist02>250 && dist12<250) {//la marca 1 esta mas cerca
      material[0]=objects[1].colour[0];
      material[1]=objects[1].colour[1];
      material[2]=objects[1].colour[2];

    }else{//las dos marcas están cerca
      material[0]=fmod(objects[0].colour[0]+objects[1].colour[0],1.0);
      material[1]=fmod(objects[0].colour[1]+objects[1].colour[1],1.0);
      material[2]=fmod(objects[0].colour[2]+objects[1].colour[2],1.0);
    }


	printf("Objeto0 = %f, %f, %f\n",objects[0].colour[0],objects[0].colour[1],objects[0].colour[2]);
	printf("Objeto1 = %f, %f, %f\n",objects[1].colour[0],objects[1].colour[1],objects[1].colour[2]);
	printf("Material = %f, %f, %f\n",material[0],material[1],material[2]);
	material[3]=1.0;
  	glMaterialfv(GL_FRONT, GL_AMBIENT, material);
 	glTranslatef(0.0, 0.0, 40.0);
  	glutSolidCube(80.0);

}


/*
void drawcube(char a) {
GLfloat material[4];

	if(a<60.0){//ROjo
	  	material[0]=1.0;
		material[1]=0.0;
		material[2]=0.0;
		material[3]=1.0;
		printf("rojo " );
	}else if(a>60.1 && a<120.0){//Verde
		material[0]=0.0;
		material[1]=1.0;
		material[2]=0.0;
		material[3]=1.0;
		printf("Verde ");
	}else if(a>120.1){
		material[0]=0.0;
		material[1]=0.0;
		material[2]=1.0;
		material[3]=1.0;
		printf("Azul ");
	}
  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  glTranslatef(0.0, 0.0, 40.0);
  glutSolidCube(80.0);
  printf("cube %f\n",a);
}
*/
// ======== cleanup =================================================
static void cleanup(void) {   // Libera recursos al salir ...
  arVideoCapStop();  arVideoClose();  argCleanup();  free(objects);
  exit(0);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {

  case 0x1B: case 'Q': case 'q':
    cleanup(); break;

  }
}

// ======== draw ====================================================
void draw( void ) {
  double  gl_para[16];   // Esta matriz 4x4 es la usada por OpenGL
  GLfloat light_position[]  = {100.0,-200.0,200.0,0.0};
  int i;
  char color;
  double colour[3];
  double v[3];
  float angle=0.0, module=0.0;
  double m[3][4], m2[3][4];
  argDrawMode3D();              // Cambiamos el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Limpiamos buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);


  if (objects[0].visible && objects[2].visible) {
  arUtilMatInv(objects[0].patt_trans, m);
  arUtilMatMul(m, objects[2].patt_trans, m2);
  dist02 = sqrt(pow(m2[0][3],2)+pow(m2[1][3],2)+pow(m2[2][3],2));
  printf ("Distancia objects[0] y objects[2]= %G\n", dist02);
}else{
  dist02=999.0;
}

if (objects[1].visible && objects[2].visible) {
  arUtilMatInv(objects[1].patt_trans, m);
  arUtilMatMul(m, objects[2].patt_trans, m2);
  dist12 = sqrt(pow(m2[0][3],2)+pow(m2[1][3],2)+pow(m2[2][3],2));
  printf ("Distancia objects[0] y objects[1]= %G\n", dist12);
}else{
  dist12=999.0;
}

  for (i=0; i<nobjects; i++) {
    if (objects[i].visible) {   // Si el objeto es visible
      argConvGlpara(objects[i].patt_trans, gl_para);
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(gl_para);   // Cargamos su matriz de transf.

      glEnable(GL_LIGHTING);  glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);

      // Calculamos el angulo de rotacion de la segunda marca si modo es 0, sino se queda el mismo color
      v[0] = objects[i].patt_trans[0][0];
      v[1] = objects[i].patt_trans[1][0];
      v[2] = objects[i].patt_trans[2][0];

      module = sqrt(pow(v[0],2)+pow(v[1],2)+pow(v[2],2));
      v[0] = v[0]/module;  v[1] = v[1]/module; v[2] = v[2]/module;
      angle = acos (v[0]) * 57.2958;   // Sexagesimales! * (180/PI)
      //printf("SE ha detectado esto %f en el objeto %i\n",angle,i );
      float r;
      float g;
      float b;
      angletorgb(angle,&r,&g,&b);
      printf("r: %f g: %f b: %f con angulo: %f\n",r,g,b,angle);

      colour[0]=r;
      colour[1]=g;
      colour[2]=b;
      objects[i].colour[0]=r;
      objects[i].colour[1]=g;
      objects[i].colour[2]=b;
      objects[i].drawme(r,g,b);

    }
  }
 
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)
  double c[2] = {0.0, 0.0};  // Centro de patron (por defecto)

  // Abrimos dispositivo de video
  if(arVideoOpen("-dev=/dev/video0") < 0) exit(0);
  if(arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0)
    print_error ("Error en carga de parametros de camara\n");

  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  // Inicializamos la lista de objetos
  addObject("data/simple.patt", 90.0, c, drawteapot);
  addObject("data/identic.patt", 90.0, c, drawteapot);
  addObject("data/4x4_1.patt", 130.0, c, drawCenter);


  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num, i, j, k;

  // Capturamos un frame de la camara de video
  if((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
    // Si devuelve NULL es porque no hay un nuevo frame listo
    arUtilSleep(2);  return;  // Dormimos el hilo 2ms y salimos
  }

  argDrawMode2D();
  argDispImage(dataPtr, 0,0);    // Dibujamos lo que ve la camara

  // Detectamos la marca en el frame capturado (return -1 si error)
  if(arDetectMarker(dataPtr, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  arVideoCapNext();      // Frame pintado y analizado... A por otro!

  // Vemos donde detecta el patron con mayor fiabilidad
  for (i=0; i<nobjects; i++) {
    for(j = 0, k = -1; j < marker_num; j++) {
      if(objects[i].id == marker_info[j].id) {
	if (k == -1) k = j;
	else if(marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }

    if(k != -1) {   // Si ha detectado el patron en algun sitio...
      objects[i].visible = 1;
      arGetTransMat(&marker_info[k], objects[i].center,
		    objects[i].width, objects[i].patt_trans);
    } else { objects[i].visible = 0; }  // El objeto no es visible
  }

  draw();           // Dibujamos los objetos de la escena
  argSwapBuffers(); // Cambiamos el buffer con lo que tenga dibujado
}

// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio

  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );    // Asociamos callbacks
  return (0);
}
