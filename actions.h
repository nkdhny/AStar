#ifndef ACTIONS_H
#define ACTIONS_H

#include "model.h"
#include <vector>
#include <boost/utility.hpp>
#include <utility>

class FifteenAction: public std::unary_function<const Field&, Field> {
public:
    virtual Field operator()(const Field& f) const {
        return f;
    }
    virtual bool isDefined(const Field&) const = 0;
    virtual std::ostream& print(std::ostream& os) const = 0;
    virtual ~FifteenAction() {}
};

template<int D_ROW, int D_COL>
class MoveAction: public FifteenAction {
public:
    enum {row = D_ROW, col = D_COL};

    const Displacement d;

    const Position move_from;

    MoveAction(Position _move_from):
        move_from(_move_from),
        d(MoveAction<D_ROW, D_COL>::row, MoveAction<D_ROW, D_COL>::col)
    {}

    Field operator()(const Field& origin) const {
        Position move_to = move_from+d;

        assert(origin.comprise(move_to));
        assert(isDefined(origin));

        return origin.swap(move_from, move_to);
    }

    bool isDefined(const Field& origin) const {

        assert(origin.comprise(move_from));

        Position move_to = move_from+d;

        return origin.comprise(move_to) && origin.vacant(move_from) && origin.occupied(move_to);
    }


    std::ostream& print(std::ostream& os) const{
        os<<"move ["<<move_from<<"+"<<d<<"]->"<<std::endl;
        return os;
    }


    ~MoveAction() {}

};
template <int R, int C>
std::ostream& operator << (std::ostream& os, const MoveAction<R, C>& a) {
    os<<"move ["<<a.move_from<<"+"<<a.d<<"]->"<<std::endl;
}


typedef MoveAction< 0, -1> MoveLeft;
typedef MoveAction< 0,  1> MoveRight;
typedef MoveAction<-1,  0> MoveUp;
typedef MoveAction< 1,  0> MoveDown;

#endif // ACTIONS_H
