//
// Created by dany on 09/05/19.
//

#ifndef OPENGL_MATRIX4_HH
#define OPENGL_MATRIX4_HH


#include <iosfwd>
#include <vector>
#include <iostream>

namespace mygl {

    class matrix4 {


    public:

        matrix4(float values[4][4]);

        void operator*=(const matrix4 &rhs);

        static matrix4 identity();


    public:

        float buffer[4][4];

    };


    std::ostream& operator<<(std::ostream& out, const matrix4& mat);

}














#endif //OPENGL_MATRIX4_HH
