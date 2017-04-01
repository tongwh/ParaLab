varying vec3 vNormal;
varying vec3 vVertex;

void main (void)
{

	// Lighting
	vec3 eyePos = vec3(0.0, 0.0, 15.0);
	vec3 lightPos = vec3(0.0, 15.0, 15.0);

	vec3 EyeVert = normalize(eyePos - vVertex);
	vec3 LightVert = normalize(lightPos - vVertex);
	vec3 EyeLight = normalize(LightVert + EyeVert);
	vec3 Normal = normalize(vNormal);

	// Simple contours
	float sil = dot(Normal, EyeVert);
	if (sil < 0.387 && sil > -0.387) 
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
