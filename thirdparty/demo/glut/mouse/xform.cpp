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
 * xform.c
 */

#include <math.h>
#include <stdio.h>
#include <GL/glut.h>    /* includes gl.h, glu.h */
#include "3d.h"

#define STACKDEPTH 10

typedef struct {
    GLdouble	mat[4][4];
    GLdouble	norm[3][3];
} mat_t;

static mat_t matstack[STACKDEPTH] = {
    {{{1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0}},
    {{1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}}}
};
static int identitymat = 1;

static int mattop = 0;

void m_xformpt(GLdouble pin[3], GLdouble pout[3], 
    GLdouble nin[3], GLdouble nout[3])
{
    int	i;
    GLdouble	ptemp[3], ntemp[3];
    mat_t	*m = &matstack[mattop];

    if (identitymat) {
	for (i = 0; i < 3; i++) {
	    pout[i] = pin[i];
	    nout[i] = nin[i];
	}
	return;
    }
    for (i = 0; i < 3; i++) {
	ptemp[i] = pin[0]*m->mat[0][i] +
		   pin[1]*m->mat[1][i] +
		   pin[2]*m->mat[2][i] +
		   m->mat[3][i];
	ntemp[i] = nin[0]*m->norm[0][i] +
		   nin[1]*m->norm[1][i] +
		   nin[2]*m->norm[2][i];
    }
    for (i = 0; i < 3; i++) {
	pout[i] = ptemp[i];
	nout[i] = ntemp[i];
    }
    normalize(nout);
}

void m_xformptonly(GLdouble pin[3], GLdouble pout[3])
{
    int	i;
    GLdouble	ptemp[3];
    mat_t	*m = &matstack[mattop];

    if (identitymat) {
	for (i = 0; i < 3; i++) {
	    pout[i] = pin[i];
	}
	return;
    }
     for (i = 0; i < 3; i++) {
	ptemp[i] = pin[0]*m->mat[0][i] +
		   pin[1]*m->mat[1][i] +
		   pin[2]*m->mat[2][i] +
		   m->mat[3][i];
    }
    for (i = 0; i < 3; i++) {
	pout[i] = ptemp[i];
    }
}

void m_pushmatrix(void)
{
    if (mattop < STACKDEPTH-1) {
		matstack[mattop+1] = matstack[mattop];
		mattop++;
    } else
		error("m_pushmatrix: stack overflow\n");
}

void m_popmatrix(void)
{
    if (mattop > 0)
		mattop--;
    else
		error("m_popmatrix: stack underflow\n");
}

void m_translate(GLdouble x, GLdouble y, GLdouble z)
{
    int	i;
    mat_t	*m = &matstack[mattop];

    identitymat = 0;
    for (i = 0; i < 4; i++)
		m->mat[3][i] = x*m->mat[0][i] +
					 y*m->mat[1][i] +
					 z*m->mat[2][i] +
					 m->mat[3][i];
}

void m_scale(GLdouble x, GLdouble y, GLdouble z)
{
    int	i;
    mat_t	*m = &matstack[mattop];

    identitymat = 0;
    for (i = 0; i < 3; i++) {
		m->mat[0][i] *= x;
		m->mat[1][i] *= y;
		m->mat[2][i] *= z;
    }
    for (i = 0; i < 3; i++) {
		m->norm[0][i] /= x;
		m->norm[1][i] /= y;
		m->norm[2][i] /= z;
    }
}
