#ifndef MATH_H
#define MATH_H

#include <Arduino.h>

template <class T>
class Vector3 {
private:
  T v[3];

public:
  Vector3() {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
  }

  Vector3(const Vector3<T> &o) {
    v[0] = o.v[0];
    v[1] = o.v[1];
    v[2] = o.v[2];
  }

  Vector3(T x, T y, T z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }

  Vector3(T *p) {
    v[0] = p[0];
    v[1] = p[1];
    v[2] = p[2];
  }

  T& operator[] (const size_t i) {
    return v[i];
  }

  void set(float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }
  
  void set(const Vector3 &i) {
    v[0] = i.v[0];
    v[1] = i.v[1];
    v[2] = i.v[2];
  }
  
  T dp(Vector3 &i) {
    T op = 0;
    op += v[0] * i.v[0];
    op += v[1] * i.v[1];
    op += v[2] * i.v[2];
    return op; 
  }

  Vector3 cp(Vector3 &i) {
    return Vector3(
      (v[1] * i.v[2]) - (v[2] * i.v[1]),
      (v[2] * i.v[0]) - (v[0] * i.v[2]),
      (v[0] * i.v[1]) - (v[1] * i.v[0])
    );
  }

  Vector3 scale(float scale) {
    return Vector3(
      v[0] * scale, 
      v[1] * scale, 
      v[2] * scale 
    );
  }

  Vector3 add(Vector3 &i) {
    return Vector3(
      v[0] + i.v[0],
      v[1] + i.v[1],
      v[2] + i.v[2]
    );
  }

  float magnitude() {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  }

  void print() {
    printf("V<% 6.3f % 6.3f % 6.3f>", v[0], v[1], v[2]);
  }
};

template <class T>
class Matrix3x3 {
private:
  Vector3<T> rows[3];

public:
  Matrix3x3() {
    identity();
  }

  Matrix3x3(Vector3<T> &a, Vector3<T> &b, Vector3<T> &c) {
    rows[0] = a;
    rows[1] = b;
    rows[2] = c;
  }

  Matrix3x3(const Matrix3x3<T> &m) {
    rows[0] = m.rows[0];
    rows[1] = m.rows[1];
    rows[2] = m.rows[2];
  }

public:
  void identity() {
    rows[0].set(1, 0, 0);
    rows[1].set(0, 1, 0);
    rows[2].set(0, 0, 1);
  }

  Vector3<T>& operator[] (const size_t i) {
    return rows[i];
  }

  Matrix3x3 multiply(Matrix3x3 &m) {
    Matrix3x3 o;
    for (byte x = 0; x < 3; ++x) {
      for (byte y = 0; y < 3; ++y) {
        float op[3]; 
        for (byte w = 0; w < 3; ++w) {
          op[w] = rows[x][w] * m[w][y];
        } 
        o[x][y] = op[0] + op[1] + op[2];
      }
    }
    return o;
  }

  Matrix3x3 add(Matrix3x3 &m) {
    Matrix3x3 o;
    for (byte x = 0; x < 3; ++x) {
      for (byte y = 0; y < 3; ++y) {
        o[x][y] = rows[x][y] + m[x][y];
      }
    }
    return o;
  }

  void print() {
    rows[0].print(); printf("\n\r");
    rows[1].print(); printf("\n\r");
    rows[2].print(); printf("\n\r");
  }

};

typedef Vector3<float> FVector3;
typedef Matrix3x3<float> FMatrix3x3;

#endif
