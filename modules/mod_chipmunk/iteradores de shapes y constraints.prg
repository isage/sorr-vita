typedef void (*cpBodyConstraintIteratorFunc)(cpBody *body, cpConstraint *constraint, void *data)
void cpBodyEachConstraint(cpBody *body, cpBodyConstraintIteratorFunc func, void *data)

typedef void (*cpBodyShapeIteratorFunc)(cpBody *body, cpShape *shape, void *data)
void cpBodyEachShape(cpBody *body, cpBodyShapeIteratorFunc func, void *data)

1512

typedef void (*cpSpaceConstraintIteratorFunc)(cpConstraint *constraint, void *data)
void cpSpaceEachConstraint(cpSpace *space, cpSpaceConstraintIteratorFunc func, void *data)

typedef void (*cpSpaceShapeIteratorFunc)(cpShape *shape, void *data)
void cpSpaceEachShape(cpSpace *space, cpSpaceShapeIteratorFunc func, void *data)

typedef void (*cpSpaceBodyIteratorFunc)(cpBody *body, void *data)
void cpSpaceEachBody(cpSpace *space, cpSpaceBodyIteratorFunc func, void *data)

        Failed condition: cpSpaceContainsConstraint(space, constraint)


         if (cpSpaceIsLocked(modChipmunk_cpEspacio))
            cpSpaceAddPostStepCallback(modChipmunk_cpEspacio, (cpPostStepFunc)rompeJoint, c, NULL);
        else{
            cpSpaceRemoveConstraint(modChipmunk_cpEspacio, c);
            cpConstraintFree(c);
        }
        
        usar todo esto...