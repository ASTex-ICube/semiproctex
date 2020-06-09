////////////////////////////////////////////////////////////////////////////////
//
// FRAGMENT SHADER
//
// Front to back peeling
// - This shader program is used render a mesh
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// VERSION
////////////////////////////////////////////////////////////////////////////////

#version 460

////////////////////////////////////////////////////////////////////////////////
// INPUT
////////////////////////////////////////////////////////////////////////////////

in vec3 vPosition;
in vec2 vTextureCoordinate;
in vec3 vNormal;

////////////////////////////////////////////////////////////////////////////////
// UNIFORM
////////////////////////////////////////////////////////////////////////////////

// Mesh color (including alpha)
//uniform vec3 uMeshColor;

uniform sampler2D uPPTBFTexture;
uniform float hmax;

// Color map
//uniform sampler2D uColormapTex; // TODO: use texture instead?
uniform int uColormapIndex;

////////////////////////////////////////////////////////////////////////////////
// OUTPUT
////////////////////////////////////////////////////////////////////////////////

// Fragment color
layout (location = 0) out vec4 oFragmentColor;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * underover
 *
 * if edge0 < x <= edge1, return 1.0, otherwise return 0
 ******************************************************************************/
float segment(float edge0, float edge1, float x)
{
    return step(edge0,x) * (1.0-step(edge1,x));
}

/******************************************************************************
 * underover
 *
 * return under if t < 0, over if t > 1, color else
 ******************************************************************************/
vec3 underover(float t, vec3 color, vec3 under, vec3 over)
{
    return step(t,0.0)*under + segment(0.0,1.0,t)*color + step(1.0,t)*over;
}

/******************************************************************************
 * gray
 ******************************************************************************/
vec3 gray(float t)
{
    return vec3(t);
}

/******************************************************************************
 * gray
 ******************************************************************************/
vec3 gray(float t, vec3 under, vec3 over)
{
    return underover(t, gray(t), under, over);
}

/******************************************************************************
 * hot
 ******************************************************************************/
vec3 hot(float t)
{
    return vec3(smoothstep(0.00,0.33,t),
                smoothstep(0.33,0.66,t),
                smoothstep(0.66,1.00,t));
}

/******************************************************************************
 * hot
 ******************************************************************************/
vec3 hot(float t, vec3 under, vec3 over)
{
    return underover(t, hot(t), under, over);
}

/******************************************************************************
 * cool
 ******************************************************************************/
vec3 cool(float t)
{
    return mix( vec3(0.0,1.0,1.0), vec3(1.0,0.0,1.0), t);
}

/******************************************************************************
 * cool
 ******************************************************************************/
vec3 cool(float t, vec3 under, vec3 over)
{
    return underover(t, cool(t), under, over);
}

/******************************************************************************
 * autumn
 ******************************************************************************/
vec3 autumn(float t)
{
    return mix( vec3(1.0,0.0,0.0), vec3(1.0,1.0,0.0), t);
}

/******************************************************************************
 * autumn
 ******************************************************************************/
vec3 autumn(float t, vec3 under, vec3 over)
{
    return underover(t, autumn(t), under, over);
}

/******************************************************************************
 * winter
 ******************************************************************************/
vec3 winter(float t)
{
    return mix( vec3(0.0,0.0,1.0), vec3(0.0,1.0,0.5), sqrt(t));
}

/******************************************************************************
 * winter
 ******************************************************************************/
vec3 winter(float t, vec3 under, vec3 over)
{
    return underover(t, winter(t), under, over);
}

/******************************************************************************
 * spring
 ******************************************************************************/
vec3 spring(float t)
{
    return mix( vec3(1.0,0.0,1.0), vec3(1.0,1.0,0.0), t);
}

/******************************************************************************
 * spring
 ******************************************************************************/
vec3 spring(float t, vec3 under, vec3 over)
{
    return underover(t, spring(t), under, over);
}

/******************************************************************************
 * summer
 ******************************************************************************/
vec3 summer(float t)
{
    return mix( vec3(0.0,0.5,0.4), vec3(1.0,1.0,0.4), t);
}

/******************************************************************************
 * summer
 ******************************************************************************/
vec3 summer(float t, vec3 under, vec3 over)
{
    return underover(t, summer(t), under, over);
}

/******************************************************************************
 * ice
 ******************************************************************************/
vec3 ice(float t)
{
   return vec3(t, t, 1.0);
}

/******************************************************************************
 * ice
 ******************************************************************************/
vec3 ice(float t, vec3 under, vec3 over)
{
    return underover(t, ice(t), under, over);
}

/******************************************************************************
 * fire
 ******************************************************************************/
vec3 fire(float t)
{
    return mix( mix(vec3(1,1,1), vec3(1,1,0), t),
                mix(vec3(1,1,0), vec3(1,0,0), t*t), t);
}

/******************************************************************************
 * fire
 ******************************************************************************/
vec3 fire(float t, vec3 under, vec3 over)
{
    return underover(t, fire(t), under, over);
}

/******************************************************************************
 * ice_and_fire
 ******************************************************************************/
vec3 ice_and_fire(float t)
{
    return segment(0.0,0.5,t) * ice(2.0*(t-0.0)) +
           segment(0.5,1.0,t) * fire(2.0*(t-0.5));
}

/******************************************************************************
 * ice_and_fire
 ******************************************************************************/
vec3 ice_and_fire(float t, vec3 under, vec3 over)
{
    return underover(t, ice_and_fire(t), under, over);
}

/******************************************************************************
 * reds
 ******************************************************************************/
vec3 reds(float t)
{
    return mix(vec3(1,1,1), vec3(1,0,0), t);
}

/******************************************************************************
 * reds
 ******************************************************************************/
vec3 reds(float t, vec3 under, vec3 over)
{
    return underover(t, reds(t), under, over);
}

/******************************************************************************
 * greens
 ******************************************************************************/
vec3 greens(float t)
{
    return mix(vec3(1,1,1), vec3(0,1,0), t);
}

/******************************************************************************
 * greens
 ******************************************************************************/
vec3 greens(float t, vec3 under, vec3 over)
{
    return underover(t, greens(t), under, over);
}

/******************************************************************************
 * blues
 ******************************************************************************/
vec3 blues(float t)
{
    return mix(vec3(1,1,1), vec3(0,0,1), t);
}

/******************************************************************************
 * blues
 ******************************************************************************/
vec3 blues(float t, vec3 under, vec3 over)
{
    return underover(t, blues(t), under, over);
}


/******************************************************************************
 * wheel
 ******************************************************************************/
vec3 wheel(float t)
{
    return clamp(abs(fract(t + vec3(1.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0) -1.0, 0.0, 1.0);
}

/******************************************************************************
 * wheel
 ******************************************************************************/
vec3 wheel(float t, vec3 under, vec3 over)
{
    return underover(t, wheel(t), under, over);
}

/******************************************************************************
 * stripes
 ******************************************************************************/
vec3 stripes(float t)
{
    return vec3(mod(floor(t * 64.0), 2.0) * 0.2 + 0.8);
}

/******************************************************************************
 * stripes
 ******************************************************************************/
vec3 stripes(float t, vec3 under, vec3 over)
{
    return underover(t, stripes(t), under, over);
}

// Discrete

/******************************************************************************
 * discrete
 ******************************************************************************/
vec3 discrete(float t)
{
    return segment(0.0,0.2,t) * vec3(1,0,0)
         + segment(0.2,0.5,t) * vec3(0,1,0)
         + segment(0.5,1.0,t) * vec3(0,0,1);
}
/******************************************************************************
 * discrete
 ******************************************************************************/
vec3 discrete(float t, vec3 under, vec3 over)
{
    return underover(t, discrete(t), under, over);
}

mat3 cotangent_frame( vec3 n, vec3 p, vec2 uv )
{
  vec3 dp1 = dFdx( p );
  vec3 dp2 = dFdy( p );
  vec3 dp2perp = cross( dp2, n );
  vec3 dp1perp = cross( n, dp1 );

  vec2 duv1 = dFdx( uv );
  vec2 duv2 = dFdy( uv );

  vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
  vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

  // construit une frame invariante à l'échelle
  float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
  
  return mat3( T * invmax, B * invmax, n );
}

////////////////////////////////////////////////////////////////////////////////
// PROGRAM
////////////////////////////////////////////////////////////////////////////////
void main()
{
	// // Retrieve PPTBF
	// float pptbf = texture( uPPTBFTexture, vTextureCoordinate ).r;
	
	// // Write mesh color
	// //oFragmentColor = vec4( uMeshColor, 0.5 );
	// vec3 color = vec3( pptbf, pptbf, pptbf );
	// oFragmentColor = vec4( color, 0.5 );
	
	const vec3 lightPosition = vec3(2,4,2);
	const vec3 color0 = vec3(0.4,0,0.8);
	const vec3 color1 = vec3(0.8,0,0);
	const float k_shin = 0.01;
	const float k_spec = 0.2;
	const float dtex = 1.0 / 512.0;
	//const float hmax = 1.0;
	
	vec3 Lu = normalize( lightPosition - vPosition );
    vec3 Nn = normalize( vNormal );
	mat3 fr = cotangent_frame(Nn, vPosition, vTextureCoordinate);
	float Nx = texture(uPPTBFTexture,vec2(vTextureCoordinate.x-dtex, vTextureCoordinate.y)).r - texture(uPPTBFTexture,vec2(vTextureCoordinate.x+dtex, vTextureCoordinate.y)).r;  
    float Ny = texture(uPPTBFTexture,vec2(vTextureCoordinate.x, vTextureCoordinate.y-dtex)).r  - texture(uPPTBFTexture,vec2(vTextureCoordinate.x, vTextureCoordinate.y+dtex)).r;  
	vec3 No = fr * normalize(vec3(hmax*Nx,hmax*Ny,0.5));

    vec3 V = normalize(-vPosition);
    float lamb = max(0.1,dot(Lu,No));
    vec3 R = reflect(-Lu,No);
    float spec = pow(max(0.0,dot(R,V)),k_spec);

    float pptbf = texture(uPPTBFTexture,vTextureCoordinate).r;
	
	// Apply colormap
	// - default is PPTBF gray level
	vec4 color = vec4( pptbf, pptbf, pptbf, 1.0 );
	// - select color map
	if ( uColormapIndex == 0 ) color.rgb = gray( pptbf );
	if ( uColormapIndex == 1 ) color.rgb = hot( pptbf );
    if ( uColormapIndex == 2 ) color.rgb = cool( pptbf );
    if ( uColormapIndex == 3 ) color.rgb = autumn( pptbf );
    if ( uColormapIndex == 4 ) color.rgb = winter( pptbf );
    if ( uColormapIndex == 5 ) color.rgb = spring( pptbf );
    if ( uColormapIndex == 6 ) color.rgb = summer( pptbf );
    if ( uColormapIndex == 7 ) color.rgb = ice( pptbf );
    if ( uColormapIndex == 8 ) color.rgb = fire( pptbf );
    if ( uColormapIndex == 9 ) color.rgb = ice_and_fire( pptbf );
    if ( uColormapIndex == 10 ) color.rgb = reds( pptbf );
    if ( uColormapIndex == 11 ) color.rgb = greens( pptbf );
    if ( uColormapIndex == 12 ) color.rgb = blues( pptbf );
	if ( uColormapIndex == 13 ) color.rgb = wheel( pptbf );
    if ( uColormapIndex == 14 ) color.rgb = stripes( pptbf );
    if ( uColormapIndex == 15 ) color.rgb = discrete( pptbf );
    if ( uColormapIndex == 16 ) color.rgb = gray( -.05 + 1.1 * pptbf, vec3( 1, 0, 0 ), vec3( 0, 0,1 ) );

    //vec3 color = mix( color0, color1, pptbf ) * lamb + k_shin * spec;
	//oFragmentColor = vec4( color, 1.0 );
	
	color.rgb = color.rgb * lamb + k_shin * spec;
	oFragmentColor = vec4( color );
}
