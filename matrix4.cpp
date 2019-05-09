//
// Created by dany on 09/05/19.
//

#include "matrix4.hh"

namespace mygl {

    matrix4::matrix4(float values[4][4]) {
        for(int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++)
                buffer[i][j] = values[i][j];
        }

    }

    void matrix4::operator*=(const mygl::matrix4 &rhs) {
        float res[4][4];
        for(int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                res[i][j] = 0;
                for(int k =0; k < 4; k++)
                    res[i][j] += buffer[i][k] * rhs.buffer[k][j];

            }

        }

        for (int i = 0; i < 4; i++){
            for( int j= 0; j < 4; j++)
                buffer[i][j] = res[i][j];
        }

    }

    matrix4 matrix4::identity() {

        float arr[4][4] = {{1, 0, 0, 0},
                           {0, 1, 0, 0},
                           {0, 0, 1, 0},
                           {0, 0, 0, 1}};

        matrix4 iden = matrix4(arr);

        return iden;
    }


    std::ostream& operator<<(std::ostream& out, const matrix4& mat){

        for(const auto &rows : mat.buffer){
            for(const auto &elt: rows){
                out << elt;
                out << " ";
            }
            out << '\n';
        }

        return out;

    }

}