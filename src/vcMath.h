#ifndef vcMath_h__
#define vcMath_h__

#include "udMath.h"

enum udEaseType
{
  udET_Linear,

  udET_QuadraticIn,
  udET_QuadraticOut,
  udET_QuadraticInOut,

  udET_CubicIn,
  udET_CubicOut,
  udET_CubicInOut,
};

template <typename T>
T udEase(const T t, udEaseType easeType = udET_Linear)
{
  T result = t;
  switch (easeType)
  {
  case udET_QuadraticIn:
    result = t * t;
    break;
  case udET_QuadraticOut:
    result = t * (T(2) - t);
    break;
  case udET_QuadraticInOut:
    result = (t < T(0.5)) ? (T(2) * t * t) : (T(-1) + (T(4) - T(2) * t) * t);
    break;
  case udET_CubicIn:
    result = t * t * t;
    break;
  case udET_CubicOut:
    result = (t - T(1)) * (t - 1) * (t - 1) + T(1);
    break;
  case udET_CubicInOut:
    result = (t < T(0.5)) ? (T(4) * t * t * t) : ((t - T(1))*(T(2) * t - T(2))*(T(2) * t - T(2)) + T(1));
    break;
  case udET_Linear: // fall through
  default:
    break;
  }

  return result;
}

//Clamp with wrap
template<typename T>
inline T udClampWrap(T val, T min, T max)
{
  if (max <= min)
    return min;

  if (val < min)
    val += ((min - val + (max - min - T(1))) / (max - min)) * (max - min); // Clamp above min

  return (val - min) % (max - min) + min;
}

template<>
inline float udClampWrap(float val, float min, float max)
{
  if (max <= min)
    return min;

  if (val < min)
    val += udCeil((min - val) / (max - min)) * (max - min); // Clamp above min

  return fmodf(val - min, max - min) + min;
}

template<>
inline double udClampWrap(double val, double min, double max)
{
  if (max <= min)
    return min;

  if (val < min)
    val += udCeil((min - val) / (max - min)) * (max - min); // Clamp above min

  return fmod(val - min, max - min) + min;
}

template <typename T>
static udVector3<T> udClosestPointOnOOBB(const udVector3<T> &point, const udMatrix4x4<T> &oobbMatrix)
{
  udVector3<T> origin = udVector3<T>::zero();
  udVector3<T> scale = udVector3<T>::zero();
  udQuaternion<T> rotation = udQuaternion<T>::identity();
  udExtractTransform(oobbMatrix, origin, scale, rotation);
  udVector3<T> localPoint = udInverse(rotation).apply(point - origin);
  return origin + rotation.apply(udClamp(localPoint, udDouble3::zero(), scale));
}

// Returns -1=outside, 0=inside, >0=partial (bits of planes crossed)
template <typename T>
static int udFrustumTest(const udVector4<T> frustumPlanes[6], const udVector3<T> &boundCenter, const udVector3<T> &boundExtents)
{
  int partial = 0;

  for (int i = 0; i < 6; ++i)
  {
    double distToCenter = udDot4(udVector4<T>::create(boundCenter, 1.0), frustumPlanes[i]);
    //optimized for case where boxExtents are all same: udFloat radiusBoxAtPlane = udDot3(boxExtents, udAbs(udVec3(curPlane)));
    double radiusBoxAtPlane = udDot3(boundExtents, udAbs(frustumPlanes[i].toVector3()));
    if (distToCenter < -radiusBoxAtPlane)
      return -1; // Box is entirely behind at least one plane
    else if (distToCenter <= radiusBoxAtPlane) // If spanned (not entirely infront)
      partial |= (1 << i);
  }

  return partial;
}

template <typename T>
static bool udPointInAABB(udVector3<T> point, udVector3<T> minPos, udVector3<T> maxPos)
{
  return (point.x >= minPos.x && point.y >= minPos.y && point.z >= minPos.z &&
    point.x <= maxPos.x && point.y <= maxPos.y && point.z <= maxPos.z);
}

template <typename T>
static udVector3<T> udGetSphereCenterFromPoints(T ballRadius, udVector3<T> p0, udVector3<T> p1, udVector3<T> p2)
{
  // See https://en.wikipedia.org/wiki/Circumscribed_circle, specifically regarding "higher dimensions"

  // Get circumscribed circle from triangle
  udVector3<T> a = p1 - p0;
  udVector3<T> b = p2 - p0;
  udVector3<T> triangleNormal = udNormalize(udCross(a, b));

  T aMagSq = udMagSq3(a);
  T aMag = udSqrt(aMagSq);
  T bMagSq = udMagSq3(b);
  T bMag = udSqrt(bMagSq);

  // Get circumradius
  T circumradius = (aMag * bMag * udMag3(a - b)) / (T(2) * udMag3(udCross(a, b)));

  // Get circumcenter
  udVector3<T> circumcenter = (udCross(aMagSq * b - bMagSq * a, udCross(a, b)) / (T(2) * udMagSq3(udCross(a, b)))) + p0;

  T distanceSq = (ballRadius * ballRadius) - (circumradius * circumradius);

  // Return sphere center
  if (distanceSq < T(0))
    return udVector3<T>::zero();
  else
    return circumcenter + triangleNormal * udSqrt(distanceSq);
}

template <typename T>
static T udDistanceToTriangle(udVector3<T> p0, udVector3<T> p1, udVector3<T> p2, udVector3<T> point, udVector3<T> *pClosestPoint = nullptr)
{
  // Solution taken from 'Real Time Collision Detection' by Christer Ericson,
  // p.141: 'Closest Point on Triangle to Point
  
  udVector3<T> p0_p1 = p1 - p0;
  udVector3<T> p0_p2 = p2 - p0;
  udVector3<T> p1_p2 = p2 - p1;

  T snom = udDot(point - p0, p0_p1);
  T sdenom = udDot(point - p1, p0 - p1);

  T tnom = udDot(point - p0, p0_p2);
  T tdenom = udDot(point - p2, p0 - p2);

  udVector3<T> closestPoint = {};

  do
  {
    if (snom <= T(0) && tnom <= T(0))
    {
      closestPoint = p0;
      break;
    }

    T unom = udDot(point - p1, p1_p2);
    T udenom = udDot(point - p2, p1 - p2);

    if (sdenom <= T(0) && unom <= T(0))
    {
      closestPoint = p1;
      break;
    }

    if (tdenom <= T(0) && udenom <= T(0))
    {
      closestPoint = p2;
      break;
    }

    udVector3<T> n = udCross3(p1 - p0, p2 - p0);
    T v_p2 = udDot(n, udCross3(p0 - point, p1 - point));
    if (v_p2 <= T(0) && snom >= T(0) && sdenom >= T(0))
    {
      closestPoint = p0 + p0_p1 * (snom / (snom + sdenom));
      break;
    }

    T v_p0 = udDot(n, udCross(p1 - point, p2 - point));
    if (v_p0 <= T(0) && unom >= T(0) && udenom >= T(0))
    {
      closestPoint = p1 + p1_p2 * (unom / (unom + udenom));
      break;
    }

    T v_p1 = udDot(n, udCross(p2 - point, p0 - point));
    if (v_p1 <= T(0) && tnom >= T(0) && tdenom >= T(0))
    {
      closestPoint = p0 + p0_p2 * (tnom / (tnom + tdenom));
      break;
    }

    T u = v_p0 / (v_p0 + v_p1 + v_p2);
    T v = v_p1 / (v_p0 + v_p1 + v_p2);
    T w = T(1) - u - v;
    closestPoint = u * p0 + v * p1 + w * p2;
  } while (false);

  if (pClosestPoint != nullptr)
    *pClosestPoint = closestPoint;

  return udMag3(closestPoint - point);
}

#endif //vcMath_h__
