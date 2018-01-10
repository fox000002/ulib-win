/* Demonstrates how to load PLY files
* Needs some refactoring.
*
* Model needs to be triangulated
* Use blender
*
* Just the class for loading PLY files.
*
*/

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

class Model_PLY
{
public:
    int Load(char *filename);
    void Draw();
    float* CalculateNormal( float *coord1, float *coord2, float *coord3 );

    Model_PLY();
    ~Model_PLY();

    float* Faces_Triangles;
    float* Faces_Quads;
    float* Vertex_Buffer;
    float* Normals;

    int TotalConnectedTriangles;
    int TotalConnectedQuads;
    int TotalConnectedPoints;
    int TotalFaces;

    float norm[3];

};



Model_PLY::Model_PLY()
{
    Faces_Triangles = NULL;
    Faces_Quads = NULL;
    Vertex_Buffer = NULL;
    Normals = NULL;
}

Model_PLY::~Model_PLY()
{
    if (Faces_Triangles) free(Faces_Triangles);
    if (Faces_Quads) free(Faces_Quads);
    if (Vertex_Buffer) free(Vertex_Buffer);
    if (Normals) free(Normals);
}


float* Model_PLY::CalculateNormal( float *coord1, float *coord2, float *coord3 )
{
    /* calculate Vector1 and Vector2 */
    float va[3], vb[3], vr[3], val;
    va[0] = coord1[0] - coord2[0];
    va[1] = coord1[1] - coord2[1];
    va[2] = coord1[2] - coord2[2];

    vb[0] = coord1[0] - coord3[0];
    vb[1] = coord1[1] - coord3[1];
    vb[2] = coord1[2] - coord3[2];

    /* cross product */
    vr[0] = va[1] * vb[2] - vb[1] * va[2];
    vr[1] = vb[0] * va[2] - va[0] * vb[2];
    vr[2] = va[0] * vb[1] - vb[0] * va[1];

    /* normalization factor */
    val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );


    norm[0] = vr[0]/val;
    norm[1] = vr[1]/val;
    norm[2] = vr[2]/val;


    return norm;
}



int Model_PLY::Load(char* filename)
{
    this->TotalConnectedTriangles = 0;
    this->TotalConnectedQuads = 0;
    this->TotalConnectedPoints = 0;

    char* pch = strstr(filename,".ply");

    if (pch != NULL)
    {
        FILE* file = fopen(filename,"r");

        fseek(file,0,SEEK_END);
        long fileSize = ftell(file);

        try
        {
            Vertex_Buffer = (float*) malloc (ftell(file));
        }
        catch (char* )
        {
            return -1;
        }


        if (Vertex_Buffer == NULL) return -1;

        fseek(file,0,SEEK_SET);

        Faces_Triangles = (float*) malloc(fileSize*sizeof(float));
        Normals  = (float*) malloc(fileSize*sizeof(float));

        if (file)
        {
            int i = 0;
            int temp = 0;
            int quads_index = 0;
            int triangle_index = 0;
            int normal_index = 0;
            char buffer[1000];


            fgets(buffer,300,file);            // ply
            printf("%s\n", buffer);

            // READ HEADER
            // -----------------

            // Find number of vertexes
            while (strncmp( "element vertex", buffer,strlen("element vertex")) != 0  )
            {
                fgets(buffer,300,file);            // format
                printf("%s\n", buffer);
            }
            strcpy(buffer, buffer+strlen("element vertex"));
            sscanf(buffer, "%i", &TotalConnectedPoints);

            printf("Total Connected Points : %d\n", TotalConnectedPoints);

            // Find number of vertexes
            fseek(file,0,SEEK_SET);
            while (  strncmp( "element face", buffer,strlen("element face")) != 0  )
            {
                fgets(buffer,300,file);            // format
                printf("%s\n", buffer);
            }
            strcpy(buffer, buffer+strlen("element face"));
            sscanf(buffer,"%i", &TotalFaces);

            printf("Total faces : %d\n", TotalFaces);

            // go to end_header
            while (  strncmp( "end_header", buffer,strlen("end_header")) != 0  )
            {
                fgets(buffer,300,file);            // format
                printf("%s\n", buffer);
            }

            //----------------------

            // read verteces
            i =0;
            for (int iterator = 0; iterator < TotalConnectedPoints; iterator++)
            {
                fgets(buffer,300,file);

                sscanf(buffer,"%f %f %f", &Vertex_Buffer[i], &Vertex_Buffer[i+1], &Vertex_Buffer[i+2]);
                i += 3;
            }

            // read faces
            i =0;
            for (int iterator = 0; iterator < TotalFaces; iterator++)
            {
                fgets(buffer,300,file);

                if (buffer[0] == '3')
                {
                    int vertex1 = 0, vertex2 = 0, vertex3 = 0;
                    //sscanf(buffer,"%i%i%i\n", vertex1,vertex2,vertex3 );
                    buffer[0] = ' ';
                    sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );
                    /*vertex1 -= 1;
                        vertex2 -= 1;
                        vertex3 -= 1;
*/
                    //  vertex == punt van vertex lijst
                    // vertex_buffer -> xyz xyz xyz xyz
                    printf("%f %f %f ", Vertex_Buffer[3*vertex1], Vertex_Buffer[3*vertex1+1], Vertex_Buffer[3*vertex1+2]);

                    Faces_Triangles[triangle_index] = Vertex_Buffer[3*vertex1];
                    Faces_Triangles[triangle_index+1] = Vertex_Buffer[3*vertex1+1];
                    Faces_Triangles[triangle_index+2] = Vertex_Buffer[3*vertex1+2];
                    Faces_Triangles[triangle_index+3] = Vertex_Buffer[3*vertex2];
                    Faces_Triangles[triangle_index+4] = Vertex_Buffer[3*vertex2+1];
                    Faces_Triangles[triangle_index+5] = Vertex_Buffer[3*vertex2+2];
                    Faces_Triangles[triangle_index+6] = Vertex_Buffer[3*vertex3];
                    Faces_Triangles[triangle_index+7] = Vertex_Buffer[3*vertex3+1];
                    Faces_Triangles[triangle_index+8] = Vertex_Buffer[3*vertex3+2];

                    float coord1[3] = {
                        Faces_Triangles[triangle_index],
                        Faces_Triangles[triangle_index+1],
                        Faces_Triangles[triangle_index+2]
                    };

                    float coord2[3] = {
                        Faces_Triangles[triangle_index+3],
                        Faces_Triangles[triangle_index+4],
                        Faces_Triangles[triangle_index+5]
                    };

                    float coord3[3] = {
                        Faces_Triangles[triangle_index+6],
                        Faces_Triangles[triangle_index+7],
                        Faces_Triangles[triangle_index+8]
                    };

                    float *norm = CalculateNormal( coord1, coord2, coord3 );

                    Normals[normal_index] = norm[0];
                    Normals[normal_index+1] = norm[1];
                    Normals[normal_index+2] = norm[2];
                    Normals[normal_index+3] = norm[0];
                    Normals[normal_index+4] = norm[1];
                    Normals[normal_index+5] = norm[2];
                    Normals[normal_index+6] = norm[0];
                    Normals[normal_index+7] = norm[1];
                    Normals[normal_index+8] = norm[2];

                    normal_index += 9;

                    triangle_index += 9;

                    TotalConnectedTriangles += 3;
                }


                i += 3;
            }


            fclose(file);
        }
        else
        {
            printf("File can't be opened\n");
        }
    }
    else
    {
        printf("File does not have a .PLY extension. ");
    }
    return 0;
}

void Model_PLY::Draw()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,GL_FLOAT,    0,Faces_Triangles);
    glNormalPointer(GL_FLOAT, 0, Normals);
    glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

Model_PLY model;

// Mouse interface
int LeftButtonDown = 0;
int MiddleButtonDown = 0;
int RightButtonDown = 0;
int OldX,OldY,NewX,NewY;

/*
These values are changed in the motion() callback, and they
are then used to set up viewing and some modeling parameters.
*/
float Elevation=0;
float Azimuth=0;
float Dist = 8;

// Window dimensions
int ww = 400, wh = 400;



void myinit(void)
{ /* select clearing (background) color   */
    //glClearColor(0.5, 0.5, 0.5, 0.0);     // These RGB values make gray
    glEnable(GL_DEPTH_TEST);
    /* initialize viewing values */
    glMatrixMode(GL_PROJECTION);            // Select Matrix Mode
    glLoadIdentity();                // Provide Base Matrix
    glOrtho(0.0, 2000.0, 0.0, 1500.0, -2000.0, 2000.0);       // Set window dimensions
    // Parameters:  glOrtho(Left, Right, Bottom, Top, Front, Back)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myDisplay(void)
{ /* clear all pixels */


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set lookat point
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0,0, 8, 0, 0, 0, 0,1,0);

    glRotatef(Elevation,1,0,0);
    glRotatef(Azimuth,0,1,0);
	
	glTranslatef(0.0,0.0,-Dist);

    //glColor3f(1.0f, 0.0f, 1.0f);

    model.Draw();            // Call my Drawing Routine

    glFlush();  // Force writing all pending OpenGL actions to the screen.

    glutSwapBuffers();
}

void resize (int w, int h)
{
    ww = w;
    wh = h;

    // Prevent a divide by zero
    if(h == 0)
    h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    // Set viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(30, (float)w/h, 0.5f, 20);

    // Modelview matrix reset
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse (int button, int state, int x, int y)
{
    NewX = x;
    NewY = y;

    // Left mouse button pressed
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        LeftButtonDown = 1;
        RightButtonDown = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
    }
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		 MiddleButtonDown = 1;
	}
    else 
    {
        RightButtonDown = LeftButtonDown = MiddleButtonDown = 0;
    }
}

void motion (int x, int y)
{
    float RelX, RelY;

    OldX = NewX;
    OldY = NewY;
    NewX = x;
    NewY = y;

    RelX = (NewX-OldX) / (float)glutGet((GLenum)GLUT_WINDOW_WIDTH);
    RelY = (NewY-OldY) / (float)glutGet((GLenum)GLUT_WINDOW_HEIGHT);

    if (MiddleButtonDown )
    {
        // Change distance
        Dist += 0.1;

    }
	else if (LeftButtonDown)
    {
        // Change Rotation
        Azimuth += (RelX*180);
        Elevation += (RelY*180);
    }
	else if (RightButtonDown)
	{
		
	}

    glutPostRedisplay();
}

void keyboard (unsigned char c, int x, int y)
{
    switch (c)
    {
    case 27:
    case 'q':
        exit(0);
        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <PLY file>\n", argv[0]);
        return -1;
    }
    model.Load(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glutCreateWindow("PLY Model");

    glutDisplayFunc(myDisplay);

    //glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    myinit();

    glutMainLoop();

    return 0;
}
