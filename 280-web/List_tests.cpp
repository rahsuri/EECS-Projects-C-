// Project UID c1f28c309e55405daf00c565d57ff9ad

#include "List.h"
#include "unit_test_framework.h"

using namespace std;

// Add your test cases here

TEST(test_stub) {
    // Add test code here
    ASSERT_TRUE(true);
}
TEST(test_construct) {
    List<int> test;
    
    ASSERT_TRUE(test.empty());
    ASSERT_EQUAL(test.size(), 0);
}
TEST(test_copy_const) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    
    List<int> test2(test1);
    
    ASSERT_EQUAL(test1.front(), test2.front());
}

TEST(test_assignop) {
    List<int> test1;
    List<int> test2;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    
    test2 = test1;
    
    ASSERT_EQUAL(test1.front(), test2.front());
}
TEST(test_empty1) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    
    ASSERT_FALSE(test1.empty());
}
TEST(test_empty2) {
    List<int> test1;
    
    ASSERT_TRUE(test1.empty());
}
TEST(test_size1) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    
    ASSERT_EQUAL(test1.size(), 2);
}
TEST(test_size2) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_front(4);
    
    ASSERT_EQUAL(test1.size(), 4);
}
TEST(test_front) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_front(4);
    
    ASSERT_EQUAL(test1.front(), 4);
}
TEST(test_back) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_front(4);
    
    ASSERT_EQUAL(test1.back(), 1);
}
TEST(test_pushfront) {
    List<int> test1;
    
    test1.push_front(1);
    
    ASSERT_EQUAL(test1.front(), 1);
    ASSERT_EQUAL(test1.back(), 1);
}
TEST(test_pushback1) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_back(4);
    
    ASSERT_EQUAL(test1.back(), 4);
}
TEST(test_pushback2) {
    List<int> test1;
    
    test1.push_back(1);
    
    ASSERT_EQUAL(test1.back(), 1);
    ASSERT_EQUAL(test1.front(), 1);
}
TEST(test_popfront1) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_front(4);
    test1.pop_front();
    
    ASSERT_EQUAL(test1.front(), 3);
}
TEST(test_popfront2) {
    List<int> test1;
    
    test1.push_front(1);
    test1.pop_front();
    
    ASSERT_TRUE(test1.empty());
}
TEST(test_popback1) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_front(4);
    test1.pop_back();
    
    ASSERT_EQUAL(test1.back(), 2);
}
TEST(test_popback2) {
    List<int> test1;
    
    test1.push_front(1);
    test1.pop_back();
    
    ASSERT_TRUE(test1.empty());
}
TEST(test_clear1) {
    List<int> test1;
    
    test1.push_front(1);
    test1.push_front(2);
    test1.push_front(3);
    test1.push_front(4);
    test1.clear();
    
    ASSERT_TRUE(test1.empty());
}
TEST(test_clear2) {
    List<int> test1;
    
    test1.clear();
    
    ASSERT_TRUE(test1.empty());
}

//iterator tests
TEST(test_begin) {
    List<int> test;
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    // Test begin() iterator
    List<int>::Iterator it = test.begin();
    ASSERT_EQUAL(*it, 1);
}
TEST(test_inc_op) {
    List<int> test;
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it = test.begin();
    
    //test ++ operator
    ++it;
    
    ASSERT_EQUAL(*it, 2);
}
TEST(test_unequal_op1) {
    List<int> test;
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it1 = test.begin();
    List<int>::Iterator it2 = test.begin();
    
    ++it1;
    
    ASSERT_TRUE(it1 != it2);
}
TEST(test_unequal_op2) {
    List<int> test;
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it1 = test.begin();
    List<int>::Iterator it2 = test.begin();
    
    
    ASSERT_FALSE(it1 != it2);
}
TEST(test_erase) {
    List<int> test;
    
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it = test.begin();
    
    test.erase(it);
    
    ASSERT_EQUAL(test.front(), 2);
    ASSERT_NOT_EQUAL(test.size(), 3);
}
TEST(test_insert1) {
    List<int> test;
    
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it = test.begin();
    
    test.insert(it, 5);
    
    ASSERT_EQUAL(test.front(), 5);
}
TEST(test_insert2) {
    List<int> test;
    
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it = test.begin();
    ++it;
    
    test.insert(it, 5);
    
    List<int>::Iterator it2 = test.begin();
    ++it2;
    
    ASSERT_EQUAL(*it2, 5);
}
TEST(test_insert3) {
    List<int> test;
    
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);

    List<int>::Iterator it = test.begin();
    ++it;
    ++it;
    
    test.insert(it, 5);
    
    List<int>::Iterator it2 = test.begin();
    ++it2;
    ++it2;
    
    ASSERT_EQUAL(*it2, 5);
    ASSERT_EQUAL(test.back(), 3);
}
TEST(test_popandpush1) {
    List<int> test;
    
    test.push_front(1);
    test.pop_back();
    test.push_front(2);
    
    ASSERT_EQUAL(test.front(), 2);
}
TEST(test_popandpush2) {
    List<int> test;
    
    test.push_back(1);
    test.pop_front();
    test.push_back(2);
    
    ASSERT_EQUAL(test.front(), 2);
}
TEST(test_popandpush3) {
    List<int> test;
    
    test.push_front(1);
    test.pop_back();
    
    ASSERT_TRUE(test.size() == 0);
}
TEST(test_popandpush4) {
    List<int> test;
    
    test.push_back(1);
    test.pop_front();
    
    ASSERT_TRUE(test.size() == 0);
}
TEST(test_equalityop1) {
    List<int> test;
    
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);
    
    List<int>::Iterator it1 = test.begin();
    List<int>::Iterator it2 = test.begin();
    
    ASSERT_TRUE(it1 == it2);
}
TEST(test_equalityop2) {
    List<int> test;
    
    test.push_front(3);
    test.push_front(2);
    test.push_front(1);
    
    List<int>::Iterator it1 = test.begin();
    List<int>::Iterator it2 = test.begin();
    
    ++it1;
    
    ASSERT_FALSE(it1 == it2);
}


TEST_MAIN()
