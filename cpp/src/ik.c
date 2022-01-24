#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

typedef struct point
{
  float x;
  float y;
  float z;
};
/*

  ^ y
  |
  |
  |
  |
  |
  |
  O - - - - - - - - - - -> x
  \
   \
    \
     \
      \
       _| z

*/


struct point* inverse_kinematics(int angles[5], int seglen[5])
{
  for(int i = 1 ; i < 4; i++)
    angles[i] -= angles[ i - 1 ];

  struct point* tgt = malloc(sizeof(struct point));
  tgt -> x = 0;
  tgt -> y = 0;
  tgt -> z = 0;

  tgt -> y += seglen[0];

  for(int i = 1; i < 5; i++)
    {
      tgt -> x += seglen[i] * cos( degToRad(angles[i]) );
      tgt -> y += seglen[i] * sin( degToRad(angles[i]) );
    }

  tgt -> z = tgt -> x * sin( degToRad(angles[0]) );
  //struct point* ret = &tgt;
  return tgt;
}

void free_point(struct point* p)
{
  free(p);
}

// cc -fPIC -shared -o ../ik.so ik.c
