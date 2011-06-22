#define STRINGIFY(A) #A
#include <string>
#include <complex>

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
	float i = texture3D( imageTexture, gl_TexCoord[0].xyz ).r ; // get our intensity of the origin texture (scaled 0.0 ... 1.0)
	float inormed = ( i * range ) + min; // since i goes 0.0 ... 1.0 we have to caclulate it back to its origin range
	bool az = ( inormed > 0.0 ); // we are above 0
	bool bz = (inormed < 0.0 ); // we are below 0
	float abs_max = abs(max);
	float abs_min = abs(min);
	//since we can not be certain that abs(min) == abs(max) and so i = 0.5 == inormed = 0.0 we have to scale 
	i = float(az) * (((i - (abs_min/range)) * (0.5 / (abs_max/range))) + 0.5 - 1e-6) + float(bz) * (i * (0.5 / (abs_min/range))) ;
		
	// if the user modifies the lower or upper threshold we always want the same range of the colortable
	float iscaled = (float(az) * (i - (upper_threshold / (abs_max)) ) * (abs_max/(abs_max-upper_threshold))) + (float(bz) * (i - (lower_threshold / abs_min)) * (abs_min/(abs_min-lower_threshold)));
	vec4 colorLut = texture1D( lut, iscaled );
	
	//ok, since we have to avoid if-statements we have to do it that way...
	
	bool ut = !( inormed < upper_threshold);
	bool lt = !( inormed > lower_threshold);
	bool inz1 = !(inormed > 0.0 - err);
	bool inz2 = !(inormed < 0.0 + err);
	
	//and since we can not be sure the GLSL version supports bitwise operators we have to cheat a little and simulate them
	//this exactly the same expression as:
	//colorLut.a = float((ut & az & (inz1 ^ inz2)) ^ (lt & bz & (inz1 ^ inz2))) * opacity;
	colorLut.a = (((float(ut) * float(az)) * (float(inz1)+float(inz2))) + ((float(lt) * float(bz)) * (float(inz1) + float(inz2)))) * opacity;
	
	//gl_FragColor = ( colorLut + bias / range ) * scaling;
	gl_FragColor = colorLut;
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
