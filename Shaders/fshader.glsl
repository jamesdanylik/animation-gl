#version 120 

// per-fragment interpolated values from the vertex shader
varying  vec3 fN;
varying  vec3 fL;
varying  vec3 fE;

uniform vec4  AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4  ModelView;
uniform vec4  LightPosition;
uniform float Shininess;
uniform sampler2D Tex;
uniform int   EnableTex;
uniform sampler2D BumpTex;
uniform int   EnableBumpTex;
uniform int	  EnableSkybox;
uniform int	  EnableFade;
uniform float Fade;

void main() 
{ 
	if (EnableSkybox == 1)
	{
		vec4 alpha = texture2D(Tex, vec2(gl_TexCoord[0])).aaaa;
		if (EnableFade == 1)
			if( EnableTex == 1 )
				gl_FragColor = alpha*vec4(1.0,1.0,1.0,Fade)*texture2D(Tex, vec2(gl_TexCoord[0]));
			else
				gl_FragColor = vec4(1.0,1.0,1.0,Fade)*DiffuseProduct;
		else
			gl_FragColor = alpha*texture2D(Tex, vec2(gl_TexCoord[0]));
	}
	else
	{
		// Normalize the input lighting vectors
		vec3 N = normalize(fN);

		// Bump mapping??
		if (EnableBumpTex == 1)
		{
			vec3 Normal = texture2D(BumpTex, vec2(gl_TexCoord[0])).xyz;
			Normal -= 0.5f;
			Normal *= 2.0f;
			N = normalize(Normal);
			N *= fN;
			N = normalize(N);
			//N = normalize(Normal);
		}

		vec3 E = normalize(fE);
		vec3 L = normalize(fL);

		vec3 H = normalize( L + E );
		//vec3 R = normalize(reflect(L, N));
		
		vec4 ambient = AmbientProduct;

		float Kd = max(dot(L, N), 0.0);
		vec4 diffuse = Kd * DiffuseProduct;
		
		if (EnableTex == 1)
			diffuse *= texture2D(Tex, vec2(gl_TexCoord[0]));
		
		float Ks = pow(max(dot(N, H), 0.0), Shininess);
		//float Ks = pow(max(dot(R, E), 0.0), Shininess);
		
		vec4 specular = Ks * SpecularProduct;

		// discard the specular highlight if the light's behind the vertex
		if( dot(L, N) < 0.0 ) 
		{
			specular = vec4(0.0, 0.0, 0.0, 1.0);
		}
		if ( EnableTex == 1)
		{
        	vec4 alpha = texture2D(Tex, vec2(gl_TexCoord[0])).aaaa;
        	gl_FragColor = (ambient + diffuse + specular)*alpha;
		}
		else
		{
			gl_FragColor = ambient + diffuse + specular;
    	    gl_FragColor.a = 1.0;
		}
		//gl_FragColor = ambient + diffuse + specular;
		//gl_FragColor.a = 1.0;
		//vec4 alpha = texture2D(Tex, vec2(gl_TexCoord[0])).aaaa;
		//gl_FragColor = (ambient + diffuse + specular)*alpha;
	}
} 

//
