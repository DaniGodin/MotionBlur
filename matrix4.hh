//
// Created by dany on 09/05/19.
//

#ifndef OPENGL_MATRIX4_HH
#define OPENGL_MATRIX4_HH


class matrix4 {


public:

    matrix4();

    void operator*=(const matrix4& rhs );

    static matrix4 identity();

    







};


#endif //OPENGL_MATRIX4_HH
