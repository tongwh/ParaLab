varying vec3 vNormal;
varying vec3 vVertex;

uniform vec4 cmate;
uniform vec4 csilh;
uniform vec4 cspec;

void main(void)
{
  vVertex = gl_Vertex.xyz;
//  vNormal = gl_Normal;
  vNormal = gl_NormalMatrix * gl_Normal;

  gl_TexCoord[0] = cmate;
  gl_TexCoord[1] = csilh;
  gl_TexCoord[2] = cspec;

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
