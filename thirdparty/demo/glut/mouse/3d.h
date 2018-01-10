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
 * 3d.h 
 */

void	error(char *);
void	diff3(GLdouble [3], GLdouble [3], GLdouble [3]);
void	add3(GLdouble [3], GLdouble [3], GLdouble [3]);
void	scalarmult(GLdouble, GLdouble [3], GLdouble [3]);
GLdouble	dot3(GLdouble [3], GLdouble [3]);
GLdouble	length3(GLdouble [3]);
GLdouble	dist3(GLdouble [3], GLdouble [3]);
void	copy3(GLdouble [3], GLdouble [3]);
void	crossprod(GLdouble [3], GLdouble [3], GLdouble [3]);
void	normalize(GLdouble [3]);
void	print3(GLdouble [3]);
void	printmat3(GLdouble [3][3]);
void	identifymat3(GLdouble [3][3]);
void	copymat3(GLdouble *, GLdouble *);
void	xformvec3(GLdouble [3], GLdouble [3][3], GLdouble [3]);

void m_resetmatrixstack();
void m_xformpt(GLdouble [3], GLdouble [3], GLdouble [3], GLdouble [3]);
void m_xformptonly(GLdouble [3], GLdouble [3]);
void m_pushmatrix();
void m_popmatrix();
void m_shear(GLdouble, GLdouble, GLdouble);
void m_translate(GLdouble, GLdouble, GLdouble);
void m_scale(GLdouble, GLdouble, GLdouble);
void m_rotate(GLdouble, char);
