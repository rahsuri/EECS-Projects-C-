// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725
//  main.cpp
//  281-project1
//
//  Created by Rahul Suri on 9/16/23.
//

#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <deque>
#include <getopt.h>

//plan:
//1. first want to create a command line options reader to read in different options.
//  - to do this, we are going to create a struct that is created from read in from command args.
//  the struct will contain the option choice, that will be conveyed to our "puzzle" class.
//2. we want to be able to read in the map and establish the starting point of the game (@).
//3. then, we want to create our algorithm to try and beat the game, and simultaneously be able to record solves.

using namespace std;

//puzzle class. this will contain all the operations, datastrucutres, and conditional values we will use.
class puzzle {
public:
    //function to read in data from cin
    puzzle() {num_colors = 1; height = 0; width = 0;}
    
    void read_data();
    
    //function to get the user option (queue, stack, output, help)
    void get_option(int argc, char * argv[]);
    
    //solve using queue
    void queueBFS();
    
    //solve using a stack
    void stackBFS();
    
    char choice();
    
    void output(string type);
    
    void noSol();
    
private:
    vector<vector<char>> map;
    vector<vector<vector<char>>> backtrace;
    vector<vector<vector<bool>>> discovered;
    
    uint32_t num_colors;
    uint32_t height;
    uint32_t width;
    
    struct Options {
        bool allowed = false; //true if valid option chose, false if not.
        uint32_t bfsType = 0; //0 for not chosen, 1 for queue, 2 for stack.
        bool output = false; //true if output is redirected, false if it will be to cout.
        string outputType = "map"; //output type chosen, default map.
    };  // Options{}
    
    Options option;
    
    struct state {
        uint32_t color = 0;
        uint32_t x = 0;
        uint32_t y = 0;
        void north() {
            x--;
        }
        void east() {
            y++;
        }
        void south() {
            x = x + 1;
        }
        void west() {
            y--;
        }
    };
    
    state start_state;
    state end_state;
    
    uint32_t chartocolor(char c) {
        if (c == '^' || c == '.' || c == '?') {
            return 0;
        }
        else if (c == '#') {
            return 27;
        }
        else if (c == 'B') {
            return 2;
        }
        else if (static_cast<uint32_t>(c) < 91) {
            return static_cast<uint32_t>(c - 'A') + 1;
        }
        else {
            return static_cast<uint32_t>(c - 'a') + 1;
        }
    }
    
    bool isvalid(state pstate) {
        //does it exist?
        if (discovered[pstate.color][pstate.x][pstate.y] == true) {
            return false;
        }
        else if (pstate.color > 26) {
            return false;
        }
//        else if (pstate.color > 0 && map[pstate.x][pstate.y] == '^') {
//            return false;
//        }
        else if (pstate.x >= height || pstate.y >= width) {
            return false;
        }
        //is it somewhere we are allowed to go?
        // 1. is it a wall?
        else if (map[pstate.x][pstate.y] == '#') {
            return false;
        }
        // 2. if the color is not default ('^'), is it a color we are allowed to be on?
        else if (discovered[pstate.color][pstate.x][pstate.y] == true) {
            return false;
        }
        else if (pstate.color != 0) {
            if (chartocolor(map[pstate.x][pstate.y]) != 0
                && chartocolor(map[pstate.x][pstate.y]) != pstate.color
                && map[pstate.x][pstate.y] < 97) {
                return false;
            }
            else {
                return true;
            }
        }
        else if (pstate.color != chartocolor(map[pstate.x][pstate.y])
                 && (map[pstate.x][pstate.y] < 91)) {
            return false;
        }
        //has it been discovered already?
        else {
            return true;
        }
    }
};

void puzzle::output(string type) {
    if (type == "map") {
        vector<vector<vector<char>>> outmap;
        
        //creating an output map
        outmap.resize(num_colors + 1);
        for (uint32_t k=0; k < (num_colors + 1); k++) {
            outmap[k].resize(height);
            for (uint32_t i=0; i < height; i++) {
                outmap[k][i].resize(width);
            }
        }
        
        //filling the map
        for (uint32_t k = 0; k < (num_colors + 1); k++) {
            for (uint32_t i = 0; i < height; i++) {
                for (uint32_t j = 0; j < width; j++) {
                    if (chartocolor(map[i][j]) == k) {
                        outmap[k][i][j] = '.';
                    }
                    else {
                        outmap[k][i][j] = map[i][j];
                    }
                }
            }
        }
        
        stack<state> coords;
        
        state iterator;
        iterator.color = end_state.color;
        iterator.x = end_state.x;
        iterator.y = end_state.y;
        
        while (iterator.color != start_state.color
               || iterator.x != start_state.x
               || iterator.y != start_state.y) {
            coords.push(iterator);
            if (backtrace[iterator.color][iterator.x][iterator.y] == '1') {
                iterator.north();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '3') {
                iterator.east();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '2') {
                iterator.south();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '4') {
                iterator.west();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '^') {
                iterator.color = 0;
            }
            else if (chartocolor(backtrace[iterator.color][iterator.x][iterator.y]) > 0
                     && chartocolor(backtrace[iterator.color][iterator.x][iterator.y]) < 200) {
                iterator.color = static_cast<uint32_t>(backtrace[iterator.color][iterator.x][iterator.y] - 'a') + 1;
            }
        }
        
        //PRINTING THE MAP!
        //while loop going while our coords container is not empty
        //we want to print '+' for every peice except:
        // - places where a button was pressed
        //     - if a button was pressed, we want to print a '%' where the button was pressed and a '@' for where we arrived after the button was pressed.
        //     - we can detect if a button was pressed by seeing if the next object in the container has a different color.
        //
        //
        state check;
        
        for (uint32_t k = 0; k < (num_colors + 1); k ++) {
            if (k == 0) {
                outmap[k][start_state.x][start_state.y] = '@';
            }
            else {
                outmap[k][start_state.x][start_state.y] = '.';
            }
        }
        
        outmap[0][end_state.x][end_state.y] = '?';
        
        while (!coords.empty()) {
            check = coords.top();
            coords.pop();
            if (!coords.empty()) {
                if (check.color != coords.top().color) {
                    outmap[check.color][check.x][check.y] = '%';
                    outmap[coords.top().color][coords.top().x][coords.top().y] = '@';
                    coords.pop();
                }
                else {
                    outmap[check.color][check.x][check.y] = '+';
                }
            }
            else {
                outmap[check.color][check.x][check.y] = '?';
            }
        }
        
        for (uint32_t k = 0; k < (num_colors + 1); k++) {
            if (k == 0) {
                cout << "// color ^\n";
            }
            else {
                cout << "// color " << static_cast<char>('a' + k - 1) << "\n";
            }
            for (uint32_t i = 0; i < height; i++) {
                for (uint32_t j = 0; j < width; j++) {
                    cout << outmap[k][i][j];
                }
                cout << "\n";
            }
        }
    }
    else {
        stack<state> coords;
        
        state iterator;
        iterator.color = end_state.color;
        iterator.x = end_state.x;
        iterator.y = end_state.y;
        
        while (iterator.color != start_state.color
               || iterator.x != start_state.x
               || iterator.y != start_state.y) {
            coords.push(iterator);
            if (backtrace[iterator.color][iterator.x][iterator.y] == '1') {
                iterator.north();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '3') {
                iterator.east();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '2') {
                iterator.south();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '4') {
                iterator.west();
            }
            else if (backtrace[iterator.color][iterator.x][iterator.y] == '^') {
                iterator.color = 0;
            }
            else if (chartocolor(backtrace[iterator.color][iterator.x][iterator.y]) > 0
                     && chartocolor(backtrace[iterator.color][iterator.x][iterator.y]) < 200) {
                iterator.color = static_cast<uint32_t>(backtrace[iterator.color][iterator.x][iterator.y] - 'a') + 1;
            }
        }
        
        coords.push(iterator);
        
        uint32_t ocolor;
        uint32_t ox;
        uint32_t oy;
        
        while (!coords.empty()) {
            ocolor = coords.top().color;
            ox = coords.top().x;
            oy = coords.top().y;
            if (ocolor == 0) {
                cout << "(^, " <<
                "(" << ox << ", " <<
                oy << "))\n";
            }
            else {
                cout << "(" << static_cast<char>(('a' + ocolor) - 1) << ", " <<
                "(" << ox << ", " <<
                oy << "))\n";
            }
            coords.pop();
        }
    }
}

void puzzle::noSol() {
    vector<vector<char>> outmap;
    
    outmap.resize(height);
    for (uint32_t i=0; i < height; i++) {
        outmap[i].resize(width);
    }
        
    cout << "No solution.\n";
    cout << "Discovered:\n";
    for (uint32_t k = 0; k < (num_colors + 1); k++) {
        for (uint32_t i = 0; i < height; i++) {
            for (uint32_t j = 0; j < width; j++) {
                if (discovered[k][i][j] == true) {
                    outmap[i][j] = map[i][j];
                }
                else {
                    if (outmap[i][j] != map[i][j]) {
                        outmap[i][j] = '#';
                    }
                }
            }
        }
    }
    
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            cout << outmap[i][j];
        }
        cout << "\n";
    }
    
}

void printHelp() {
  cout << "Possible Usage: ./project1 [--queue/-q]\n"
       << "./project1 [--stack/-s]\n"
       << "./project1 [--output/-o]\n"
       << "-o can be used with -q and -s, however -q and -s cannot be used in the same argument.\n"
       << "output and input redirection can be specified by using:\n"
       << "./project1 < <input_file> > <output_file>";
       
}  // printHelp()

void puzzle::get_option(int argc, char * argv[]) {
    // These are used with getopt_long()
    opterr = false; //"opting" out of letting the program handle our errors for us (false)
    int choice; //option that our user chooses
    int choice_index = 0;
    struct option long_options[] = {
        {"queue", no_argument, nullptr, 'q'},
        {"stack", no_argument, nullptr, 's'},
        {"output", optional_argument, nullptr, 'o'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'}
  };  // long_options[]

  // TODO: Fill in the double quotes, to match the mode and help options.
  while ((choice = getopt_long(argc, argv, "o:qsh", long_options, &choice_index)) != -1) {
      switch (choice) {
          case 'o':
              if (option.output == true) {
                  cerr << "Error: two output options provided.";
                  exit(1);
              }
              option.output = true;
              if (optarg != nullptr) {
                  option.outputType = optarg;
                  if (option.outputType != "map" && option.outputType != "list") {
                      cerr << "Error: Invalid output mode." <<
                      "Please choose 'map' or 'list'. chosen: " << option.outputType;
                      exit(1);
                  }
              }
              break;
          case 'q':
              if (option.bfsType) {
                  cerr << "Error: Can not have both stack and queue";
                  exit(1);
              }
              option.bfsType = true;
              option.allowed = true;
              break;
          case 's':
              if (option.bfsType) {
                  cerr << "Error: Can not have both stack and queue";
                  exit(1);
              }
              option.bfsType = false;
              option.allowed = true;
              break;
          case 'h':
              printHelp();
              exit(0);
          default:
              cerr << "Error: unknown option" << endl;
              exit(1);
    }  // switch ..choice
  }  // while

    if (option.allowed == false) {
        cerr << "Error: Must have at least one of stack or queue" << endl;
        exit(1);
    }
}  // if ..mode
// getMode()

char puzzle::choice() {
    if (option.bfsType == 1) {
        return 'q';
    }
    else {
        return 's';
    }
}

void puzzle::read_data() {
    string junk;
    string line;
    
    bool haveStart = false;
    bool haveEnd = false;
    
    uint32_t col,h, w;
    
    
    //reading in the number of colors, the height, and the width
    cin >> col >> h >> w;
    
    if (col <= 26) {
        num_colors = col;
    }
    else {
        cerr << "Error: Invalid numColor";
        exit(1);
    }
    
    if (h >= 1) {
        height = h;
    }
    else {
        cerr << "Error: Invalid height";
        exit(1);
    }
    
    if (w >= 1) {
        width = w;
    }
    else {
        cerr << "Error: Invalid width";
        exit(1);
    }
    
    //resizing our map and backtrace to *height rows and *width columns
    map.resize(height);
    for (uint32_t i=0; i < height; i++) {
        map[i].resize(width);
    }
    
    
    backtrace.resize(num_colors + 1);
    for (uint32_t k=0; k < (num_colors + 1); k++) {
        backtrace[k].resize(height);
        for (uint32_t i=0; i < height; i++) {
            backtrace[k][i].reserve(width);
        }
    }
    
    
    discovered.resize(num_colors + 1);
    for (uint32_t k=0; k < (num_colors + 1); k++) {
        discovered[k].resize(height);
        for (uint32_t i=0; i < height; i++) {
            discovered[k][i].resize(width, false);
        }
    }
    
    
    //since after reading through cin we still have a \n character, we want to discard it.
    getline(cin, junk);
    
    uint32_t i = 0;
    while(getline(cin, line)) {
        if (line.substr(0,2) == "//") {
            continue;
        }
        if (line[0] == ' ') {
            return;
        }
        for (uint32_t j = 0; j < width; j++) {
            if (line[j] == '@' && haveStart == true) {
                cerr << "Error: Puzzle must have only one start.";
                exit(1);
            }
            else if (line[j] == '@' && haveStart == false) {
                start_state.x = i;
                start_state.y = j;
                haveStart = true;
            }
            if (line[j] == '?' && haveEnd == true) {
                cerr << "Error: Puzzle must only have one target.";
                exit(1);
            }
            else if (line[j] == '?' && haveEnd == false) {
                end_state.x = i;
                end_state.y = j;
                haveEnd = true;
            }
            map[i][j] = line[j];
        }
        i++;
    }
}

//steps for searches:
//1. we want to create a search containter for our 'discoveries'.
//2. start at the starting point, and thus add it to our search container since it is discovered.
//3. we want to traverse by one unit in all directions, N,E,S,W, to find other locations and possibilites.
//4.
void puzzle::queueBFS() {
    bool sol_found = false;
    queue<state> search_container;
    state currState = start_state;
    search_container.push(currState);
    backtrace[currState.color][currState.x][currState.y] = '@';
    discovered[currState.color][currState.x][currState.y] = true;
    
    while (!search_container.empty()) {
        state check = currState;
        search_container.pop();
        //if on a button, do button stuff. else, do the rest.
        if (chartocolor(map[currState.x][currState.y]) > 0
        && chartocolor(map[currState.x][currState.y]) < 27
        && currState.color != chartocolor(map[currState.x][currState.y])) {
            //currState.color = chartocolor(map[currState.x][currState.y]);
            currState.color = chartocolor(map[currState.x][currState.y]);
            if (isvalid(currState)) {
                
                search_container.push(currState);
                
                discovered[currState.color][currState.x][currState.y] = true;
                
                
                if (check.color == 0) {
                    backtrace[currState.color][currState.x][currState.y] = '^';
                }
                else {
                    backtrace[currState.color][currState.x][currState.y] =
                    static_cast<char>('a' + check.color - 1);
                }
            }
            currState.color = check.color;
        }
        else if (currState.color != chartocolor(map[currState.x][currState.y])
                 && map[currState.x][currState.y] == '^') {
            currState.color = 0;
            if (isvalid(currState)) {
                search_container.push(currState);
                
                discovered[currState.color][currState.x][currState.y] = true;
                
                backtrace[currState.color][currState.x][currState.y] =
                static_cast<char>('a' + check.color - 1);
            }
            currState.color = check.color;
        }
        //if on a trap, switch back to color 0
        else {
            //making sure we can go north
            if (currState.x > 0) {
                //if we can, go north
                currState.north();
                //if the tile we are on is valid, add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '2';
                }
                //go back to starting point and switch back color
                currState.south();
            }
            
            //making sure we can go east
            if (currState.y < (width-1)) {
                //if we can, go east
                currState.east();
                //if the tile we are on is valid,
                //add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '4';
                }
                //go back to starting point and switch back color
                currState.west();
            }
            
            //making sure we can go south
            if (currState.x < (height-1)) {
                //if we can, go south
                currState.south();
                //if the tile we are on is valid,
                //add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '1';
                }
                //go back to starting point and switch back color
                currState.north();
            }
            
            //making sure we can go west
            if (currState.y > 0) {
                //if we can, go west
                currState.west();
                //if the tile we are on is valid,
                //add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '3';
                }
                //go back to starting point and switch back color
                currState.east();
            }
            
            if (map[currState.x][currState.y] == '?') {
                //print the solution based off the current state (which is the end)
                end_state.color = currState.color;
                end_state.x = currState.x;
                end_state.y = currState.y;
                //cout << "backtrace on layer a: " << backtrace[1][1][1] << " and now layer b: " << backtrace[2][0][6];
                output(option.outputType);
                sol_found = true;
                return;
            }
        }
        currState = search_container.front();
    }
//    cout << "Final State: " << currState.color << " " << currState.x << " " << currState.y << "\n";
    if (sol_found == false) {
        noSol();
    }
}
void puzzle::stackBFS() {
    bool sol_found = false;
    stack<state> search_container;
    state currState = start_state;
    search_container.push(currState);
    backtrace[currState.color][currState.x][currState.y] = '@';
    discovered[currState.color][currState.x][currState.y] = true;
    
    while (!search_container.empty()) {
        state check = currState;
        search_container.pop();
        //if on a button, do button stuff. else, do the rest.
        if (chartocolor(map[currState.x][currState.y]) > 0
        && chartocolor(map[currState.x][currState.y]) < 27
        && currState.color != chartocolor(map[currState.x][currState.y])) {
            //currState.color = chartocolor(map[currState.x][currState.y]);
            currState.color = chartocolor(map[currState.x][currState.y]);
            if (isvalid(currState)) {
                
                search_container.push(currState);
                
                discovered[currState.color][currState.x][currState.y] = true;
                
                
                if (check.color == 0) {
                    backtrace[currState.color][currState.x][currState.y] = '^';
                }
                else {
                    backtrace[currState.color][currState.x][currState.y] =
                    static_cast<char>('a' + check.color - 1);
                }
            }
            currState.color = check.color;
        }
        else if (currState.color != chartocolor(map[currState.x][currState.y])
                 && map[currState.x][currState.y] == '^') {
            currState.color = 0;
            if (isvalid(currState)) {
                search_container.push(currState);
                
                discovered[currState.color][currState.x][currState.y] = true;
                
                backtrace[currState.color][currState.x][currState.y] =
                static_cast<char>('a' + check.color - 1);
            }
            currState.color = check.color;
        }
        //if on a trap, switch back to color 0
        else {
            //making sure we can go north
            if (currState.x > 0) {
                //if we can, go north
                currState.north();
                //if the tile we are on is valid, add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '2';
                }
                //go back to starting point and switch back color
                currState.south();
            }
            
            //making sure we can go east
            if (currState.y < (width-1)) {
                //if we can, go east
                currState.east();
                //if the tile we are on is valid,
                //add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '4';
                }
                //go back to starting point and switch back color
                currState.west();
            }
            
            //making sure we can go south
            if (currState.x < (height-1)) {
                //if we can, go south
                currState.south();
                //if the tile we are on is valid,
                //add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '1';
                }
                //go back to starting point and switch back color
                currState.north();
            }
            
            //making sure we can go west
            if (currState.y > 0) {
                //if we can, go west
                currState.west();
                //if the tile we are on is valid,
                //add it to the search container and mark it discovered.
                if (isvalid(currState)) {
                    search_container.push(currState);
                    discovered[currState.color][currState.x][currState.y] = true;
                    
                    //adding it to backtrace
                    backtrace[currState.color][currState.x][currState.y] = '3';
                }
                //go back to starting point and switch back color
                currState.east();
            }
            
            if (map[currState.x][currState.y] == '?') {
                //print the solution based off the current state (which is the end)
                end_state.color = currState.color;
                end_state.x = currState.x;
                end_state.y = currState.y;
                //cout << "backtrace on layer a: " << backtrace[1][1][1] << " and now layer b: " << backtrace[2][0][6];
                output(option.outputType);
                sol_found = true;
                return;
            }
        }
        currState = search_container.top();
    }
//    cout << "Final State: " << currState.color << " " << currState.x << " " << currState.y << "\n";
    if (sol_found == false) {
        noSol();
    }
}




int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    
    //creating an instance of our game.
    puzzle Puzzle;
    
    //getting options
    
    Puzzle.get_option(argc, argv);
    
    //reading data
    Puzzle.read_data();
    
    if (Puzzle.choice() == 'q') {
        Puzzle.queueBFS();
    }
    else if (Puzzle.choice() == 's') {
        Puzzle.stackBFS();
    }
    
    return 0;
}
