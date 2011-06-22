#define STRINGIFY(A) #A
#include <string>

std::string colormap_shader_code = STRINGIFY(
									   uniform sampler3D imageTexture;
									   uniform sampler1D lut;
									   uniform float min;
									   uniform float max;
									   uniform float upper_threshold;
									   uniform float lower_threshold;
									   uniform float bias;
									   uniform float scaling;
									   uniform float opacity;
									   uniform float killZeros;
									   void main ()
{
	
	float range = max - min;
	float err = 0.02 * range;
	float i = texture3D( imageTexture, gl_TexCoord[0].xyz ).r;
	vec4 colorLut = texture1D( lut, i );
	float inormed = ( i * range ) + min;
	
	//ok, since we have to avoid if statements we have to do it that way...
	
	bool ut = !( inormed < upper_threshold);
	bool az = ( inormed > 0.0 );
	bool lt = !( inormed > lower_threshold);
	bool bz = (inormed < 0.0 );
	bool inz1 = !(inormed > 0.0 - err);
	bool inz2 = !(inormed < 0.0 + err);
	
	//and since we can not be sure the GLSL version supports bitwise operators we have to cheat a little and simulate them
	//this exactly the same expression as:
	//colorLut.a = float((ut & az & (inz1 ^ inz2)) ^ (lt & bz & (inz1 ^ inz2))) * opacity;
	colorLut.a = (((float(ut) * float(az)) * (float(inz1)+float(inz2))) + ((float(lt) * float(bz)) * (float(inz1) + float(inz2)))) * opacity;
	

	gl_FragColor = ( colorLut + bias / range ) * scaling;
}
								   );

std::string scaling_shader_code = STRINGIFY(
									  uniform sampler3D imageTexture;
									  uniform float min;
									  uniform float max;
									  uniform float upper_threshold;
									  uniform float lower_threshold;
									  uniform float scaling;
									  uniform float bias;
									  uniform float opacity;
									  uniform float killZeros;
									  void main()
{
	float range = max - min;
	float err = 0.001 * range;
	vec4 color = texture3D( imageTexture, gl_TexCoord[0].xyz );
	color.a = opacity;
	float inormed = ( color.r * range ) + min;
	bool inz1 = !(inormed > 0.0 - err);
	bool inz2 = !(inormed < 0.0 + err);
	
	color.a = float(inz1) + float(inz2) * opacity;
	
	gl_FragColor = ( color + bias / range ) * scaling;
}
								  );
