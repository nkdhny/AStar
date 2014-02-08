#ifndef MODEL_H
#define MODEL_H

#include <utility>
#include <boost/optional.hpp>
#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <boost/utility.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/ref.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/bind.hpp>

struct Tile
{

    const unsigned value;

    explicit Tile(unsigned _value):
        value(_value)
    {}

    Tile(const Tile& other):
        value(other.value)
    {}

    Tile operator = (const Tile& other) {
        return Tile(other);
    }

    bool operator == (const Tile& other) const {
        return value == other.value;
    }
};

std::ostream& operator << (std::ostream& os, const boost::optional<Tile> t) {
    if(t){
        os<<t.get();
    } else {
        os<<"_";
    }
}

struct Position
{
    const int row;
    const int column;

    Position(int _row, int _column):
        row(_row),
        column(_column)
    {}

    Position operator + (const Position& other) const {
        Position tmp(this->row+other.row, this->column+other.column);
        return tmp;
    }

    Position operator - (const Position& other) const {
        Position tmp(this->row+other.row, this->column+other.column);
        assert(tmp.row>0 && tmp.column > 0);
        return tmp;
    }

    Position(const Position& other):
        row(other.row),
        column(other.column)
    {}

    Position operator = (const Position& other) {
        return Position(other);
    }

    bool operator == (const Position& other) const {
        return other.row == row && other.column == column;
    }
};

std::ostream& operator << (std::ostream& os, const Position& pos) {
    os<<"["<<pos.row<<":"<<pos.column<<"]";

    return os;
}

typedef Position Displacement;

struct Place
{
    const Position position;
    boost::optional<Tile> tileMaybe;

    Place():
        position(0,0),
        tileMaybe()
    {}

    Place(Position _position):
        position(_position)
    {}

    Place(Position _position, Tile _tile):
        position(_position),
        tileMaybe(_tile)
    {}

    bool vacant() const {
        return tileMaybe==boost::none;
    }

    bool occupied() const {
        return ! vacant();
    }

    void place(Tile tile) {
        assert(vacant());
        tileMaybe = tile;
    }

    void move(){
        tileMaybe = boost::none;
    }

    Place(const Place& other):
        position(other.position),
        tileMaybe(other.tileMaybe)
    {}

    Place operator=(const Place& other){
        return Place(other);
    }

    bool operator == (const Place& other) const {
        return other.position == position && other.tileMaybe == tileMaybe;
    }
};


std::ostream& operator << (std::ostream& os, const Place& pl) {
    os<<pl.position<<"{";
    os<<pl.tileMaybe;
    os<<"}";

    return os;
}

class Field {
    unsigned size_squared;
    std::vector<Place> places;

    int movementsDone;

    unsigned index(Position pos) const {
        assert(pos.row >= 0);
        assert(pos.column >= 0);
        return pos.row*size + pos.column;
    }

    Field();

    int countEmpty() {
        int count = std::count_if(
                        places.begin(),
                        places.end(),
                        boost::lambda::bind(&Place::vacant, boost::lambda::_1)
                    );

        return count;
    }



public:

    unsigned size;
    typedef std::vector<Place>::iterator iterator;
    typedef std::vector<Place>::const_iterator const_iterator;

    explicit Field(unsigned _size):
        size(_size),
        size_squared(_size*_size),
        places(),
        movementsDone(0)
    {


        for(int row = 0; row < size; row++) {
            for(int column = 0; column < size; column++){
                Position pos(row, column);
                Place pl(pos, Tile(index(pos)+1));

                places.push_back(pl);
            }
        }



        places[size_squared - 1].move();
        assert(size_squared == places.size());
        assert(countEmpty()==1);
    }

    explicit Field(const std::vector<Place>& tiles):
        size(sqrt(tiles.size())),
        size_squared(tiles.size()),
        places(tiles),
        movementsDone(0)
    {
        assert(countEmpty()==1);
        assert(static_cast<double>(size) == sqrt(tiles.size()));
    }

    std::ostream& print(std::ostream& os) const {
        os<<"\t";
        for(unsigned i=0;i<size_squared;i++){
            if(i>0 && i%size==0){                
                os<<std::endl;
                os<<"\t";
            }
            if(places[i].vacant()){
                os<<"_";
            } else {
                os<<places[i].tileMaybe.get().value;
            }
        }
        os<<std::endl;
        return os;
    }

    const boost::optional<Tile>& at(const Position& pos) {
        assert(index(pos) < size_squared);
        assert(comprise(pos));

        return places[index(pos)].tileMaybe;
    }

    bool comprise(const Position& pos) const{

        return pos.row < size && pos.column < size && index(pos) < size_squared;
    }

    bool vacant(const Position& pos) const {
        assert(comprise(pos));
        return places[index(pos)].vacant();
    }

    bool occupied(const Position& pos) const {
        assert(comprise(pos));
        return ! vacant(pos);
    }

    const Field& swap(const Position& left, const Position& right) {
        assert(vacant(left) || vacant(right));

        places[index(left)].tileMaybe.swap(places[index(right)].tileMaybe);

        assert(countEmpty() == 1);

        return *this;
    }

    Field swap(const Position& left, const Position& right) const {
        Field tmp(*this);
        tmp.swap(left, right);

        return tmp;
    }

    iterator begin() {
        return places.begin();
    }

    iterator end() {
        return places.end();
    }

    const_iterator begin() const {
        return places.begin();
    }
    const_iterator end() const {
        return places.end();
    }

    bool operator == (const Field& other) const {
        return other.places == places;
    }

};


std::ostream& operator<<(std::ostream& os, const Field& f) {
    return f.print(os);
}


struct CalculateHash{
    size_t* hash;

    void operator()(const Place& p){
        (*hash)+=p.tileMaybe.get_value_or(Tile(0)).value;
    }
};

namespace std {
        template<>
        struct hash< Field > {
            std::size_t operator()(const Field & c ) const
            {
                size_t hash = 0;
                CalculateHash calculateHash;
                calculateHash.hash = &hash;

                std::for_each(
                            c.begin(),
                            c.end(),
                            calculateHash
                            );

                return hash;
            }
        };
}

typedef Place Element;
typedef Field Domain;

#endif // MODEL_H
