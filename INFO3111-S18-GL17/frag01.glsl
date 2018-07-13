varying vec3 color;
void main()
{
    gl_FragColor = vec4(color, 1.0);
	
	gl_FragColor.rgb *= 0.0001f;		// Almost zero
	
	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );
};
