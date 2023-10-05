#version 430

uniform bool uUseDistance; // light distance
uniform vec3 uLightPos;
uniform float uMaxDistance;

in vec3 vWorldPos;

out vec4 fragColor;

// depth 范围[0, 1.0]
// r = depth - fract(depth * 256) / 256
// g = (fract(depth * 256) / 256 - fract(depth * 256 * 256) / (256 * 256)) * 256
//   = fract(depth * 256) - fract(depth * 256 * 256) / 256
// b = fract(depth * 256 * 256) - fract(depth * 256 * 256 * 256) / 256
// a = fract(depth * 256 * 256 * 256)
vec4 pack (float depth) {
    // 使用rgba 4字节共32位来存储z值,1个字节精度为1/256
    const vec4 bitShift = vec4(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0);
    const vec4 bitMask = vec4(1.0/256.0, 1.0/256.0, 1.0/256.0, 0.0);
    // gl_FragCoord:片元的坐标,fract():返回数值的小数部分
    vec4 rgbaDepth = fract(depth * bitShift); //计算每个点的z值
    rgbaDepth -= rgbaDepth.gbaa * bitMask; // Cut off the value which do not fit in 8 bits
    return rgbaDepth;
}

float unpack(vec4 depth)
{
    const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0 * 256.0), 1.0/(256.0 * 256.0 * 256.0));
    return dot(depth, bitShift);
}

void main()
{
  if (uUseDistance)
  {
      float dis = distance(vWorldPos, uLightPos);
      fragColor = pack(dis/uMaxDistance);
  }
  else
  {
      fragColor = pack(gl_FragCoord.z);
  }
}