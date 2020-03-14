#ifndef vcLabelRenderer_h__
#define vcLabelRenderer_h__

#include "udMath.h"

enum vcLabelFontSize
{
  vcLFS_Medium, // Default size is medium

  vcLFS_Small,
  vcLFS_Large,

  vcLFS_Count,
};

struct vcLabelInfo
{
  udDouble3 worldPosition;

  const char *pText;
  vcLabelFontSize textSize;
  uint32_t textColourRGBA;
  uint32_t backColourRGBA;
};

bool vcLabelRenderer_Render(vcLabelInfo *pLabel, const udDouble4x4 &viewProjectionMatrix, const udUInt2 &screenSize);

#endif//vcLabelRenderer_h__
