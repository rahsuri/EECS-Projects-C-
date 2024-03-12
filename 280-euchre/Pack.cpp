// Project UID 1d9f47bfc76643019cfbf037641defe1
//  Pack.cpp
//  p3-euchre
//
//  Created by Shaunak Bhagat on 2/10/23.
//

#include <stdio.h>
#include <cassert>
#include <iostream>
#include <array>
#include "Pack.h"
#include "Card.h"
#include <string>

using namespace std;

Pack::Pack() {
    int c = 0;
    next = 0;
    //for loop alternating through suits from min to max
    for (int s = SPADES; s <= DIAMONDS; ++s) {
        Suit suit = static_cast<Suit>(s);
        //for loop iterating through rank values from seven to ace
        for (int i = NINE; i <= ACE; ++i) {
            Rank rank = static_cast<Rank>(i);
            Card card(rank, suit);
            cards[c] = card;
            ++c;
        }
    }
}

Pack::Pack(istream& pack_input) {
    string rank;
    string junk;
    string suit;
    next = 0;
    int c = 0;
    while(pack_input >> rank >> junk >> suit) {
        Card card(string_to_rank(rank), string_to_suit(suit));
        cards[c] = card;
        c++;
    }
}

Card Pack::deal_one() {
    next++;
    return cards[next - 1];
}

void Pack::reset() {
    next = 0;
    return;
}

void Pack::shuffle() {
    Card first_half[PACK_SIZE / 2];
    Card second_half[PACK_SIZE / 2];
    
    //loop seven times to fully shuffle deck
    for (int i = 0; i < 7; ++i) {
        
        //copy the first and second half of the deck into two separate arrays
        for (int j = 0; j < PACK_SIZE; ++j) {
            if (j < PACK_SIZE / 2) {
                first_half[j] = cards[j];
            }
            else {
                second_half[j - (PACK_SIZE / 2)] = cards[j];
            }
        }
        
        //refill pack alternating cards from each copied array
        for (int k = 0; k < PACK_SIZE / 2; ++k) {
            cards[2 * k] = second_half[k];
            cards[2 * k + 1] = first_half[k];
        }
    }
    
    //reset the next index
    reset();
}

bool Pack::empty() const {
    if (next >= PACK_SIZE) {
        return true;
    }
    else {
        return false;
    }
}
