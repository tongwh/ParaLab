#define shininess 20.0

varying vec3 vNormal;
varying vec3 vVertex;

void main (void)
{

vec4 cmate = gl_TexCoord[0];
vec4 csilh = gl_TexCoord[1];
vec4 cspec = gl_TexCoord[2];

// Lighting
vec3 eyePos = vec3(0.0,0.0,15.0);
vec3 lightPos = vec3(0.0,15.0,15.0);
// vec3 lightPos = gl_LightSource[0].position.xyz;
// vec3 eyePos = vec3(0.0, 5.0, -5.0);
// vec3 lightPos = vec3(0.0, -5.0, -5.0);

vec3 EyeVert = normalize(eyePos - vVertex);
vec3 LightVert = normalize(lightPos - vVertex);
vec3 EyeLight = normalize(LightVert + EyeVert);
// vec3 Normal = normalize(gl_NormalMatrix * vNormal);
vec3 Normal = normalize(vNormal);

// Simple Silhouette
float sil = max(dot(Normal,EyeVert), 0.0);
if (sil < 0.3) gl_FragColor = csilh;
else 
{
  gl_FragColor = cmate ; 

 // Specular part
 float spec = pow(max(dot(Normal,EyeLight),0.0), shininess);

// if (spec < 0.2) gl_FragColor *= 0.8;
// else gl_FragColor = cspec; 
 
gl_FragColor += cspec*smoothstep(0.1,0.4,spec);
// gl_FragColor += cspec*step(0.4,spec);
 
// gl_FragColor += spec*cspec;  // advanced 3D Toon

 // Diffuse part
 float diffuse = max(dot(Normal,LightVert),0.0);
 if (diffuse < 0.5) gl_FragColor *=0.8;
}

// gl_FragColor = cmate;
}
