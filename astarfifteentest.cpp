#include "astar.h"
#include "model.h"
#include "actions.h"
#include <boost/optional.hpp>
#include <gtest/gtest.h>

struct AllActionsAtThisPosition {
    std::vector<FifteenAction*>* actions;

    void operator()(const Place& p){
        MoveDown* d = new MoveDown(p.position);
        MoveUp* u = new MoveUp(p.position);
        MoveLeft* l = new MoveLeft(p.position);
        MoveRight* r = new MoveRight(p.position);

        actions->push_back(l);
        actions->push_back(r);
        actions->push_back(u);
        actions->push_back(d);
    }
};

void allPossibleActions(const Field& f, std::vector<FifteenAction*>& actions) {
    AllActionsAtThisPosition actionsFunctor;
    actionsFunctor.actions = &actions;

    std::for_each(f.begin(), f.end(), actionsFunctor);
}

struct AtRightPlace{

    int size;

    bool operator()(const Place& p) {
        return p.tileMaybe == boost::none || p.tileMaybe.get().value == p.position.row*size+p.position.column+1;
    }
};

struct DisplacementHeuristic: std::unary_function<const Field&, Cost> {
    Cost operator ()(const Field& f) const{

        AtRightPlace atRightPlace;
        atRightPlace.size = f.size;

        return f.size*f.size - std::count_if(
                    f.begin(),
                    f.end(),
                    atRightPlace
                    );
    }
};

struct MovemetsToRightPlace{
    int size;

    int operator()(const Place& p){
        if(p.tileMaybe == boost::none){
            return 0;
        } else {

            int tileCol= (p.tileMaybe.get().value-1)%size;
            int tileRow= (p.tileMaybe.get().value-1)/size;

            return abs(p.position.row-tileRow) + abs(p.position.column-tileCol);
        }
    }
};

struct Summer {
    int* sum;
    Summer(int* _sum):
        sum(_sum)
    {}

    void operator ()(int i){
        (*sum)+=i;
    }
};

struct MovemetsToRightPlaceHeuristic: std::unary_function<const Field&, Cost> {
    Cost operator ()(const Field& f) const{

        MovemetsToRightPlace movements;
        movements.size = f.size;

        std::vector<int> mnts(f.size*f.size);

        std::transform(f.begin(), f.end(), mnts.begin(), movements);
        int sum=0;
        Summer s(&sum);
        std::for_each(mnts.begin(), mnts.end(), s);

        return sum;
    }
};


/**
 * @brief  8|1|7
 *         4|5|6
 *         2|_|3
 *
 *8 1 7 4 5 6 2 0 3
 */
Field testField() {
    std::vector<Place> p;


    p.push_back(Place(Position(0,0), Tile(8)));
    p.push_back(Place(Position(0,1), Tile(1)));
    p.push_back(Place(Position(0,2), Tile(7)));
    p.push_back(Place(Position(1,0), Tile(4)));
    p.push_back(Place(Position(1,1), Tile(5)));
    p.push_back(Place(Position(1,2), Tile(6)));
    p.push_back(Place(Position(2,0), Tile(2)));
    p.push_back(Place(Position(2,1)));//, Tile(2)));
    p.push_back(Place(Position(2,2), Tile(3)));

    return Field(p);
}

void printActionAndTileField(FifteenAction* a) {
    static Field field = testField();
    a->print(std::cout);
    field = (*a)(field);
    std::cout<< field;
}

void printActionAndTileFieldForAGraph(FifteenAction* a) {
    static Field field = testField();
    a->print(std::cout);
    field = (*a)(field);
    std::cout<< field;
}


TEST(AStar, shouldFindAsolutionAsATree) {

    std::vector<FifteenAction*> actions;

    Field f = testField();
    TracedDomain<Field, FifteenAction*> solution(f);

    allPossibleActions(f, actions);


    MovemetsToRightPlaceHeuristic heuristic;
    tree_plan(testField(), Field(3), heuristic, actions.begin(), actions.end(), solution);

    std::cout<<solution.actions().size()<<std::endl;

    std::for_each(
                solution.actions().begin(),
                solution.actions().end(),
                printActionAndTileField
                );

    EXPECT_TRUE(true);
}

TEST(AStar, shouldFindAsolutionAsAGraph) {

    std::vector<FifteenAction*> actions;

    Field f = testField();
    TracedDomain<Field, FifteenAction*> solution(f);

    allPossibleActions(f, actions);


    MovemetsToRightPlaceHeuristic heuristic;
    graph_plan(testField(), Field(3), heuristic, actions.begin(), actions.end(), solution);

    std::cout<<solution.actions().size()<<std::endl;

    std::for_each(
                solution.actions().begin(),
                solution.actions().end(),
                printActionAndTileFieldForAGraph
                );

    EXPECT_TRUE(true);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
