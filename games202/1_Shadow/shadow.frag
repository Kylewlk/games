#version 430

uniform sampler2D uSampler;
uniform vec3 uKd;
uniform vec3 uKs;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform float uLightIntensity;
uniform vec3 uCameraPos;
uniform bool uUseTexture;
uniform bool uHalfLambert;

uniform sampler2D uShadowMap;
uniform vec2 uShadowMapInvSize;
uniform float uMaxLightDistance;
uniform float uBias;
uniform int uShadowType;
uniform int uSampleDistribution;
uniform int uSamplerFilterSize;

in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vNormal;
in vec4 vPositionInLight;

out vec4 fragColor;

// Shadow map related variables
#define NUM_SAMPLES 20
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

highp float rand_1to1(highp float x ) { 
  // -1 -1
  return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

float unpack(vec4 rgbaDepth) {
    const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
    return dot(rgbaDepth, bitShift);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples( const in vec2 randomSeed ) {

  float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

  float angle = rand_2to1( randomSeed ) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}

void uniformDiskSamples( const in vec2 randomSeed ) {

  float randNum = rand_2to1(randomSeed);
  float sampleX = rand_1to1( randNum ) ;
  float sampleY = rand_1to1( sampleX ) ;

  float angle = sampleX * PI2;
  float radius = sqrt(sampleY);

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( radius * cos(angle) , radius * sin(angle)  );

    sampleX = rand_1to1( sampleY ) ;
    sampleY = rand_1to1( sampleX ) ;

    angle = sampleX * PI2;
    radius = sqrt(sampleY);
  }
}

float PCF(vec2 shadowCoord, int penumbraSize)
{
  vec3 lightDir = normalize(uLightPos - vWorldPos);
  vec3 normal = normalize(vNormal);
  float bias = max(uBias * (1.0 - dot(normal, lightDir)), 0.0001);
  float dis = distance(vWorldPos, uLightPos);
  float occ = 0.0;

  if (uSampleDistribution <= 1)
  {
    if(uSampleDistribution == 0)
    {
      poissonDiskSamples(shadowCoord);
    }
    else
    {
      uniformDiskSamples(shadowCoord);
    }

    float lightDis = unpack(texture(uShadowMap, shadowCoord)) * uMaxLightDistance;
    occ += step(0, (lightDis - dis + bias));

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
      vec2 uv = shadowCoord + (poissonDisk[i] * uShadowMapInvSize * float(penumbraSize));
      lightDis = unpack(texture(uShadowMap, uv)) * uMaxLightDistance;
      occ += step(0, (lightDis - dis + bias));
    }
    occ = occ / (float(NUM_SAMPLES) + 1.0);
  }
  else
  {
    for(int x = -penumbraSize; x <= penumbraSize; ++x)
    {
        for(int y = -penumbraSize; y <= penumbraSize; ++y)
        {
          vec2 uv = shadowCoord + vec2(x, y) * uShadowMapInvSize;
          float lightDis = unpack(texture(uShadowMap, uv)) * uMaxLightDistance;
          occ += step(0.0, (lightDis - dis + bias));        
        }    
    }
    float col = float(penumbraSize * 2 + 1);
    occ /= col * col;
  }

  return occ * 0.5 + 0.5;
}

float findBlocker(vec2 shadowCoord)
{
  float blocker = 0.0f;
  const int blockerSize = 5;
  for(int x = -blockerSize; x <= blockerSize; ++x)
  {
      for(int y = -blockerSize; y <= blockerSize; ++y)
      {
        vec2 uv = shadowCoord + vec2(x, y) * uShadowMapInvSize;
        blocker += unpack(texture(uShadowMap, uv));
             
      }    
  }
  float col = float(blockerSize * 2 + 1);
  blocker /= col * col;

	return blocker;
}

float PCSS(vec2 shadowCoord)
{

  // STEP 1: avgblocker depth
  float blockDepth = findBlocker(shadowCoord);
  blockDepth *= uMaxLightDistance;
  
  // STEP 2: penumbra size
  float ligthDis = distance(vWorldPos, uLightPos);

  float size = uSamplerFilterSize * (ligthDis - blockDepth) / blockDepth; 
  int penumbraSize = max(0, min(1000, int(size)));

  // STEP 3: filtering
  float f = PCF(shadowCoord, penumbraSize);

  return f;
}


float useShadowMap(vec2 shadowCoord)
{
  float dis = distance(vWorldPos, uLightPos);
  float lightDis = unpack(texture(uShadowMap, shadowCoord)) * uMaxLightDistance;

  vec3 lightDir = normalize(uLightPos - vWorldPos);
  vec3 normal = normalize(vNormal);
  float bias = max(uBias * (1.0 - dot(normal, lightDir)), 0.0001);

  return step(0, (lightDis - dis + bias)) * 0.5 + 0.5;
}


vec3 blinnPhong() 
{
  vec3 color;
  if (uUseTexture) {
    color = pow(texture(uSampler, vTexCoord).rgb, vec3(2.2));
  } else {
    color = uKd;
  }
  
  vec3 ambient = 0.05 * color * uLightColor;

  vec3 lightDir = normalize(uLightPos - vWorldPos);
  vec3 normal = normalize(vNormal);
  float diff = 0.0;
  if (uHalfLambert)
  {
    diff = dot(lightDir, normal) * 0.5 + 0.5;
  }
  else
  {
    diff = max(dot(lightDir, normal), 0.0); 
  }
  float light_atten_coff = uLightIntensity / (length(uLightPos - vWorldPos) + 1.0);
  vec3 diffuse =  color * uLightColor * diff * light_atten_coff;

  vec3 viewDir = normalize(uCameraPos - vWorldPos);
  float spec = 0.0;
  // vec3 reflectDir = reflect(-uLightDir, normal);
  // spec = pow (max(dot(viewDir, reflectDir), 0.0), 35.0);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  spec = pow (max(dot(viewDir, halfwayDir), 0.0), 35.0);
  vec3 specular = uKs * uLightColor * spec * light_atten_coff;  
  
  return pow((ambient + diffuse + specular), vec3(1.0/2.2));
}


void main(void) 
{
  vec2 shadowCoord = (vPositionInLight.xy/vPositionInLight.w) * 0.5 + 0.5;

  float visibility = 1.0;
  if(shadowCoord.x >=0 && shadowCoord.x < 1.0 && shadowCoord.y >= 0 && shadowCoord.y <=1.0)
  {
    if (uShadowType == 1)
    {
      visibility = useShadowMap(shadowCoord);
    }
    else if (uShadowType == 2)
    {
      visibility = PCF(shadowCoord, uSamplerFilterSize);
    }
    else
    {
      visibility = PCSS(shadowCoord);
    }
  }

  vec3 phongColor = blinnPhong();

  fragColor = vec4(phongColor * visibility, 1.0);
  // fragColor = vec4(phongColor, 1.0);
} 
