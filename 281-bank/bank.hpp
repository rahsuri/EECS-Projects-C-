//Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
//
//  bank.hpp
//  281-p3
//
//  Created by Rahul Suri on 10/26/23.
//

#ifndef bank_hpp
#define bank_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <queue>
#include <utility>
#include "getopt.h"
#include <string>
#include <fstream>

using namespace std;

class bank {
private:
    
    //get opt variables
    bool registered = false;
    string regFile = "";
    bool verbose = false;
    
    struct place {
        uint32_t id;
        unsigned long long place_time;
        string ip;
        string sender;
        string recip;
        uint32_t amt;
        unsigned long long exec;
        char cover;
    };
    
    //profile data structures
    struct profile{
        unsigned long long reg_time;
        uint32_t pin;
        uint32_t balance;
        vector<place*> incoming;
        vector<place*> outgoing;
        bool hist;
        unordered_set<string> ips;
    };
    
    //storing our user profiles, profiles[id] = profile;
    // and their corresponding valid ips
    unordered_map<string, profile> users;
    
    //current user(s) that is/are logged in, and the current time
    vector<string> curr_users;
    unsigned long long curr_time = 000000000000;
    
    //transaction placement data structures
    
    uint32_t trans_id = 0;
    
    class placecomp {
    public:
        bool operator()(place* &place1, place* &place2) const {
            if (place1->exec < place2->exec) {
                return true;
            }
            else if (place1->exec > place2->exec) {
                return false;
            }
            else {
                return (place1->id < place2->id);
            }
        }
        bool operator()(place* lhs, unsigned long long rhs) const {
                // Your comparison logic here
                // For example:
                return lhs->exec <= rhs; // Replace 'someMember' with the actual member to compare
        }
        
        bool operator()(unsigned long long lhs, place* rhs) const {
                // Your comparison logic here
                // For example:
                return lhs >= rhs->exec; // Replace 'someMember' with the actual member to compare
        }
    };
    
    class placecomp2 {
    public:
        bool operator()(place* &place1, place* &place2) const {
            if (place1->exec < place2->exec) {
                return false;
            }
            else if (place1->exec > place2->exec) {
                return true;
            }
            else {
                return (place1->id > place2->id);
            }
        }
    };
    
    deque<place> placed;
    priority_queue<place*, vector<place*>, placecomp2> transactions;
    vector<place*> executed;
    
public:
    
    //running command line options
    void printHelp();
    void get_opts(int c, char ** v);
    
    //registering users
    unsigned long long convert_time(string time);
    void registration();
    
    //-handling commands-
    
    //login
    void login(string id, uint32_t pin, string ip);
    
    //logout
    void logout(string id, string ip);
    
    //placing transactions / executing them
    bool discounted(string user, place transaction);
    void execute_trans();
    
    bool fraud_check(place placement);
    void place_trans(place placement);
    
    //getting commands from user
    void get_commands();
    
    //after commands are read, get queries
    void list(unsigned long long x, unsigned long long y);
    void revenue(unsigned long long x, unsigned long long y);
    void history(string id);
    void summarize(unsigned long long date);
    
    void get_queries();
    
    //our in-class tests
    void run_tests();
};




#endif /* bank_hpp */

