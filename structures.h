#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#define OUEST   (1<<1)
#define EST     (1<<0)
#define BIDIRR (OUEST | EST)

#define VAL_TGV (1<<0)
#define VAL_GL  (1<<1)
#define VAL_M   (1<<2)
#define VAL_TOT (VAL_TGV | VAL_GL | VAL_M)

#define POS_GARE    1
#define POS_GARRAGE (POS_GARE+1)
#define POS_TUNNEL  (POS_GARRAGE+1)
#define POS_LIGNE   (POS_TUNNEL+1)

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
