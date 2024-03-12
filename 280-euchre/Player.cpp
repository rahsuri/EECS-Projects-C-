// Project UID 1d9f47bfc76643019cfbf037641defe1
//  Player.cpp
//  p3-euchre
//
//  Created by Shaunak Bhagat on 2/10/23.
//

#include <stdio.h>
#include "Card.h"
#include "Player.h"
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <array>
#include <algorithm>

using namespace std;

class Simple : public Player {
public:
    const std::string & get_name() const override;
    
    void add_card(const Card &c) override;
    
    bool make_trump(const Card &upcard, bool is_dealer,
                    int round, Suit &order_up_suit) const override;
    
    void add_and_discard(const Card &upcard) override;
    
    Card lead_card(Suit trump) override;
    
    Card play_card(const Card &led_card, Suit trump) override;
    
    Simple(string name_in);
    

private:
    string name;
    vector<Card> hand;
};

class Human : public Player {
public:
    const std::string & get_name() const override;
    
    void add_card(const Card &c) override;
    
    bool make_trump(const Card &upcard, bool is_dealer,
                    int round, Suit &order_up_suit) const override;
    
    void add_and_discard(const Card &upcard) override;
    
    Card lead_card(Suit trump) override;
    
    Card play_card(const Card &led_card, Suit trump) override;
    
    Human(string name_in);
    

private:
    string name;
    vector<Card> hand;
    void print_hand() const;
};

Player * Player_factory(const std::string &name, const std::string &strategy) {
    // Check the value of strategy and return the corresponding player type.
    if (strategy == "Simple") {
        // The "new" keyword dynamically allocates an object.
        return new Simple(name);
      }
      // Repeat for each other type of Player
    if (strategy == "Human") {
        // Dynamically allocate human object
        return new Human(name);
    }
    // Invalid strategy if we get here
    assert(false);
    return nullptr;
}

std::ostream & operator<<(std::ostream &os, const Player &p) {
    os << p.get_name();
    return os;
}

Simple::Simple(string name_in)
: name(name_in) {}

const std::string & Simple::get_name() const {
    return name;
}

void Simple::add_card(const Card &c) {
    assert(hand.size() < MAX_HAND_SIZE);
    hand.push_back(c);
    return;
}

bool Simple::make_trump(const Card &upcard, bool is_dealer,
                        int round, Suit &order_up_suit) const {
    // make sure it is either round one or two
    assert(round == 1 || round == 2);
    
    // create rule for round 1
    if (round == 1) {
        
        // initialize the amount of face or ace of trump suit to 0
        int face_or_ace_trump = 0;
        
        // go through the hand and count all of the face or ace cards the player
        // has of the trump suit
        for (auto h:hand) {
            if (h.is_face_or_ace() && h.is_trump(upcard.get_suit())) {
                face_or_ace_trump += 1;
            }
        }
        
        // if the player has two or more face or ace cards of trump suit,
        //they will make trump
        if (face_or_ace_trump >= 2) {
            order_up_suit = upcard.get_suit();
            return true;
        }
        else {
            return false;
        }
    }
    
    // create rule for round 2
    if (round == 2) {
       
        // if person is dealer, returns other suit of the same color
        if (is_dealer) {
            order_up_suit = Suit_next(upcard.get_suit());
            return true;
        }
        
        // if person is not dealer, checks if they have a face or ace of the
        // same color in their hand
        else {
            
            // loops through player's hand, and if they have a face or ace of next suit
            //, they will make
            for (auto h:hand) {
                if (h.is_face_or_ace() && h.is_trump(Suit_next(upcard.get_suit()))) {
                    order_up_suit = Suit_next(upcard.get_suit());
                    return true;
                }
            }
            
            // returns false if there are no face or ace of next suit in person's hand
            return false;
        }
    }
    return false;
}

void Simple::add_and_discard(const Card &upcard) {
    assert(hand.size() >= 1);
    
    //add the upcard to the player's hand
    hand.push_back(upcard);
    
    int index_of_min = 0;
    
    //find index of the minimum card in hand
    for (int i = 1; i < hand.size(); ++i) {
        if (Card_less(hand[i], hand[index_of_min], upcard.get_suit())) {
            index_of_min = i;
        }
    }
    
    //remove the minimum card
    hand.erase(hand.begin() + index_of_min);
}

Card Simple::lead_card(Suit trump) {
    assert(hand.size() >= 1);
    
    //set the amount of trump cards in the player's hand to 0
    int trumps_in_hand = 0;
    
    //loop through the player's hand and count how many trumps they have
    for (auto h:hand) {
        if (h.get_suit(trump) == trump) {
            trumps_in_hand += 1;
        }
    }
    
    //if they have all trumps, simply play the maximum card
    if (trumps_in_hand == hand.size()) {
        
        //find maximum card
        int index_of_max = 0;
        
        for (int i = 1; i < hand.size(); ++i) {
            
            if (!(Card_less(hand[i], hand[index_of_max], trump))) {
                index_of_max = i;
            }
        }
        
        //return and erase maximum card
        Card removed_card = hand[index_of_max];
        hand.erase(hand.begin() + index_of_max);
        return removed_card;
    }
    
    //for when they have non-trumps in hand, find the maximum non-trump
    int max_non_trump = 0;
    
    //find first non-trump card in hand
    for (int j = 0; j < hand.size(); ++j) {
        if (hand[j].get_suit(trump) != trump) {
            max_non_trump = j;
            break;
        }
    }
    
    //index from first non-trump card to find the maximum non trump card
    for (int k = max_non_trump + 1; k < hand.size(); ++k) {
        if (hand[k].get_suit(trump) != trump &&
            !(Card_less(hand[k], hand[max_non_trump], trump))) {
            max_non_trump = k;
        }
    }
    
    //return and erase the non-trump card with the highest value
    Card removed_card1 = hand[max_non_trump];
    hand.erase(hand.begin() + max_non_trump);
    return removed_card1;
}

Card Simple::play_card(const Card &led_card, Suit trump) {
    assert(hand.size() >= 1);
    
    //check if the player does not have any of led card suit
    int led_suit_in_hand = 0;
    
    //loop through the player's hand and count how many of led suit they have
    for (auto h:hand) {
        if (h.get_suit(trump) == led_card.get_suit(trump)) {
            led_suit_in_hand += 1;
        }
    }
    
    //if all of their cards are not led
    if (led_suit_in_hand == 0) {
        int index_of_min = 0;
        
        //find index of the minimum card in hand
        for (int i = 1; i < hand.size(); ++i) {
            if (Card_less(hand[i], hand[index_of_min], trump)) {
                index_of_min = i;
            }
        }
        Card card_played = hand[index_of_min];
        hand.erase(hand.begin() + index_of_min);
        return card_played;
    }
    
    //if they have a card that is of the led suit, then play the highest led card
    int max_led = 0;
    
    //find first card of led suit in hand
    for (int j = 0; j < hand.size(); ++j) {
        if (hand[j].get_suit(trump) == led_card.get_suit(trump)) {
            max_led = j;
            break;
        }
    }
    
    //index from first non-trump card to find the maximum non trump card
    for (int k = max_led + 1; k < hand.size(); ++k) {
        if (hand[k].get_suit(trump) == led_card.get_suit(trump) &&
            !(Card_less(hand[k], hand[max_led], trump))) {
            max_led = k;
        }
    }
    
    Card highest_led = hand[max_led];
    hand.erase(hand.begin() + max_led);
    return highest_led;
}

Human::Human(string name_in)
: name(name_in) {}

void Human::print_hand() const {
  for (size_t i=0; i < hand.size(); ++i)
    cout << "Human player " << name << "'s hand: "
         << "[" << i << "] " << hand[i] << "\n";
}

const std::string & Human::get_name() const {
    return name;
}

void Human::add_card(const Card &c) {
    assert(hand.size() < MAX_HAND_SIZE);
    hand.push_back(c);
    sort(hand.begin(), hand.end());
    return;
}

bool Human::make_trump(const Card &upcard, bool is_dealer,
                       int round, Suit &order_up_suit) const {
    
    assert(round == 1 || round == 2);
    
    //print hand and request player to order up or pass
    print_hand();
    cout << "Human player " << name << ", please enter a suit, or \"pass\":\n";
    
    //change order up suit depending on their decision
    string decision;
    cin >> decision;

    if (decision != "pass") {
      order_up_suit = string_to_suit(decision);
        return true;
    }
    
    else {
        return false;
    }
}

void Human::add_and_discard(const Card &upcard) {
    assert(hand.size() >= 1);
    
    print_hand();
    
    cout << "Discard upcard: [-1]\n";
    cout << "Human player " << name << ", please select a card to discard:\n";
    
    int decision;
    cin >> decision;
    
    //returns the same hand if they want to discard upcard
    if (decision == -1) {
        return;
    }
    
    //erases card chosen to discard and adds upcard to hand
    //if they choose a different card
    else {
        hand.erase(hand.begin() + decision);
        add_card(upcard);
        return;
    }
}

Card Human::lead_card(Suit trump) {
    assert(hand.size() >= 1);
    
    //show hand and prompt user to select a card
    print_hand();
    cout << "Human player " << name << ", please select a card:\n";
    
    //read in their decision
    int decision;
    cin >> decision;
    
    //remove correct card from their hand
    Card played_card = hand[decision];
    hand.erase(hand.begin() + decision);
    return played_card;
}

Card Human::play_card(const Card &led_card, Suit trump) {
    assert(hand.size() >= 1);
    
    //show hand and prompt user to select a card
    print_hand();
    cout << "Human player " << name << ", please select a card:\n";
    
    //read in their decision
    int decision;
    cin >> decision;
    
    //remove correct card from their hand
    Card played_card = hand[decision];
    hand.erase(hand.begin() + decision);
    return played_card;
}

