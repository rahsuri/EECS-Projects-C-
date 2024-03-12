// Project UID 1d9f47bfc76643019cfbf037641defe1

#include <cassert>
#include <iostream>
#include <array>
#include "Card.h"
#include <string>

using namespace std;

/////////////// Rank operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const RANK_NAMES[] = {
  "Two",   // TWO
  "Three", // THREE
  "Four",  // FOUR
  "Five",  // FIVE
  "Six",   // SIX
  "Seven", // SEVEN
  "Eight", // EIGHT
  "Nine",  // NINE
  "Ten",   // TEN
  "Jack",  // JACK
  "Queen", // QUEEN
  "King",  // KING
  "Ace"    // ACE
};

//REQUIRES str represents a valid rank ("Two", "Three", ..., "Ace")
Rank string_to_rank(const std::string &str) {
  for(int r = TWO; r <= ACE; ++r) {
    if (str == RANK_NAMES[r]) {
      return static_cast<Rank>(r);
    }
  }
  assert(false); // Input string didn't match any rank
}

//EFFECTS Prints Rank to stream, for example "Two"
std::ostream & operator<<(std::ostream &os, Rank rank) {
  os << RANK_NAMES[rank];
  return os;
}

//REQUIRES If any input is read, it must be a valid rank
//EFFECTS Reads a Rank from a stream, for example "Two" -> TWO
std::istream & operator>>(std::istream &is, Rank &rank) {
  string str;
  if(is >> str) {
    rank = string_to_rank(str);
  }
  return is;
}



/////////////// Suit operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const SUIT_NAMES[] = {
  "Spades",   // SPADES
  "Hearts",   // HEARTS
  "Clubs",    // CLUBS
  "Diamonds", // DIAMONDS
};

//REQUIRES str represents a valid suit ("Spades", "Hearts", "Clubs", or "Diamonds")
Suit string_to_suit(const std::string &str) {
  for(int s = SPADES; s <= DIAMONDS; ++s) {
    if (str == SUIT_NAMES[s]) {
      return static_cast<Suit>(s);
    }
  }
  assert(false); // Input string didn't match any suit
}

//EFFECTS Prints Suit to stream, for example "Spades"
std::ostream & operator<<(std::ostream &os, Suit suit) {
  os << SUIT_NAMES[suit];
  return os;
}

//REQUIRES If any input is read, it must be a valid suit
//EFFECTS Reads a Suit from a stream, for example "Spades" -> SPADES
std::istream & operator>>(std::istream &is, Suit &suit) {
  string str;
  if (is >> str) {
    suit = string_to_suit(str);
  }
  return is;
}


/////////////// Write your implementation for Card below ///////////////


// NOTE: We HIGHLY recommend you check out the operator overloading
// tutorial in the project spec before implementing
// the following operator overload functions:
//   operator<<
//   operator>>
//   operator<
//   operator<=
//   operator>
//   operator>=
//   operator==
//   operator!=

Card::Card()
: rank(TWO), suit(SPADES) {}

Card::Card(Rank rank_in, Suit suit_in)
: rank(rank_in), suit(suit_in) {}

Rank Card::get_rank() const {
    return rank;
}

Suit Card::get_suit() const {
    return suit;
}

Suit Card::get_suit(Suit trump) const {
    if (suit == Suit_next(trump) && rank == JACK) {
        return trump;
    }
    return suit;
}

bool Card::is_face_or_ace() const {
    if (rank >= JACK) {
        return true;
    }
    else {
        return false;
    }
}

bool Card::is_right_bower(Suit trump) const {
    if (rank == JACK && suit == trump) {
        return true;
    }
    else {
        return false;
    }
}

bool Card::is_left_bower(Suit trump) const {
    //get the suit of the same color
    Suit left = Suit_next(trump);
    
    if (rank == JACK && suit == left) {
        return true;
    }
    else {
        return false;
    }
}

bool Card::is_trump(Suit trump) const {
    if (suit == trump) {
        return true;
    }
    else if (is_left_bower(trump)) {
        return true;
    }
    else {
        return false;
    }
}

std::ostream & operator<<(std::ostream &os, const Card &card) {
    os << card.get_rank() << " of " << card.get_suit();
    return os;
}

std::istream & operator>>(std::istream &is, Card &card) {
    string rank;
    string junk;
    string suit;
    if (is >> rank >> junk >> suit) {
        card.rank = string_to_rank(rank);
        card.suit = string_to_suit(suit);
    }
    return is;
}

bool operator<(const Card &lhs, const Card &rhs) {
    if (lhs.get_rank() < rhs.get_rank()) {
        return true;
    }
    else if (lhs.get_rank() > rhs.get_rank()) {
        return false;
    }
    else if (lhs.get_suit() < rhs.get_suit()) {
        return true;
    }
    else {
        return false;
    }
}

bool operator==(const Card &lhs, const Card &rhs) {
    if (lhs.get_suit() == rhs.get_suit() && lhs.get_rank() == rhs.get_rank()) {
        return true;
    }
    else {
        return false;
    }
}

bool operator<=(const Card &lhs, const Card &rhs) {
    return lhs < rhs || lhs == rhs;
}

bool operator>(const Card &lhs, const Card &rhs) {
    return (!(lhs <= rhs));
}

bool operator>=(const Card &lhs, const Card &rhs) {
    return !(lhs < rhs);
}

bool operator!=(const Card &lhs, const Card &rhs) {
    return !(lhs == rhs);
}

Suit Suit_next(Suit suit) {
    //create an integer correlating with current suit
    int curr_suit = suit;
    
    //set the integer to the suit of the same color
    if (curr_suit <= 1) {
        curr_suit += 2;
    }
    else {
        curr_suit -= 2;
    }
    
    //return a Suit variable with the corresponding suit integer
    return static_cast<Suit>(curr_suit);
}

bool Card_less(const Card &a, const Card &b, Suit trump) {
    //returns true if a is not trump and b is trump
    if (a.get_suit(trump) != trump && b.get_suit(trump) == trump) {
        return true;
    }
    
    //returns false if a is trump and b is not trump
    if (a.get_suit(trump) == trump && b.get_suit(trump) != trump) {
        return false;
    }
    
    //accounts for jacks being more powerful
    if (!a.is_right_bower(trump) && b.is_right_bower(trump)) {
        return true;
    }
    
    if (a.is_right_bower(trump) && !b.is_right_bower(trump)) {
        return false;
    }
    
    if (!a.is_left_bower(trump) && b.is_left_bower(trump)) {
        return true;
    }
    
    if (a.is_left_bower(trump) && !b.is_left_bower(trump)) {
        return false;
    }
    
    //returns value of simple a < b comparison
    return a < b;
}

bool Card_less(const Card &a, const Card &b, const Card &led_card, Suit trump) {
    //returns true if a is not trump and b is trump
    if (a.get_suit(trump) != trump && b.get_suit(trump) == trump) {
        return true;
    }
    
    //returns false if a is trump and b is not trump
    if (a.get_suit(trump) == trump && b.get_suit(trump) != trump) {
        return false;
    }
    
    //accounts for jacks being more powerful
    if (b.is_right_bower(trump) && !a.is_right_bower(trump)) {
        return true;
    }
    
    if (!b.is_right_bower(trump) && a.is_right_bower(trump)) {
        return false;
    }
    
    if (b.is_left_bower(trump) && !a.is_left_bower(trump)) {
        return true;
    }
    
    if (!b.is_left_bower(trump) && a.is_left_bower(trump)) {
        return false;
    }
    
    //returns true if a is not led suit and b is led suit
    if (a.get_suit(trump) != led_card.get_suit(trump)
        && b.get_suit(trump) == led_card.get_suit(trump)) {
        return true;
    }
    
    //returns false is a is led suit and b is not led suit
    if (a.get_suit(trump) == led_card.get_suit(trump)
        && b.get_suit(trump) != led_card.get_suit(trump)) {
        return false;
    }
    
    //returns value of simple a < b comparison
    return a < b;
}

