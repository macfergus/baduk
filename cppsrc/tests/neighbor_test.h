#include <unordered_set>

#include <cxxtest/TestSuite.h>

#include "../baduk/point.h"
#include "../baduk/neighbor.h"

void assertPointsEqual(
        std::vector<baduk::Point> actual,
        std::vector<baduk::Point> expected) {
    std::unordered_set<baduk::Point> actual_set(actual.begin(), actual.end());
    std::unordered_set<baduk::Point> expected_set(
        expected.begin(), expected.end());
    TS_ASSERT_EQUALS(actual_set, expected_set);
}

class NeighborTestSuite : public CxxTest::TestSuite {
public:
    void testNeighborTable() {
        const auto neighbor_table = baduk::NeighborTable(5, 5);

        assertPointsEqual(neighbor_table.get("A1"), {"A2", "B1"});
        assertPointsEqual(neighbor_table.get("A3"), {"A2", "A4", "B3"});
        assertPointsEqual(neighbor_table.get("C3"), {"C2", "C4", "B3", "D3"});
    }

    void testGetNeighbors() {
        const auto table19 = baduk::getNeighborTable(19, 19);
        assertPointsEqual(table19->get("A1"), {"A2", "B1"});
        assertPointsEqual(table19->get("T19"), {"S19", "T18"});
        const auto table9 = baduk::getNeighborTable(9, 9);
        assertPointsEqual(table9->get("J5"), {"J4", "J6", "H5"});
    }
};
