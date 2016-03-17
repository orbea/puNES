{
"\n"
"//\n"
"// PUBLIC DOMAIN CRT STYLED SCAN-LINE SHADER\n"
"//\n"
"//   by Timothy Lottes\n"
"//\n"
"// This is more along the style of a really good CGA arcade monitor.\n"
"// With RGB inputs instead of NTSC.\n"
"// The shadow mask example has the mask rotated 90 degrees for less chromatic aberration.\n"
"//\n"
"// Left it unoptimized to show the theory behind the algorithm.\n"
"//\n"
"// It is an example what I personally would want as a display option for pixel art games.\n"
"// Please take and use, change, or whatever.\n"
"//\n"
"\n"
"// -- config  -- //\n"
"#pragma parameter hardScan \"hardScan\" -8.0 -20.0 0.0 1.0 // default, minimum, maximum, optional step\n"
"#pragma parameter hardPix \"hardPix\" -3.0 -20.0 0.0 1.0\n"
"#pragma parameter warpX \"warpX\" 0.031 0.0 0.125 0.01\n"
"#pragma parameter warpY \"warpY\" 0.041 0.0 0.125 0.01\n"
"#pragma parameter maskDark \"maskDark\" 0.5 0.0 2.0 0.1\n"
"#pragma parameter maskLight \"maskLight\" 1.5 0.0 2.0 0.1\n"
"#pragma parameter scaleInLinearGamma \"scaleInLinearGamma\" 1.0 0.0 1.0 1.0\n"
"#pragma parameter shadowMask \"shadowMask\" 1.0 0.0 1.0 1.0\n"
"#pragma parameter brightboost \"brightness\" 1.0 0.0 2.0 0.05\n"
"\n"
"#ifdef PARAMETER_UNIFORM // If the shader implementation understands #pragma parameters, this is defined.\n"
"uniform float hardScan;\n"
"uniform float hardPix;\n"
"uniform float warpX;\n"
"uniform float warpY;\n"
"uniform float maskDark;\n"
"uniform float maskLight;\n"
"uniform float scaleInLinearGamma;\n"
"uniform float shadowMask;\n"
"uniform float brightboost;\n"
"#else\n"
"#define hardScan -8.0\n"
"#define hardPix -3.0\n"
"#define warpX 0.031\n"
"#define warpY 0.041\n"
"#define maskDark 0.5\n"
"#define maskLight 1.5\n"
"#define scaleInLinearGamma 1\n"
"#define shadowMask 1\n"
"#define brightboost 1\n"
"\n"
"#endif\n"
"// ------------- //\n"
"\n"
"void main_vertex\n"
"(\n"
"   float4 position : POSITION,\n"
"   out float4 oPosition : POSITION,\n"
"   uniform float4x4 modelViewProj,\n"
"   float2 tex : TEXCOORD,\n"
"   out float2 oTex : TEXCOORD\n"
")\n"
"{\n"
"   oPosition = mul(modelViewProj, position);\n"
"   oTex = tex;\n"
"}\n"
"\n"
"struct input\n"
"{\n"
"   float2 video_size;\n"
"   float2 texture_size;\n"
"   float2 output_size;\n"
"   float  frame_count;\n"
"   float  frame_direction;\n"
"   float frame_rotation;\n"
"};\n"
"\n"
"input IN_global;\n"
"sampler2D s0_global;\n"
"\n"
"#define warp float2(warpX,warpY)\n"
"\n"
"\n"
"//------------------------------------------------------------------------\n"
"\n"
"// sRGB to Linear.\n"
"// Assuing using sRGB typed textures this should not be needed.\n"
"float ToLinear1(float c)\n"
"{\n"
"   if (scaleInLinearGamma==0) return c;\n"
"   return(c<=0.04045)?c/12.92:pow((c+0.055)/1.055,2.4);\n"
"}\n"
"float3 ToLinear(float3 c)\n"
"{\n"
"   if (scaleInLinearGamma==0) return c;\n"
"   return float3(ToLinear1(c.r),ToLinear1(c.g),ToLinear1(c.b));\n"
"}\n"
"\n"
"// Linear to sRGB.\n"
"// Assuing using sRGB typed textures this should not be needed.\n"
"float ToSrgb1(float c)\n"
"{\n"
"   if (scaleInLinearGamma==0) return c;\n"
"   return(c<0.0031308?c*12.92:1.055*pow(c,0.41666)-0.055);\n"
"}\n"
"\n"
"float3 ToSrgb(float3 c)\n"
"{\n"
"   if (scaleInLinearGamma==0) return c;\n"
"   return float3(ToSrgb1(c.r),ToSrgb1(c.g),ToSrgb1(c.b));\n"
"}\n"
"\n"
"// Nearest emulated sample given floating point position and texel offset.\n"
"// Also zero's off screen.\n"
"float3 Fetch(float2 pos,float2 off){\n"
"  pos=(floor(pos*IN_global.texture_size.xy+off)+float2(0.5,0.5))/IN_global.texture_size.xy;\n"
"  return ToLinear(brightboost * tex2D(s0_global,pos.xy).rgb);}\n"
"\n"
"// Distance in emulated pixels to nearest texel.\n"
"float2 Dist(float2 pos){pos=pos*IN_global.texture_size.xy;return -((pos-floor(pos))-float2(0.5));}\n"
"    \n"
"// 1D Gaussian.\n"
"float Gaus(float pos,float scale){return exp2(scale*pos*pos);}\n"
"\n"
"// 3-tap Gaussian filter along horz line.\n"
"float3 Horz3(float2 pos,float off){\n"
"  float3 b=Fetch(pos,float2(-1.0,off));\n"
"  float3 c=Fetch(pos,float2( 0.0,off));\n"
"  float3 d=Fetch(pos,float2( 1.0,off));\n"
"  float dst=Dist(pos).x;\n"
"  // Convert distance to weight.\n"
"  float scale=hardPix;\n"
"  float wb=Gaus(dst-1.0,scale);\n"
"  float wc=Gaus(dst+0.0,scale);\n"
"  float wd=Gaus(dst+1.0,scale);\n"
"  // Return filtered sample.\n"
"  return (b*wb+c*wc+d*wd)/(wb+wc+wd);}\n"
"\n"
"// 5-tap Gaussian filter along horz line.\n"
"float3 Horz5(float2 pos,float off){\n"
"  float3 a=Fetch(pos,float2(-2.0,off));\n"
"  float3 b=Fetch(pos,float2(-1.0,off));\n"
"  float3 c=Fetch(pos,float2( 0.0,off));\n"
"  float3 d=Fetch(pos,float2( 1.0,off));\n"
"  float3 e=Fetch(pos,float2( 2.0,off));\n"
"  float dst=Dist(pos).x;\n"
"  // Convert distance to weight.\n"
"  float scale=hardPix;\n"
"  float wa=Gaus(dst-2.0,scale);\n"
"  float wb=Gaus(dst-1.0,scale);\n"
"  float wc=Gaus(dst+0.0,scale);\n"
"  float wd=Gaus(dst+1.0,scale);\n"
"  float we=Gaus(dst+2.0,scale);\n"
"  // Return filtered sample.\n"
"  return (a*wa+b*wb+c*wc+d*wd+e*we)/(wa+wb+wc+wd+we);}\n"
"\n"
"// Return scanline weight.\n"
"float Scan(float2 pos,float off){\n"
"  float dst=Dist(pos).y;\n"
"  return Gaus(dst+off,hardScan);}\n"
"\n"
"// Allow nearest three lines to effect pixel.\n"
"float3 Tri(float2 pos){\n"
"  float3 a=Horz3(pos,-1.0);\n"
"  float3 b=Horz5(pos, 0.0);\n"
"  float3 c=Horz3(pos, 1.0);\n"
"  float wa=Scan(pos,-1.0);\n"
"  float wb=Scan(pos, 0.0);\n"
"  float wc=Scan(pos, 1.0);\n"
"  return a*wa+b*wb+c*wc;}\n"
"\n"
"// Distortion of scanlines, and end of screen alpha.\n"
"float2 Warp(float2 pos){\n"
"  pos=pos*2.0-1.0;    \n"
"  pos*=float2(1.0+(pos.y*pos.y)*warp.x,1.0+(pos.x*pos.x)*warp.y);\n"
"  return pos*0.5+0.5;}\n"
"\n"
"// Shadow mask.\n"
"float3 Mask(float2 pos){\n"
"  pos.x+=pos.y*3.0;\n"
"  float3 mask=float3(maskDark,maskDark,maskDark);\n"
"  pos.x=fract(pos.x/6.0);\n"
"  if(pos.x<0.333)mask.r=maskLight;\n"
"  else if(pos.x<0.666)mask.g=maskLight;\n"
"  else mask.b=maskLight;\n"
"  return mask;}    \n"
"\n"
"\n"
"float4 main_fragment (float2 tex : TEXCOORD, uniform sampler2D s0 : TEXUNIT0, uniform input IN) : COLOR\n"
"{\n"
"   float2 pos=Warp(tex.xy*(IN.texture_size.xy/IN.video_size.xy))*(IN.video_size.xy/IN.texture_size.xy);\n"
"   s0_global = s0;\n"
"   IN_global = IN;\n"
"   float3 outColor = Tri(pos);\n"
"\n"
"   if(shadowMask)\n"
"      outColor.rgb*=Mask(floor(tex.xy*(IN.texture_size.xy/IN.video_size.xy)*IN.output_size.xy)+float2(0.5,0.5));\n"
"	  \n"
"   return float4(ToSrgb(outColor.rgb),1.0);\n"
"\n"
"}\n"
},