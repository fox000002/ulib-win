/*
 * Copyright (c) 1999  Silicon Graphics, Inc.  All rights reserved.
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
 * shapes.h
 */

#include <GL/glut.h>

void WireSphere (GLdouble radius);
void WireCube (GLdouble size);
void WireBox (GLdouble width, GLdouble height, GLdouble depth);
void WireTorus (GLdouble innerRadius, GLdouble outerRadius);
void WireCylinder (GLdouble radius, GLdouble height);
void WireIcosahedron (GLdouble radius);
void WireOctahedron (GLdouble radius);
void WireTetrahedron (GLdouble radius);
void WireDodecahedron (GLdouble radius);
void WireCone (GLdouble base, GLdouble height);
void SolidSphere (GLdouble radius);
void SolidCube (GLdouble size);
void SolidBox (GLdouble width, GLdouble height, GLdouble depth);
void SolidTorus (GLdouble innerRadius, GLdouble outerRadius);
void SolidCylinder (GLdouble radius, GLdouble height);
void SolidIcosahedron (GLdouble radius);
void SolidOctahedron (GLdouble radius);
void SolidTetrahedron (GLdouble radius);
void SolidDodecahedron (GLdouble radius);
void SolidCone (GLdouble base, GLdouble height);
