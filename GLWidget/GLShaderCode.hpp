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
	float err = 0.05;
	float range = max - min;
	float i = texture3D( imageTexture, gl_TexCoord[0].xyz ).r;
	vec4 colorLut = texture1D( lut, i );
	colorLut.a = opacity;
	float inormed = ( i * range ) + min;

	if( inormed > 0 - err && inormed < upper_threshold ) {
		colorLut.a = 0;
	}

	if( inormed < 0 + err && inormed > lower_threshold ) {
		colorLut.a = 0;
	}

	if( killZeros == 1 && inormed > 0 - err && inormed < 0 + err ) {
		colorLut.a = 0;
	}

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
	float err = 0.05;
	float range = max - min;
	vec4 color = texture3D( imageTexture, gl_TexCoord[0].xyz );
	color.a = opacity;
	float inormed = ( color * range ) + min;
	if( ( color.r * range ) + min > upper_threshold || ( color.r * range ) + min < lower_threshold ) {
		color.a = 0;
	}
	if( killZeros == 1 && inormed > 0 - err && inormed < 0 + err ) {
		color.a = 0;
	}

	gl_FragColor = ( color + bias / range ) * scaling;
}
								  );
