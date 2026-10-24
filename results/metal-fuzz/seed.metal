#include <metal_stdlib>
using namespace metal;
kernel void k1(texture2d<float, access::read> in [[texture(0)]],
               texture2d<float, access::write> out [[texture(1)]],
               uint2 gid [[thread_position_in_grid]]) {
  float4 c = in.read(gid);
  c.rgb = c.rgb * (1.0 - c.a * 0.5) + float3(0.1, 0.2, 0.3);
  out.write(c, gid);
}
