/* mouse.c - use the mouse to control the location of the viewpoint
 *
 *  Escape key         - exit the program 
 *  Left Mouse Button  - change incidence and azimuth angles
 *  Middle Mousebutton - change the twist angle based on
 *                       horizontal mouse movement
 *  Right Mousebutton  - zoom in and out based on vertical
 *                       mouse movement
 *  R Key              - reset viewpoint
 */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>    /* includes gl.h, glu.h */

#include <math.h>
#include <stdio.h>     /* for printf */
#include <stdlib.h>

/*  Function Prototypes  */

void  initgfx( void );
void  drawScene( void );
void  reshape( GLsizei, GLsizei );
void  keyboard( GLubyte, GLint, GLint );
void  mouse( GLint, GLint, GLint, GLint );
void  motion( GLint, GLint );

void resetView( void );
void polarView( GLfloat, GLfloat, GLfloat, GLfloat);

void printHelp( char * );

/* Global Definitions */

#define KEY_ESC     27      /* ascii value for the escape key */

/* Global Variables */

enum                actions { MOVE_EYE, TWIST_EYE, ZOOM, MOVE_NONE };
static GLint        action;

static GLdouble     xStart = 0.0, yStart = 0.0;

static GLfloat beamWidth = 2.0, beamHeight = 0.4, beamDepth = 1.0;
static GLfloat nearClip, farClip, distance, twistAngle, incAngle, azimAngle;

int main( int argc, char *argv[] )
{
    GLsizei width, height;

    glutInit( &argc, argv );

    width = glutGet( GLUT_SCREEN_WIDTH ); 
    height = glutGet( GLUT_SCREEN_HEIGHT );
    glutInitWindowPosition( (width / 2) + 4, height / 4 );
    glutInitWindowSize( (width / 2) - 4, height / 2 );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutCreateWindow( argv[0] );

    initgfx();

    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );
    glutDisplayFunc( drawScene ); 

    printHelp( argv[0] );

    glutMainLoop();
	
	return 0;
}

void
printHelp( char *progname )
{
    fprintf(stdout, "\n%s - use the mouse to control the viewpoint\n\n"
         "Axes: X - red, Y - green, Z - blue\n\n"
         "Left Mousebutton       - move eye position\n"
         "Middle Mousebutton     - change twist angle\n"
         "Right Mousebutton      - move up / down to zoom in / out\n"
         "<R> Key                - reset viewpoint\n"
         "Escape Key             - exit the program\n",
         progname);
}

void
initgfx( void )
{
    GLfloat maxObjectSize;

    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glShadeModel( GL_FLAT );
    glEnable( GL_DEPTH_TEST );

    /* Maximum size of all the objects in your scene */
    maxObjectSize = sqrt( ((2*beamWidth) * (2*beamWidth)) +
                          ((2*beamHeight) * (2*beamHeight)) +
                          (beamDepth * beamDepth) );

    /* Set up nearClip and farClip so that                  */ 
    /*    ( farClip - nearClip ) > maxObjectSize,           */
    /*    and determine the viewing distance (adjust for zooming) */
    nearClip = 1.0;
    farClip = nearClip + 8*maxObjectSize; 
    resetView();
}

void
reshape( GLsizei width, GLsizei height )
{
    GLdouble               aspect;

    glViewport( 0, 0, width, height );

    aspect = (GLdouble) width / (GLdouble) height;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, aspect, nearClip, farClip );
    glMatrixMode( GL_MODELVIEW );
}

void 
keyboard( GLubyte key, GLint x, GLint y )
{
    switch (key) {
    case 'R':
         resetView();
         glutPostRedisplay();
         break;
    case KEY_ESC:   /* Exit whenever the Escape key is pressed */
         exit(0);
    }
}

void 
mouse( GLint button, GLint state, GLint x, GLint y )
{
    static GLint buttons_down = 0;

    if (state == GLUT_DOWN) { /* most recent button down wins */
         buttons_down++;
         switch (button) {
         case GLUT_LEFT_BUTTON:
              /* in case there are only two mouse 
               * buttons, shift-left is equivalent to the 
               * middle mouse button
               */
              if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
                  action = TWIST_EYE;
              else
                  action = MOVE_EYE;
              break;
         case GLUT_MIDDLE_BUTTON:
              action = TWIST_EYE;
              break;
         case GLUT_RIGHT_BUTTON:
              action = ZOOM;
              break;
         }

         /* Update the saved mouse position */
         xStart = x;
         yStart = y;
    } else {
         if (--buttons_down == 0)
              action = MOVE_NONE;      /* no more buttons down */
    }
}

void
motion( GLint x, GLint y )
{
    switch (action) {
    case MOVE_EYE:
         /* Adjust the eye position based on the mouse position */
         azimAngle += (GLdouble) (x - xStart);
         incAngle -= (GLdouble) (y - yStart);
         break;
    case TWIST_EYE:
         /* Adjust the eye twist based on the mouse position */
         twistAngle = fmod(twistAngle+(x - xStart), 360.0);
         break;
    case ZOOM:
         /* Adjust the eye distance based on the mouse position */
         distance -= (GLdouble) (y - yStart)/10.0;
         break;
    default:
         printf("unknown action %d\n", action);
    }
    
    /* Update the stored mouse position for later use */
    xStart = x;
    yStart = y;

    glutPostRedisplay();
}

void
resetView( void )
{
    distance = nearClip + (farClip - nearClip) / 2.0;
    twistAngle = 0.0;    /* rotation of viewing volume (camera) */
    incAngle = 0.0;
    azimAngle = 0.0;
}

void
polarView( GLfloat distance, GLfloat azimuth, GLfloat incidence,
              GLfloat twist)
{
    glTranslatef( 0.0, 0.0, -distance);
    glRotatef( -twist, 0.0, 0.0, 1.0);
    glRotatef( -incidence, 1.0, 0.0, 0.0);
    glRotatef( -azimuth, 0.0, 0.0, 1.0);
}

void
XYaxes( void )
{
	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT );
	glLineWidth( 3.0f );
	glDisable( GL_LIGHTING );
	glBegin( GL_LINES );
		/* color X axis red */
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( 0.0f, 0.0f );
		glVertex2f( 1.5f, 0.0f );

		/* color Y axis green */
		glColor3f(  0.0f, 1.0f, 0.0f );
		glVertex2f( 0.0f, 0.0f );
		glVertex2f( 0.0f, 1.5f );
	glEnd();

	glBegin( GL_TRIANGLES );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( 1.5f, 0.0f );
		glVertex2f( 1.35f, -0.05f );
		glVertex2f( 1.35f, 0.05f );
	
		glColor3f(  0.0f, 1.0f, 0.0f );
		glVertex2f( 0.0f, 1.5f );
		glVertex2f( -0.05f, 1.35f );
		glVertex2f( 0.05f, 1.35f );
	glEnd();
	glPopAttrib();
}

void
XYZaxes( void )
{
	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT );
	glLineWidth( 3.0f );
	glDisable( GL_LIGHTING );
	glBegin( GL_LINES );
		/* color X axis red */
		glColor3f(  1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 1.5f, 0.0f, 0.0f );

		/* color Y axis green */
		glColor3f(  0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 1.5f, 0.0f );

		/* color Z axis blue */
		glColor3f(  0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 1.5f );
	glEnd();

	glBegin( GL_TRIANGLES );
		glColor3f(  1.0f, 0.0f, 0.0f );
		glVertex3f( 1.5f, 0.0f, 0.0f );
		glVertex3f( 1.35f, -0.05f, 0.0f );
		glVertex3f( 1.35f, 0.05f, 0.0f );
	
		glColor3f(  0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 1.5f, 0.0f );
		glVertex3f( -0.05f, 1.35f, 0.0f );
		glVertex3f( 0.05f, 1.35f, 0.0f );

		glColor3f(  0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 1.5f );
		glVertex3f( -0.05f, 0.0f, 1.35f );
		glVertex3f( 0.05f, 0.0f, 1.35f );
	glEnd();
	glPopAttrib();
}

void WireBox (GLdouble width, GLdouble height, GLdouble depth);

void
drawScene( void )
{
    static GLfloat       upperArmColor[] = { 1.0, 0.0, 0.0 };
    static GLfloat       lowerArmColor[] = { 0.8, 0.5, 0.5 };
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();
         polarView( distance, azimAngle, incAngle, twistAngle );
         XYZaxes();

         /* Draw the shoulder centered at the new origin */
         glTranslatef( 1.0, 0.0, 0.0 ); 
         glColor3fv( upperArmColor );
         WireBox( beamWidth, beamHeight, beamDepth );

         /* Draw the lower arm at the end of the upper arm and 
          * rotate it 45 degrees */
         glTranslatef( 1.0, 0.0, 0.0 );
         glRotatef( 45.0, 0.0, 0.0, 1.0 );
         glTranslatef( 1.0, 0.0, 0.0 );
         glColor3fv( lowerArmColor );
         WireBox( beamWidth, beamHeight, beamDepth );
    glPopMatrix();

    glutSwapBuffers();
}
