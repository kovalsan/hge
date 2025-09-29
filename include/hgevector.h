/*-----------------------------------------------------------------------------
 * Haaf's Game Engine 1.9
 * Copyright (C) 2003-2007, Relish Games
 * Maintained 2012-2021 by dmytro.lytovchenko@gmail.com (github @kvakvs)
 * Github -- https://github.com/kvakvs/hge | Discord -- https://discord.gg/TdjamHt
 *
 * Old website: http://hge.relishgames.com; Old forum: http://relishgames.com/forum
 *-----------------------------------------------------------------------------*/
#pragma once

#include <hge.h>
#include <cmath>

class hgeVector {
public:
    float x, y;

    // Fast 1.0/sqrtf(float) routine
    static float InvSqrt(float x);

    hgeVector(const float _x, const float _y) {
      x = _x;
      y = _y;
    }

    hgeVector() {
      x = 0;
      y = 0;
    }

    hgeVector operator-() const {
      return hgeVector(-x, -y);
    }

    hgeVector operator-(const hgeVector &v) const {
      return hgeVector(x - v.x, y - v.y);
    }

    hgeVector operator+(const hgeVector &v) const {
      return hgeVector(x + v.x, y + v.y);
    }

    hgeVector &operator-=(const hgeVector &v) {
      x -= v.x;
      y -= v.y;
      return *this;
    }

    hgeVector &operator+=(const hgeVector &v) {
      x += v.x;
      y += v.y;
      return *this;
    }

    bool operator==(const hgeVector &v) const {
      return (x == v.x && y == v.y);
    }

    bool operator!=(const hgeVector &v) const {
      return (x != v.x || y != v.y);
    }

    hgeVector operator/(const float scalar) const {
      return hgeVector(x / scalar, y / scalar);
    }

    hgeVector operator*(const float scalar) const {
      return hgeVector(x * scalar, y * scalar);
    }

    hgeVector &operator*=(const float scalar) {
      x *= scalar;
      y *= scalar;
      return *this;
    }

    float Dot(const hgeVector *v) const {
      return x * v->x + y * v->y;
    }

    float Length() const {
      return sqrtf(Dot(this));
    }

    float LengthSq() const {
      return Dot(this);
    }

    float Angle(const hgeVector *v = nullptr) const;

    void Clamp(const float max) {
      const float length_sq = LengthSq();
      const float max_sq = max * max;
      if (length_sq > max_sq) {
        // Use fast inverse square root and avoid redundant calculations
        const float inv_length = InvSqrt(length_sq);
        x *= inv_length * max;
        y *= inv_length * max;
      }
    }

    hgeVector *Normalize() {
      const auto rc = InvSqrt(Dot(this));
      x *= rc;
      y *= rc;
      return this;
    }

    hgeVector *Rotate(float a);
};

inline hgeVector operator*(const float s, const hgeVector &v) {
  return v * s;
}

inline float operator^(const hgeVector &v, const hgeVector &u) {
  return v.Angle(&u);
}

inline float operator%(const hgeVector &v, const hgeVector &u) {
  return v.Dot(&u);
}
