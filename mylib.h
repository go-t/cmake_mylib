#include "mylib_export.h"

MYLIB_EXPORT int add(int a, int b);

class MYLIB_EXPORT Point {
    int x, y;
    
    Point(int x, int y);
};
