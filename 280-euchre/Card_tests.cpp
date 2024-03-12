// Project UID 1d9f47bfc76643019cfbf037641defe1

#include "Card.h"
#include "unit_test_framework.h"
#include <iostream>

using namespace std;


TEST(test_card_ctor) {
    Card c(ACE, HEARTS);
    ASSERT_EQUAL(ACE, c.get_rank());
    ASSERT_EQUAL(HEARTS, c.get_suit());
}

TEST(test_def_ctor) {
    Card c;
    ASSERT_EQUAL(TWO, c.get_rank());
    ASSERT_EQUAL(SPADES, c.get_suit());
}

TEST(test_is_face_or_ace) {
    Card c(ACE, HEARTS);
    ASSERT_TRUE(c.is_face_or_ace());
}

TEST(test_is_right_bower) {
    Card c(JACK, HEARTS);
    ASSERT_TRUE(c.is_right_bower(HEARTS));
}

TEST(test_is_left_bower) {
    Card c(JACK, DIAMONDS);
    ASSERT_TRUE(c.is_left_bower(HEARTS));
}

TEST(test_is_trump) {
    Card c(TEN, HEARTS);
    ASSERT_TRUE(c.is_trump(HEARTS));
    
    Card c1(JACK, DIAMONDS);
    ASSERT_TRUE(c.is_trump(HEARTS));
}

TEST(test_less_than) {
    Card c1(TEN, HEARTS);
    Card c2(JACK, HEARTS);
    ASSERT_TRUE(c1 < c2);
    
    Card c3(TEN, CLUBS);
    ASSERT_TRUE(c1 < c3);
    
    Card c4(NINE, CLUBS);
    ASSERT_TRUE(c4 < c1);
}

TEST(test_less_than_or_equal_to) {
    Card c1(TEN, HEARTS);
    Card c2(TEN, HEARTS);
    ASSERT_TRUE(c1 <= c2);
    
    Card c3(NINE, CLUBS);
    ASSERT_TRUE(c3 <= c1);
}

TEST(test_greater_than) {
    Card c1(TEN, HEARTS);
    Card c2(JACK, HEARTS);
    ASSERT_TRUE(c2 > c1);
    
    Card c3(TEN, CLUBS);
    ASSERT_TRUE(c3 > c1);
}

TEST(test_greater_than_or_equal_to) {
    Card c1(TEN, HEARTS);
    Card c2(TEN, HEARTS);
    ASSERT_TRUE(c1 >= c2);
    
    Card c3(TEN, CLUBS);
    ASSERT_TRUE(c3 >= c1);
    
    Card c4(JACK, SPADES);
    ASSERT_TRUE(c4 >= c1);
}

TEST(test_equal) {
    Card c1(TEN, HEARTS);
    Card c2(TEN, HEARTS);
    ASSERT_TRUE(c1 == c2);
}

TEST(test_not_equal) {
    Card c1(TEN, HEARTS);
    Card c2(NINE, HEARTS);
    ASSERT_TRUE(c1 != c2);
    
    Card c3(TEN, CLUBS);
    ASSERT_TRUE(c1 != c3);
}

TEST(test_suit_next) {
    Suit s = CLUBS;
    ASSERT_TRUE(Suit_next(s) == SPADES);
    
    Suit s1 = SPADES;
    ASSERT_TRUE(Suit_next(s1) == CLUBS);
}

TEST(test_card_less_without_led) {
    Card a(TEN, HEARTS);
    Card b(NINE, SPADES);
    Suit trump = SPADES;
    ASSERT_TRUE(Card_less(a, b, trump));
    
    Card c(NINE, HEARTS);
    ASSERT_TRUE(Card_less(c, a, trump));
    
    ASSERT_FALSE(Card_less(b, a, trump));
    ASSERT_FALSE(Card_less(a, c, trump));
}

TEST(test_card_less_with_led) {
    Suit trump = SPADES;
    Card led(ACE, HEARTS);
    
    Card a(TEN, HEARTS);
    Card b(NINE, SPADES);
    ASSERT_TRUE(Card_less(a, b, led, trump));
    ASSERT_FALSE(Card_less(b, a, led, trump));
    
    Card c(NINE, HEARTS);
    Card d(ACE, CLUBS);
    ASSERT_TRUE(Card_less(d, c, led, trump));
    ASSERT_FALSE(Card_less(c, d, led, trump));
    
    Card e(KING, DIAMONDS);
    Card f(KING, CLUBS);
    ASSERT_TRUE(Card_less(f, e, led, trump));
    ASSERT_FALSE(Card_less(e, f, led, trump));
    
    Card g(KING, CLUBS);
    Card h(QUEEN, DIAMONDS);
    ASSERT_TRUE(Card_less(h, g, led, trump));
    ASSERT_FALSE(Card_less(g, h, led, trump));
    
}

TEST(test_get_suit_w_trump) {
    Card c(JACK, DIAMONDS);
    ASSERT_TRUE(c.get_suit(HEARTS) == HEARTS);
}
// Add more test cases here

TEST_MAIN()
