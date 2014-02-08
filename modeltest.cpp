#include <gtest/gtest.h>
#include "model.h"
#include "actions.h"

TEST(FifteenTile, shouldBeVacantOrOccupied) {
    Place occupied(Position(1,1), Tile(1));
    Place vacant(Position(1,1));

    EXPECT_TRUE(occupied.occupied());
    EXPECT_FALSE(occupied.vacant());

    EXPECT_TRUE(vacant.vacant());
    EXPECT_FALSE(vacant.occupied());
}

/**
 * @brief  8|4|1
 *         2|3|_
 *         5|7|6
 */
Field testField() {
    std::vector<Place> p;


    p.push_back(Place(Position(0,0), Tile(8)));
    p.push_back(Place(Position(0,1), Tile(4)));
    p.push_back(Place(Position(0,2), Tile(1)));
    p.push_back(Place(Position(1,0), Tile(2)));
    p.push_back(Place(Position(1,1), Tile(3)));
    p.push_back(Place(Position(1,2)));
    p.push_back(Place(Position(2,0), Tile(5)));
    p.push_back(Place(Position(2,1), Tile(7)));
    p.push_back(Place(Position(2,2), Tile(6)));

    return Field(p);
}


TEST(FifteenTile, shouldBeConstructed){

    Field f = testField();

    EXPECT_EQ(f.at(Position(2,2)).get().value, 6);
    EXPECT_EQ(f.at(Position(1,2)), boost::none);
    EXPECT_TRUE(f.occupied(Position(0,0)));
    EXPECT_TRUE(f.vacant(Position(1,2)));
}

TEST(FifteenTile, shouldAcceptMovements) {
    Field f =testField();

    MoveLeft left12(Position(1,2));
    MoveRight right12(Position(1,2));
    MoveUp up12(Position(1,2));
    MoveDown down12(Position(1,2));
    MoveRight right00(Position(0,0));

    EXPECT_TRUE(left12.isDefined(f));
    EXPECT_TRUE(up12.isDefined(f));
    EXPECT_TRUE(down12.isDefined(f));
    EXPECT_FALSE(right12.isDefined(f));

    EXPECT_FALSE(right00.isDefined(f));
}

TEST(FifteenTile, shouldSwap) {
    Field f =testField();

    MoveLeft left12(Position(1,2));

    Field f_moved = left12(f);

    EXPECT_TRUE(f_moved.vacant(Position(1,1)));
    EXPECT_TRUE(f_moved.occupied(Position(1,2)));

    EXPECT_EQ(f_moved.at(Position(1,2)).get().value, 3);
}

TEST(FifteenTile, shouldConstructAsNeeded) {
    Field f(3);
    std::cout<<f;
    EXPECT_EQ(f.size, 3);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
