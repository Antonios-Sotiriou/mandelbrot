#ifndef _PAINTER_H
#define _PAINTER_H 1

// general headers
#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

// Project specific headers
#ifndef _OBJECTS_H
    #include "./objects.h"
#endif

void painter(const KNOT knot, char *image_data);

#endif /* _PAINTER_H */

