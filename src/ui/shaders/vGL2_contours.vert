varying vec3 vNormal;
varying vec3 vVertex;

void main(void)
{
	vVertex = gl_Vertex.xyz;
	vNormal = gl_NormalMatrix * gl_Normal;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
