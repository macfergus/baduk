#include <cxxtest/TestSuite.h>

class BoardTestSuite : public CxxTest::TestSuite {
public:
    void testNothing() {
        TS_ASSERT_EQUALS(1 + 1, 2);
    }
};
