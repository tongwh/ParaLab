// wwlk
//
// Do one per fragment diffuse light
// + four specular lights
// + four more specular lights
// 8 specular lights per fragment!

const float Shininess = 100.0;

void main (void)
{

	vec3 NNormal;
	vec3 LNormal;
	vec4 MyColor;
	vec4 SpecularColor;

	float Intensity;

//
//	Since the normal is getting interpolated, we
//	have to first restore it by normalizing it.
//
	NNormal = normalize( vec3(gl_TexCoord[0]) );

//
//	Per fragment diffuse lighting
//	We are going to put the diffuse light about
//	the right spot....

	Intensity = dot (vec3(gl_TexCoord[1]), NNormal );
	Intensity = max ( Intensity, 0.0 );

	MyColor =  vec4(Intensity) * gl_TexCoord[4];

//
//	We are going to point the normal to the
//	upper right front all of the time by taking
//	the abs.  No need to normalize again.
//
//	NNormal = abs(NNormal);

//
//	Now, start the specular calculations.
//

	Intensity = dot( NNormal, vec3( gl_TexCoord[1] ) );
	Intensity = max( Intensity, 0.0 );
	Intensity = pow( Intensity, Shininess );

	MyColor += vec4(Intensity) * gl_TexCoord[6];

//	Intensity = dot( NNormal, vec3( gl_TexCoord[2] ) );
//	Intensity = max( Intensity, 0.0 );
//	Intensity = pow( Intensity, Shininess );

//	MyColor += vec4(Intensity) * gl_TexCoord[7];

//
//	Squash the Y of the normal vector
//
//	NNormal.y = 0.0;

//	NNormal = normalize( NNormal );

//	Intensity = dot( NNormal, vec3( gl_TexCoord[3] ) );
//	Intensity = pow( Intensity, Shininess );

//	MyColor += vec4(Intensity) * gl_TexCoord[5];

	MyColor.a = gl_TexCoord[4].a;
	
	gl_FragColor = MyColor;

}
