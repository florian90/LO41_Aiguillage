#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#define OUEST   2
#define EST     1
#define BIDIRR (OUEST | EST)

#define VAL_TGV 1
#define VAL_GL  2
#define VAL_M   4
#define VAL_TOT (VAL_TGV | VAL_GL | VAL_M)

#define POS_GARE    1
#define POS_GARRAGE 2
#define POS_TUNNEL  3
#define POS_LIGNE   4

typedef enum {
    TGV = VAL_TGV,
    GL  = VAL_GL,
    M   = VAL_M
}Type;

typedef enum {
    droite = EST,
    gauche = OUEST
}Direction;

#endif // STRUCTURES_H_INCLUDED
