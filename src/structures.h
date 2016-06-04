#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#define NB_THREAD 1

#define EST     (1<<0)
#define OUEST   (1<<1)
#define BIDIRR  (OUEST | EST)

#define VAL_TGV (1<<0)
#define VAL_GL  (1<<1)
#define VAL_M   (1<<2)
#define VAL_TOT (VAL_TGV | VAL_GL | VAL_M)

#define POS_GARE    1
#define POS_GARAGE (POS_GARE+1)
#define POS_TUNNEL  (POS_GARAGE+1)
#define POS_LIGNE   (POS_TUNNEL+1)

#define POS_OUEST POS_GARAGE
#define POS_EST   (POS_LIGNE+1)

typedef enum {
    TGV = VAL_TGV,
    GL  = VAL_GL,
    M   = VAL_M
}Type_t;

typedef enum {
    est = EST,
    ouest = OUEST
}Direction_t;

#endif // STRUCTURES_H_INCLUDED
