/*
 * Copyright (c) 1993, 1996-1999  Silicon Graphics, Inc.  All rights reserved.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE
 * POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * error.c
 */

#include <stdio.h>
#include <GL/glut.h>    /* includes gl.h, glu.h */

#include "checkerror.h"

/* This function can be called to check to see if an OpenGL error
 * has occurred. The parameter to the function is a string that 
 * can be used to provide information about where the function
 * was invoked.  The programmer will typically call this function
 * at the end of the rendering function, although it can be useful
 * to call it in any function that is under development.  
 * 
 * Note that more than one error flag might have been set, so
 * we loop until we have gotten all of them.
 */
void 
checkError( char *label ) 
{
	GLenum error;

	while ((error = glGetError()) != GL_NO_ERROR ) {
		fprintf( stderr, "%s: %s\n", label, gluErrorString(error));
	}
}
