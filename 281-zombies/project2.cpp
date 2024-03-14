//Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
//
//  zombbb.cpp
//  281-project2
//
//  Created by Rahul Suri on 9/29/23.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <utility>
#include "getopt.h"
#include "P2random.h"

using namespace std;

//zombie struct & comparators
struct zombie {
    string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t activity = 1;
    
    bool active = true;
};

class zombbb {
public:
    
    //constructor
    zombbb() {};
    
    ~zombbb() {
        for (deque<zombie*>::iterator it = masterlist.begin(); it != masterlist.end(); ++it) {
            delete (*it);
        }
    }
    
    //getting command line input
    void get_mode(int argc, char ** argv);
    
    //help message
    void printHelp();
    
    //create intial variables
    P2random get_random();
    
    //move zombies
    void move_zomb();
    
    //read-in zombies
    void read_zomb(P2random &random);
    
    //shoot zombies
    void shoot_zomb();
    
    //running the rounds
    void run_defround(uint32_t &curr_round);
    void run_inround(uint32_t &curr_round, P2random &random);
    
    void push_median(uint32_t act);
    
    void print_median();
    
    void print_stats();
    
    //program execute
    void run_program();
    
    //random generation varibales
    uint32_t seed = 0;
    uint32_t maxDist = 0;
    uint32_t maxSpeed = 0;
    uint32_t maxHealth = 0;
    
private:
    struct zombie {
        string name;
        uint32_t distance;
        uint32_t speed;
        uint32_t health;
        uint32_t activity = 1;
        
        bool active = true;
    };
    
    class zombieETAComparator {
    public:
        bool operator()(zombie* &zomb2, zombie* &zomb1) const {
            if ((zomb1->distance / zomb1->speed) == (zomb2->distance / zomb2->speed)) {
                if (zomb1->health == zomb2->health) {
                    return zomb1->name < zomb2->name;
                }
                else {
                    return zomb1->health < zomb2->health;
                }
            }
            else {
                return (zomb1->distance / zomb1->speed) < (zomb2->distance / zomb2->speed);
            }
        }
    };
    
    class ActivityGreater {
    public:
        bool operator()(zombie* &zomb1, zombie* &zomb2) const {
            if (zomb1->activity == zomb2->activity) {
                return zomb1->name > zomb2->name;
            }
            else {
                return zomb1->activity > zomb2->activity;
            }
        }
    };
    class ActivityLess {
    public:
        bool operator()(zombie* &zomb1, zombie* &zomb2) const {
            if (zomb1->activity == zomb2->activity) {
                return zomb1->name > zomb2->name;
            }
            else {
                return zomb1->activity < zomb2->activity;
            }
        }
    };
    
    //flag variables
    bool verbose = false;
    bool median = false;
    size_t statistic = 0;
    
    //round stats
    uint32_t curr_round = 1;
    bool victory = false;
    bool defeat = false;
    
    //our player
    struct player {
        uint32_t quiver_capacity = 0;
        uint32_t arrows = 0;
    };
    
    //data containers
    
    //stores all of our zombies, in the order that they were created.
    deque<zombie*> masterlist;
    
    //list of all the targets the player should attack - strongest zombies at the top
    priority_queue<zombie*, vector<zombie*>, zombieETAComparator> targets;
    
    //list of dead zombies
    deque<zombie*> dead_zombies;
    
    player slayer;
    zombie* killer = nullptr;
    
    //verbose commands
    void ver_round() {
        cout << "Round: " << curr_round << "\n";
    }
    
    void ver_create(zombie &zomb) {
        cout << "Created: " << zomb.name
        << " (distance: " << zomb.distance
        << ", speed: " << zomb.speed
        << ", health: " << zomb.health << ")\n";
    }
    
    void ver_move(zombie &zomb) {
        cout << "Moved: " << zomb.name
        << " (distance: " << zomb.distance
        << ", speed: " << zomb.speed
        << ", health: " << zomb.health << ")\n";
    }
    
    void ver_destroy(zombie &zomb) {
        cout << "Destroyed: " << zomb.name
        << " (distance: " << zomb.distance
        << ", speed: " << zomb.speed
        << ", health: " << zomb.health << ")\n";
    }
    
    //median data structures
    priority_queue<uint32_t, vector<uint32_t>, less<uint32_t>> firstHalf;
    priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> secHalf;
    uint32_t true_median = 0;
    
    //stats data structures
    priority_queue<zombie*, vector<zombie*>, ActivityLess> leastActivity;
    priority_queue<zombie*, vector<zombie*>, ActivityGreater> mostActivity;
}; //zombbb class

void zombbb::get_mode(int argc, char ** argv) {
    // These are used with getopt_long()
    opterr = false; //"opting" out of letting the program handle our errors for us (false)
    int choice; //option that our user chooses
    int choice_index = 0;
    struct option long_options[] = {
        {"statistics", required_argument, nullptr, 's'},
        {"verbose", no_argument, nullptr, 'v'},
        {"median", no_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'}
  };  // long_options[]

  while ((choice = getopt_long(argc, argv, "s:vmh", long_options, &choice_index)) != -1) {
      switch (choice) {
          case 's':
              statistic = static_cast<size_t>(atoi(optarg));
              break;
          case 'v':
              verbose = true;
              break;
          case 'm':
              median = true;
              break;
          case 'h':
              printHelp();
              exit(0);
    }  // switch ..choice
  }  // while
}  // if ..mode
// getMode()

void zombbb::printHelp() {
    cout << "there are three options: \n"
         << "--statistics/-s <num>\n"
         << "--verbose/-v\n"
         << "--median/-m\n"
         << "Read the p2 spec to find out which one you want to use.";
        
    exit(0);
}//printHelp()

P2random zombbb::get_random() {
    P2random random;
    string junk;
    
    cin >> junk >> slayer.quiver_capacity
        >> junk >> seed
        >> junk >> maxDist
        >> junk >> maxSpeed
        >> junk >> maxHealth;
        
    random.initialize(seed, maxDist, maxSpeed, maxHealth);
    
    return random;
}

void zombbb::move_zomb() {
    if (!masterlist.empty()) {
        for (deque<zombie*>::iterator it = masterlist.begin(); it != masterlist.end(); ++it) {
            if ((*it)->active == true) {
                if ((*it)->distance <= (*it)->speed && killer==nullptr) {
                    killer = *it;
                    defeat = true;
                    (*it)->distance = 0;
                }
                else if ((*it)->distance <= (*it)->speed && killer != nullptr) {
                    (*it)->distance = 0;
                }
                else {
                    (*it)->distance -= (*it)->speed;
                }
                if (verbose == true) {
                    ver_move(**it);
                }
                ((*it)->activity)++;
            }
        }
    }
}

void zombbb::read_zomb(P2random &random) {
    string junk;
    uint32_t random_zombies;
    uint32_t named_zombies;
    cin >> junk >> random_zombies >> junk >> named_zombies;
    if (random_zombies > 0) {
        for (uint32_t i = 0; i < random_zombies; i++) {
            string r_name;
            uint32_t r_distance;
            uint32_t r_speed;
            uint32_t r_health;
            r_name = random.getNextZombieName();
            r_distance = random.getNextZombieDistance();
            r_speed = random.getNextZombieSpeed();
            r_health = random.getNextZombieHealth();
            zombie* curr_zombie = new zombie{r_name, r_distance, r_speed, r_health};
            masterlist.push_back(curr_zombie);
            targets.push(curr_zombie);
            if (verbose == true) {
                ver_create(*curr_zombie);
            }
        }
    }
    if (named_zombies > 0) {
        string n_name;
        uint32_t n_distance;
        uint32_t n_speed;
        uint32_t n_health;
        for (uint32_t i = 0; i < named_zombies; i++) {
            cin >> n_name >> junk >> n_distance >> junk >> n_speed >> junk >> n_health;
            zombie* curr_zombie = new zombie{n_name, n_distance, n_speed, n_health};
            masterlist.push_back(curr_zombie);
            targets.push(curr_zombie);
            if (verbose == true) {
                ver_create(*curr_zombie);
            }
        }
    }
}

//idea: swap minheap and maxheap since they are wrong anyway
//void zombbb::push_median(uint32_t act) {
//    if (maxheap.empty() && minheap.empty()) {
//        maxheap.push(act);
//    }
//    else if (minheap.empty()) {
//        if (act < maxheap.top()) {
//            minheap.push(act);
//        }
//        else {
//            minheap.push(maxheap.top());
//            maxheap.pop();
//            maxheap.push(act);
//        }
//    }
//    else if (maxheap.size() == minheap.size()) {
//        if (act > minheap.top()) {
//            maxheap.push(act);
//        }
//        else {
//            minheap.push((maxheap.top()));
//            maxheap.pop();
//            maxheap.push(act);
//        }
//    }
//    else {
//        if (maxheap.size() > minheap.size()) {
//            if (act > maxheap.top()) {
//                minheap.push((maxheap.top()));
//                maxheap.pop();
//                maxheap.push(act);
//            }
//            else {
//                minheap.push(act);
//            }
//        }
//        //maxheap.size < minheap.size
//        else {
//            if (act > maxheap.top()) {
//                maxheap.push(act);
//            }
//            else {
//                maxheap.push(minheap.top());
//                minheap.pop();
//                minheap.push(act);
//            }
//        }
//    }
//}

//V2
void zombbb::push_median(uint32_t act) {
    if (firstHalf.empty()) {
        firstHalf.push(act);
    }
    else if (secHalf.empty()) {
        if (act >= firstHalf.top()) {
            secHalf.push(act);
        }
        else {
            secHalf.push(firstHalf.top());
            firstHalf.pop();
            firstHalf.push(act);
        }
    }
    else if (firstHalf.size() == secHalf.size()) {
        if (act <= firstHalf.top()) {
            firstHalf.push(act);
        }
        else {
            firstHalf.push(secHalf.top());
            secHalf.pop();
            secHalf.push(act);
        }
    }
    else {
        if (act >= firstHalf.top()) {
            secHalf.push(act);
        }
        else {
            secHalf.push(firstHalf.top());
            firstHalf.pop();
            firstHalf.push(act);
        }
    }
}

//void zombbb::print_median() {
//    if (!dead_zombies.empty()) {
//        if (maxheap.size() == minheap.size()) {
//            true_median = (maxheap.top() + minheap.top()) / 2;
//        }
//        else if (maxheap.size() > minheap.size()) {
//            true_median = maxheap.top();
//        }
//        else {
//            true_median = minheap.top();
//        }
//        cout << "At the end of round " << curr_round << ", the median zombie lifetime is " << true_median << "\n";
//    }
//    else {
//        cout << "At the end of round " << curr_round << ", the median zombie lifetime is " << true_median << "\n";
//    }
//}

//V2
void zombbb::print_median() {
    if (!dead_zombies.empty()) {
        if (firstHalf.size() == secHalf.size()) {
            true_median = (firstHalf.top() + secHalf.top()) / 2;
        }
        else {
            true_median = firstHalf.top();
        }
        cout << "At the end of round " << curr_round << ", the median zombie lifetime is " << true_median << "\n";
    }
}

//V2
void zombbb::shoot_zomb() {
    if (!targets.empty()) {
        while (slayer.arrows > 0) {
            if (!targets.empty()) {
                //current zombie we want to shoot
                zombie* curr_zombie = targets.top();
                
                //if the zombie has > 0 health, shoot them.
                if (curr_zombie->health > slayer.arrows) {
                    curr_zombie->health -= slayer.arrows;
                    slayer.arrows = 0;
                }
                else if (curr_zombie->health < slayer.arrows) {
                    slayer.arrows -= curr_zombie->health;
                    curr_zombie->health = 0;
                }
                else {
                    curr_zombie-> health = 0;
                    slayer.arrows = 0;
                }
                //if the zombie has a health of 0, they are dead.
                if (curr_zombie->health == 0) {
                    //push to dead_zombies deque and set active to false.
                    curr_zombie->active = false;
                    dead_zombies.push_front(curr_zombie);
                    
                    //if verbose is true, do verbose stuff.
                    if (verbose == true) {
                        ver_destroy(*curr_zombie);
                    }
                    
                    //if median is true, do median stuff.
                    if (median == true) {
                        uint32_t act = curr_zombie->activity;
                        push_median(act);
                    }
                    
                    //lastly remove zombies from targets.
                    targets.pop();
                }
            }
            else {
                slayer.arrows = 0;
            }
        }
    }
} //shoot_zomb()

//void zombbb::shoot_zomb() {
//    if (!targets.empty()) {
//        for (uint32_t i = slayer.arrows; i > 0; i--) {
//            if (!targets.empty()) {
//                //current zombie we want to shoot
//                zombie* curr_zombie = targets.top();
//
//                //if the zombie has > 0 health, shoot them.
//                if (curr_zombie->health != 0) {
//                    curr_zombie->health--;
//                }
//                //if the zombie has a health of 0, they are dead.
//                if (curr_zombie->health == 0) {
//                    //push to dead_zombies deque and set active to false.
//                    curr_zombie->active = false;
//                    dead_zombies.push_front(curr_zombie);
//
//                    //if verbose is true, do verbose stuff.
//                    if (verbose == true) {
//                        ver_destroy(*curr_zombie);
//                    }
//
//                    //if median is true, do median stuff.
//                    if (median == true) {
//                        uint32_t act = curr_zombie->activity;
//                        push_median(act);
//                    }
//
//                    //lastly remove zombies from targets.
//                    targets.pop();
//                }
//            }
//            else {
//                slayer.arrows = 0;
//            }
//        }
//    }
//} //shoot_zomb()

void zombbb::run_defround(uint32_t &curr_round) {
    //1. printing round if verbose is enabled
    if (verbose == true) {
        ver_round();
    }
    
    //2. refilling the quiver to max capacity
    slayer.arrows = slayer.quiver_capacity;
    
    //3. move currently alive zombies
    move_zomb();
    
    //4a. check if the round is lost.
    if (defeat == true) {
        //printing defeat message
        cout << "DEFEAT IN ROUND " << curr_round << "! "
        << killer->name << " ate your brains!\n";
    }
    
    //4b. if the round isn't lost, continue.
    else {
        //5. shoot zombies
        shoot_zomb();
        
        //6. median stuff
        if (median == true) {
            print_median();
        }
    }
    curr_round++;
}


void zombbb::run_inround(uint32_t &curr_round, P2random &random) {
    
    //1. printing round if verbose is enabled
    if (verbose == true) {
        ver_round();
    }
    
    //2. refilling the quiver to max capacity
    slayer.arrows = slayer.quiver_capacity;
    
    //3. moving zombies
    move_zomb();
    
    //4a. checking to see if round was lost.
    if (defeat == true) {
        //printing defeat message
        cout << "DEFEAT IN ROUND " << curr_round << "! "
        << killer->name << " ate your brains!\n";
    }
    
    //4b. else, continue.
    else {
        
        //5. create zombies
        read_zomb(random);
        
        //6. shoot zombies
        shoot_zomb();
        
        //7. median stuff
        if (median == true) {
            print_median();
        }
    }
    curr_round++;
} //run_round()

//void zombbb::print_stats() {
//    cout << "Zombies still active: " << targets.size() << "\n";
//    cout << "First zombies killed:\n";
//    if (statistic > dead_zombies.size()) {
//        size_t j = dead_zombies.size();
//        for (size_t i = (dead_zombies.size() - 1); i > 0; i--) {
//            cout << (dead_zombies[i])->name << " " << j-i << "\n";
//        }
//        cout << (dead_zombies[0])->name << " " << "1\n";
//        cout << "Last zombies killed:\n";
//        for (size_t i = 0; i < dead_zombies.size(); i++) {
//            cout << (dead_zombies[i])->name << " " << j-i << "\n";
//        }
//    }
//    else {
//        for (uint32_t i = 0; i < statistic; i++) {
//            cout << (dead_zombies[dead_zombies.size() - i - 1])->name << " " << i + 1 << "\n";
//        }
//        cout << "Last zombies killed:\n";
//        for (size_t i = 0; i < statistic; i++) {
//            cout << (dead_zombies[i])->name << " " << statistic-i << "\n";
//        }
//    }
//    if (!masterlist.empty()) {
//        for (deque<zombie*>::iterator it = masterlist.begin(); it != masterlist.end(); ++it) {
//            leastActivity.push(*it);
//            mostActivity.push(*it);
//        }
//        cout << "Most active zombies:\n";
//        if (statistic > static_cast<uint32_t>(mostActivity.size())) {
//            size_t act_size = mostActivity.size();
//            for (size_t i = 0; i < act_size; i++) {
//                cout << leastActivity.top()->name << " " << leastActivity.top()->activity << "\n";
//                leastActivity.pop();
//            }
//            cout << "Least active zombies:\n";
//            for (size_t i = 0; i < act_size; i++) {
//                cout << mostActivity.top()->name << " " << mostActivity.top()->activity << "\n";
//                mostActivity.pop();
//            }
//
//        }
//        else {
//            for (size_t i = 0; i < statistic; i++) {
//                cout << leastActivity.top()->name << " " << leastActivity.top()->activity << "\n";
//                leastActivity.pop();
//            }
//            cout << "Least active zombies:\n";
//            for (size_t i = 0; i < statistic; i++) {
//                cout << mostActivity.top()->name << " " << mostActivity.top()->activity << "\n";
//                mostActivity.pop();
//            }
//        }
//    }
//    else {
//        cout << "Most active zombies:\n";
//        cout << "Least active zombies:\n";
//    }
//}

//V2
void zombbb::print_stats() {
    //zombies active
    cout << "Zombies still active: " << targets.size() << "\n";

    //Zombies killed

    //if stats is > size of dead_zombies, then just use dead_zombies size
    if (statistic > dead_zombies.size()) {
        size_t dz_size = dead_zombies.size();
        //making sure zombies were killed
        if (dz_size != 0) {

            //first zombies killed
            cout << "First zombies killed:\n";
            for (size_t i = 0; i < dz_size; i++) {
                cout << (dead_zombies[dz_size - i - 1])->name << " " << i + 1 << "\n";
            }

            //last zombies killed
            cout << "Last zombies killed:\n";
            for (size_t i = 0; i < dz_size; i++) {
                cout << (dead_zombies[i])->name << " " << dz_size - i << "\n";
            }
        }
        else {
            cout << "First zombies killed:\n";
            cout << "Last zombies killed:\n";
        }
    }

    //else, use the stats #
    else {
        size_t dz_size = dead_zombies.size();

        //first zombies killed
        cout << "First zombies killed:\n";
        for (uint32_t i = 0; i < statistic; i++) {
            cout << (dead_zombies[dz_size - i - 1])->name << " " << i + 1 << "\n";
        }

        //last zombies killed
        cout << "Last zombies killed:\n";
        for (size_t i = 0; i < statistic; i++) {
            cout << (dead_zombies[i])->name << " " << statistic-i << "\n";
        }
    }

    //Active zombies

    //first adding all our zombies to the activity PQs
    if (!masterlist.empty()) {
        for (deque<zombie*>::iterator it = masterlist.begin(); it != masterlist.end(); ++it) {
            leastActivity.push(*it);
            mostActivity.push(*it);
        }

        //if stats is more than the size of
        if (statistic > static_cast<uint32_t>(mostActivity.size())) {
            cout << "Most active zombies:\n";
            size_t act_size = mostActivity.size();
            for (size_t i = 0; i < act_size; i++) {
                cout << leastActivity.top()->name << " " << leastActivity.top()->activity << "\n";
                leastActivity.pop();
            }
            cout << "Least active zombies:\n";
            for (size_t i = 0; i < act_size; i++) {
                cout << mostActivity.top()->name << " " << mostActivity.top()->activity << "\n";
                mostActivity.pop();
            }

        }
        else {
            cout << "Most active zombies:\n";
            for (size_t i = 0; i < statistic; i++) {
                cout << leastActivity.top()->name << " " << leastActivity.top()->activity << "\n";
                leastActivity.pop();
            }
            cout << "Least active zombies:\n";
            for (size_t i = 0; i < statistic; i++) {
                cout << mostActivity.top()->name << " " << mostActivity.top()->activity << "\n";
                mostActivity.pop();
            }
        }
    }
    else {
        cout << "Most active zombies:\n";
        cout << "Least active zombies:\n";
    }
}

void zombbb::run_program() {
    //discarding our comment line
    
    string junk;
    getline(cin, junk);
    
    //reading in starting variables.
    P2random random = get_random();
    
    //read in round
    uint32_t read_round = 0;
    
    while (defeat != true && victory != true) {
        if (cin >> junk) {
            cin >> junk >> read_round;
            if (read_round > curr_round) {
                while (read_round > curr_round && defeat != true && victory != true) {
                    run_defround(curr_round);
                }
                if (defeat != true && victory != true) {
                    run_inround(curr_round, random);
                }
            }
            else {
                run_inround(curr_round, random);
            }
        }
        else if (!targets.empty()) {
                run_defround(curr_round);
        }
        else {
                victory = true;
                cout << "VICTORY IN ROUND " << curr_round-1 << "! " << dead_zombies.front()->name << " was the last zombie.\n";
        }
    }
    //print stats
    if (statistic > 0) {
        print_stats();
    }
} //run_program()

int main(int argc, char ** argv) {
    ios_base::sync_with_stdio(false);
    zombbb zombie;
    
    zombie.get_mode(argc, argv);
    
    zombie.run_program();
    return 0;
}
