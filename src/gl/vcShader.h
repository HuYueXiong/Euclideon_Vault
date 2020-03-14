#ifndef vcShader_h__
#define vcShader_h__

#include "udMath.h"
#include "gl/vcLayout.h"

struct vcShader;
struct vcShaderSampler;
struct vcShaderConstantBuffer;
struct vcTexture;

bool vcShader_CreateFromText(vcShader **ppShader, const char *pVertexShader, const char *pFragmentShader, const vcVertexLayoutTypes *pInputTypes, uint32_t totalInputs, const char *pGeometryShader = nullptr);
template <size_t N> inline bool vcShader_CreateFromText(vcShader **ppShader, const char *pVertexShader, const char *pFragmentShader, const vcVertexLayoutTypes (&inputTypes)[N], const char *pGeometryShader = nullptr) { return vcShader_CreateFromText(ppShader, pVertexShader, pFragmentShader, inputTypes, (uint32_t)N, pGeometryShader); }
void vcShader_DestroyShader(vcShader **ppShader);

bool vcShader_Bind(vcShader *pShader); // nullptr to unbind shader

bool vcShader_BindTexture(vcShader *pShader, vcTexture *pTexture, uint16_t samplerIndex, vcShaderSampler *pSampler = nullptr);

bool vcShader_GetConstantBuffer(vcShaderConstantBuffer **ppBuffer, vcShader *pShader, const char *pBufferName, const size_t bufferSize);
bool vcShader_BindConstantBuffer(vcShader *pShader, vcShaderConstantBuffer *pBuffer, const void *pData, const size_t bufferSize);
bool vcShader_ReleaseConstantBuffer(vcShader *pShader, vcShaderConstantBuffer *pBuffer);

bool vcShader_GetSamplerIndex(vcShaderSampler **ppSampler, vcShader *pShader, const char *pSamplerName);

#endif
