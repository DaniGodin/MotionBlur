#include <iostream>

#include <string.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "matrix4.hh"





int main(int argc, char **argv)
{
    mygl::matrix4 m = mygl::matrix4::identity();

    std::cout << m;

    return 0;
}