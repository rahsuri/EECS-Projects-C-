// Project UID 1d9f47bfc76643019cfbf037641defe1
//  main.cpp
//  p3-euchre
//
//  Created by Shaunak Bhagat on 2/9/23.
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Card.h"
#include "Pack.h"
#include "Player.h"
#include <string>
#include <vector>
#include <cassert>
#include <array>
#include <algorithm>

using namespace std;

class Game {

public:
    Game(bool shfl_in, int pts_to_win_in, vector<Player*> players_in, Pack pack_in);
        
    void play();
    
private:
    std::vector<Player*> players;
    Pack pack;
    string filename;
    bool shuffle;
    int pts_to_win;

    void shuffle_cards() {
        //shuffle if the file input says to
        if (shuffle) {
            pack.shuffle();
        }
    }
    
    void deal(int &dealer, Card &upcard) {
        //index players based on the dealer
        int left_of_dealer = (dealer + 1) % 4;
        int across_dealer = (dealer + 2) % 4;
        int right_of_dealer = (dealer + 3) % 4;
        
        //deal cards to players
        for (int i = 0; i < 3; ++i) {
            players[left_of_dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 2; ++i) {
            players[across_dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 3; ++i) {
            players[right_of_dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 2; ++i) {
            players[dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 2; ++i) {
            players[left_of_dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 3; ++i) {
            players[across_dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 2; ++i) {
            players[right_of_dealer]->add_card(pack.deal_one());
        }
        for (int i = 0; i < 3; ++i) {
            players[dealer]->add_card(pack.deal_one());
        }
        
        //deal upcard
        upcard = pack.deal_one();

        //print message to players
        cout << players[dealer]->get_name() << " deals" << endl;
        cout << upcard << " turned up" << endl;
        
        //deal the rest of the cards
        for (int i = 0; i < 3; ++i) {
            pack.deal_one();
        }
    }
    
    struct make_trump_params {
        Card &upcard;
        int &dealer;
        Suit &order_up_suit;
        int &orderer;
        int &round;
    };
    
    void make_trump(make_trump_params &m) {
        
        //index each player based on their position to the dealer
        int left_of_dealer = (m.dealer + 1) % 4;
        int across_dealer = (m.dealer + 2) % 4;
        int right_of_dealer = (m.dealer + 3) % 4;
        
        for (int i = 1; i < 3; ++i) {
            m.round = i;
            
            //make trump for the first round
            if (players[left_of_dealer]->make_trump(m.upcard, false, m.round,
                                                    m.order_up_suit)) {
                cout << players[left_of_dealer]->get_name() << " orders up "
                << m.order_up_suit << endl << endl;
                m.orderer = left_of_dealer;
                return;
            }
            else {
                cout << players[left_of_dealer]->get_name() << " passes" << endl;
            }
            
            if (players[across_dealer]->make_trump(m.upcard, false, m.round,
                                                   m.order_up_suit)) {
                cout << players[across_dealer]->get_name() << " orders up "
                << m.order_up_suit << endl << endl;
                m.orderer = across_dealer;
                return;
            }
            else {
                cout << players[across_dealer]->get_name() << " passes" << endl;
            }
            
            if (players[right_of_dealer]->make_trump(m.upcard, false, m.round,
                                                     m.order_up_suit)) {
                cout << players[right_of_dealer]->get_name() << " orders up "
                << m.order_up_suit << endl << endl;
                m.orderer = right_of_dealer;
                return;
            }
            else {
                cout << players[right_of_dealer]->get_name() << " passes" << endl;
            }
            
            if (players[m.dealer]->make_trump(m.upcard, true, m.round, m.order_up_suit)) {
                cout << players[m.dealer]->get_name() << " orders up "
                << m.order_up_suit << endl << endl;
                m.orderer = m.dealer;
                return;
            }
            else {
                cout << players[m.dealer]->get_name() << " passes" << endl;
            }
        }
        
        //function should return true and change order up suit at the last if
        //statement no matter what
    }
    
    void add_and_discard(int &dealer, int &round, Card &upcard) {
        if (round == 1) {
            players[dealer]->add_and_discard(upcard);
        }
        return;
    }
    
    struct play_hand_params {
        int &leader;
        Suit &trump;
        int &ordered_team_tricks;
        int &follow_team_tricks;
        int &orderer;
    };
    
    void play_hand(play_hand_params &p) {
        
        //play five hands
        for (int i = 0; i < 5; ++i) {
            
            // create integers for the indexes of the players in order
            int second_idx = (p.leader + 1) % 4;
            int third_idx = (p.leader + 2) % 4;
            int fourth_idx = (p.leader + 3) % 4;
            
            //play cards for each player
            Card led = players[p.leader]->lead_card(p.trump);
            cout << led << " led by " << players[p.leader]->get_name() << endl;
            
            Card second = players[second_idx]->play_card(led, p.trump);
            cout << second << " played by " << players[second_idx]->get_name() << endl;
            
            Card third = players[third_idx]->play_card(led, p.trump);
            cout << third << " played by " << players[third_idx]->get_name() << endl;
            
            Card fourth = players[fourth_idx]->play_card(led, p.trump);
            cout << fourth << " played by " << players[fourth_idx]->get_name() << endl;
            
            //create a vector of the four cards played
            vector<Card> trick;
            trick.push_back(led);
            trick.push_back(second);
            trick.push_back(third);
            trick.push_back(fourth);
            
            int index_of_highest = 0;
            
            //compare each card to determine the index of the highest card
            //since led card is indexed at 0, adding the index of the highest card
            //and the index of the leader mod 4 will return the index of the player
            //who played the card
            for (int i = 1; i < 4; ++i) {
                if (!(Card_less(trick[i], trick[index_of_highest], led, p.trump))) {
                    index_of_highest = i;
                }
            }
            
            //calculates the index of the winner of the hand and assigns
            //leader to the winner
            //since led card is indexed at 0, adding the index of the highest
            //card and the
            //index of the leader mod 4 will return the index of the player who
            //played the card
            
            int index_of_winner = (p.leader + index_of_highest) % 4;
            cout << players[index_of_winner]->get_name() << " takes the trick"
            << endl << endl;
            
            //sets the leader of the next trick to the index of the winner
            //of the current trick
            p.leader = index_of_winner;
            
            //add a point to the team that won (either orderer or follower)
            if (index_of_winner == p.orderer || index_of_winner == (p.orderer + 2) % 4) {
                p.ordered_team_tricks += 1;
            }
            else {
                p.follow_team_tricks += 1;
            }
        }
    }
    
    struct update_score_params {
        int &orderer;
        int &ordered_team_tricks;
        int &follow_team_tricks;
        int &team1_score;
        int &team2_score;
    };
    
    void update_score(update_score_params &params) {
        
        //if the orderer is on team 1
        if (params.orderer % 2 == 0) {
            
            if (params.ordered_team_tricks >= 3) {
                
                //add 2 points if they march
                if (params.ordered_team_tricks == 5) {
                    params.team1_score += 2;
                    cout << players[0]->get_name() << " and " << players[2]->get_name()
                    << " win the hand" << endl;
                    cout << "march!" << endl;
                }
                else {
                    //add 1 point if didn't win all 5 tricks
                    params.team1_score += 1;
                    cout << players[0]->get_name() << " and " << players[2]->get_name()
                    << " win the hand" << endl;
                }
            }
            else {
                
                //add 2 to following team if they euchre
                params.team2_score += 2;
                cout << players[1]->get_name() << " and " << players[3]->get_name()
                << " win the hand" << endl;
                cout << "euchred!" << endl;
                
            }
        }
        
        //if the orderer is on team 2
        else {
            
            if (params.ordered_team_tricks >= 3) {
                
                //add 2 points if they march
                if (params.ordered_team_tricks == 5) {
                    params.team2_score += 2;
                    cout << players[1]->get_name() << " and " << players[3]->get_name()
                    << " win the hand" << endl;
                    cout << "march!" << endl;
                }
                else {
                    //add 1 point if didn't win all 5 tricks
                    params.team2_score += 1;
                    cout << players[1]->get_name() << " and " << players[3]->get_name()
                    << " win the hand" << endl;
                }
            }
            else {
                //add 2 to team1 if they euchre
                params.team1_score += 2;
                cout << players[0]->get_name() << " and " << players[2]->get_name()
                << " win the hand" << endl;
                cout << "euchred!" << endl;
            }
        }
        
        cout << players[0]->get_name() << " and " << players[2]->get_name()
        << " have " << params.team1_score << " points" << endl;
        cout << players[1]->get_name() << " and " << players[3]->get_name()
        << " have " << params.team2_score << " points" << endl << endl;
        
    }
    
    void prepare_next_round(int &dealer) {
        pack.reset();
        dealer = (dealer + 1) % 4;
    }
    
    bool is_game_over(int &team1_score, int &team2_score) {
        //end game and add output if team1 wins
        if (team1_score >= pts_to_win) {
            cout << players[0]->get_name() << " and "
            << players[2]->get_name() << " win!";
            return true;
        }
        
        //end game and add output if team2 wins
        else if (team2_score >= pts_to_win) {
            cout << players[1]->get_name() << " and "
            << players[3]->get_name() << " win!";
            return true;
        }
        
        //continue playing if nobody won
        else {
            return false;
        }
        
    }
    
    
};

Game::Game(bool shfl_in, int pts_to_win_in, vector<Player*> players_in, Pack pack_in) {
    shuffle = shfl_in;
    pts_to_win = pts_to_win_in;
    pack = pack_in;
    players = players_in;
}

void Game::play() {
    //initialize dealer to player 0
    int dealer = 0;
    
    //initialize the leader, which will be assigned a value later
    int leader;
    
    //create two team scores and initialize them to 0: team1 for player 0
    //and player 2, and team2 for player 1 and player 3
    int team1_score = 0;
    int team2_score = 0;
    
    //set the game to not be over
    bool game_over = false;
    
    //set the hand to be 0
    int hand = 0;
    
    //run the game until a team has won
    while (!game_over) {
        
        //output start of hand message
        cout << "Hand " << hand << endl;
        
        //shuffle the cards to start the game
        shuffle_cards();
        
        //initialize upcard without giving it a value
        Card upcard;
        
        //deal the cards
        deal(dealer, upcard);
        
        //initialize order up suit and the index of the orderer
        Suit order_up_suit;
        int orderer;
        
        //initialize the round of making trump
        int round;
        
        make_trump_params m = {upcard, dealer, order_up_suit, orderer, round};
        
        //make trump
        make_trump(m);
        
        //add and discard cards
        add_and_discard(dealer, round, upcard);
        
        //create counters for ordered team tricks and follow team tricks
        //initialize the leader, which is left of the dealer
        int ordered_team_tricks = 0;
        int follow_team_tricks = 0;
        leader = (dealer + 1) % 4;
        
        play_hand_params p = {leader, order_up_suit, ordered_team_tricks,
            follow_team_tricks, orderer};
        
        //play a hand
        play_hand(p);
        
        //update the score, adding points to the team that won
        update_score_params params = {orderer, ordered_team_tricks, follow_team_tricks,
            team1_score, team2_score};
        update_score(params);
        
        
        //check whether the game is over
        game_over = is_game_over(team1_score, team2_score);
        
        //prepare for the next round if game is not over
        if (!game_over) {
            prepare_next_round(dealer);
            ++hand;
        }
        
    }

}

int main(int argc, const char * argv[]) {

#ifdef __APPLE__
freopen("euchre_test50.in", "r", stdin);
#endif
    
    //store points to win as an integer
    int pts_to_win_in = atoi(argv[3]);
    
    //store shuffle as a string
    string shuffle_in = argv[2];
    
    //store each player type as strings
    string p0_type = argv[5];
    string p1_type = argv[7];
    string p2_type = argv[9];
    string p3_type = argv[11];
    
    if (argc != 12 || (pts_to_win_in < 1 || pts_to_win_in > 100)
        || (!(shuffle_in == "shuffle" || shuffle_in == "noshuffle"))
        || (!(p0_type == "Simple" || p0_type == "Human")
        || !(p1_type == "Simple" || p1_type == "Human")
        || !(p2_type == "Simple" || p2_type == "Human")
        || !(p3_type == "Simple" || p3_type == "Human"))) {
        cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
        << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
        << "NAME4 TYPE4" << endl;
        return 1;
    }
    
    //check if there is an error when opening the file
    ifstream ins;
    ins.open(argv[1]);
    if (!ins.is_open()) {
        cout << "Error opening " << argv[1] << endl;
    }
    
    //initialize names of players
    string p0_name = argv[4];
    string p1_name = argv[6];
    string p2_name = argv[8];
    string p3_name = argv[10];
    
    //initialize players
    vector<Player*> players_in = {Player_factory(p0_name, p0_type),
        Player_factory(p1_name, p1_type),
        Player_factory(p2_name, p2_type),
        Player_factory(p3_name, p3_type)};
    
    //initialize pack
    Pack pack_in = Pack(ins);
    
    //initialize shfl_in
    bool shfl_in;
    if (shuffle_in == "shuffle") {
        shfl_in = true;
    }
    else {
        shfl_in = false;
    }
    
    //print the command line statements
    cout << argv[0] << " " << argv[1] << " " << argv[2] << " "
    << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6]
    << " " << argv[7] << " " << argv[8] << " " << argv[9]
    << " " << argv[10] << " " << argv[11] << " " << endl;
    
    //initialize game
    Game game(shfl_in, pts_to_win_in, players_in, pack_in);
    
    //play the game
    game.play();
    cout << endl;
    
    //close file and delete players
    ins.close();
    
    delete players_in[0];
    delete players_in[1];
    delete players_in[2];
    delete players_in[3];
        
    return 0;
}
