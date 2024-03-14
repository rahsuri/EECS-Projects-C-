//Project IDENTIFIER  = 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
//
//  main.cpp
//  281-p4
//
//  Created by Rahul Suri on 11/24/23.
//
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include "getopt.h"
#include <climits>
#include <limits>
#include <cmath>
#include "xcode_redirect.hpp"

using namespace std;

class zoo {
    uint32_t mode = 0;
    //mode 1 is MST
    //mode 2 is FASTTSP
    //mode 3 is OPTTSP
    uint32_t vertex_count = 0;
    
    struct MSTcage {
        uint32_t number;
        double x;
        double y;
        bool k = false;
        double d = INT_MAX;
        uint32_t p = 0;
        uint32_t zone = 0;
        // zone 0 is origin (0,0) meaning its in the wall
        // zone 1 is the non-wild animal cages
        // zone 2 is the left wall
        // zone 3 is the wild animal area
        // zone 4 is the bottom wall
    };
    
    class mstComp {
    public:
        bool operator()(const MSTcage* cage1, const MSTcage* cage2) {
            if (cage1->d > cage2->d) {
                return true;
            }
            else {
                return false;
            }
        }
    };
    
    bool can_connect(MSTcage* vertex1, MSTcage* vertex2) {
        if (vertex1->zone == 3 && vertex2->zone == 1) {
            return false;
        }
        else if (vertex1->zone == 1 && vertex2->zone == 3) {
            return false;
        }
        return true;
    }
    
    struct cage {
        uint32_t number = 0;
        double x = 0;
        double y = 0;
        bool k = false;
        double d = INT_MAX;
        uint32_t p = 0;
        uint32_t degree = 0;
    };
    
    //calculating distance for MST part
    double distance_mst(const MSTcage* cage1, const MSTcage* cage2) {
        double x_diff = cage1->x - cage2->x;
        double y_diff = cage1->y - cage2->y;
        
        //note that this is not the actual distance, but rather the square of the distance.
        //since if x < y then sqrt(x) < sqrt(y), we can save time by not square rooting.
        return static_cast<double>((x_diff * x_diff) + (y_diff * y_diff));
    }
    
    double distance(const cage* cage1, const cage* cage2) {
        double x_diff = cage1->x - cage2->x;
        double y_diff = cage1->y - cage2->y;
        
        //note that this is not the actual distance, but rather the square of the distance.
        //since if x < y then sqrt(x) < sqrt(y), we can save time by not square rooting.
        return static_cast<double>((x_diff * x_diff) + (y_diff * y_diff));
    }
    
public:
    void printHelp();
    void get_opts(int c, char ** v);
    
    //MST
    void read_mst(vector<MSTcage> &vertices);
    void MST();
    
    //FASTTSP
    void read_tsp(vector<cage> &vertices);
    void FASTTSP();
    
    //OPTTSP

    void run_zoo();
};

void zoo::printHelp() {
    cout << "To calculate a valid spanning tree, please do the following: " << "\n";
    cout << "-m / --mode [mode] " << "\n\n";
    cout << "Valid modes are: " << "\n";
    cout << "- MST\n";
    cout << "- FASTTSP\n";
    cout << "- OPTTSP\n";
}

void zoo::get_opts(int c, char ** v) {
    // These are used with getopt_long()
    opterr = false; //"opting" out of letting the program handle our errors for us (false)
    int choice; //option that our user chooses
    int choice_index = 0;
    struct option long_options[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'}
  };  // long_options[]
    
  while ((choice = getopt_long(c, v, "m:h", long_options, &choice_index)) != -1) {
      switch (choice) {
          case 'm':
              if (optarg == NULL) {
                  cerr << "Error: No mode specified";
                  exit(1);
              }
              else {
                  string mode_arg = optarg;
                  if (mode_arg == "MST") {
                      mode = 1;
                  }
                  else if (mode_arg == "FASTTSP") {
                      mode = 2;
                  }
                  else if (mode_arg == "OPTTSP") {
                      mode = 3;
                  }
                  else {
                      cerr << "Error: Invalid mode";
                      exit(1);
                  }
              }

              break;
          case 'h':
              printHelp();
              exit(0);
          default:
              cerr << "Error: Invalid command line option" << endl;
              exit(1);
    }  // switch ..choice
  }  // while
    
    if (mode == 0) {
        cerr << "Error: No mode specified";
        exit(1);
    }
}  // if ..mode
// getMode()

void zoo::read_mst (vector<MSTcage> &vertices) {
    
    bool are_wild = false;
    bool are_walls = false;
    bool are_non_wild = false;
    
    cin >> vertex_count;
    
    vertices.reserve(vertex_count);
    
    double rx;
    double ry;
    uint32_t num = 0;
    
    while (cin >> rx) {
        cin >> ry;
        
        MSTcage new_cage = {num, rx, ry};
        num++;
        
        //finding the zone.
        if (rx > 0 || ry > 0) {
            new_cage.zone = 1;
            if (are_non_wild == false) {
                are_non_wild = true;
            }
        }
        else if (rx < 0 && ry == 0) {
            new_cage.zone = 2;
            if (are_walls == false) {
                are_walls = true;
            }
        }
        else if (ry < 0 && rx == 0) {
            new_cage.zone = 4;
            if (are_walls == false) {
                are_walls = true;
            }
        }
        else if (rx == 0 && ry == 0) {
            new_cage.zone = 0;
            if (are_walls == false) {
                are_walls = true;
            }
        }
        else {
            new_cage.zone = 3;
            if (are_wild == false) {
                are_wild = true;
            }
        }
    
        vertices.push_back(new_cage);
    }
    
    if (are_wild == true && are_non_wild == true && are_walls == false) {
        cerr << "Cannot construct MST";
        exit(1);
    }
}

void zoo::MST() {

    vector<MSTcage> vertices;
    read_mst(vertices);
    
    vector<MSTcage*> outies;
    
    for (auto& vertex : vertices) {
        outies.push_back(&vertex);
    }
    
    vector<uint64_t> path;
    double total = 0;

    vertices[0].p = 0;
    vertices[0].d = 0;
    
    while (true)  {
        
        double min_d = INT_MAX;
        MSTcage* curr_v = nullptr;
        uint32_t i = 0;
        uint32_t idx = 0;
        for (auto& vertex : outies) {
            if (vertex->d < min_d) {
                curr_v = vertex;
                min_d = vertex->d;
                idx = i;
            }
            i++;
        }
        
        if (curr_v == nullptr) {
            break;
        }
        
        curr_v->k = true;
        swap(outies[idx], outies.back());
        outies.pop_back();
        
        for (auto& vertex : outies) {
            if (can_connect(curr_v, vertex)) {
                double curr_dist = distance_mst(curr_v, vertex);
                if (curr_dist < vertex->d) {
                    vertex->p = curr_v->number;
                    vertex->d = curr_dist;
                    
                }
            }
        }
        
        if (curr_v->number != curr_v->p) {
            path.push_back(curr_v->number);
        }
        total += sqrt(curr_v->d);
    }
    
    
    cout << total << "\n";
    for (auto& idx : path) {
        if (vertices[idx].number < vertices[idx].p) {
            cout << vertices[idx].number << " " << vertices[idx].p << "\n";
        }
        else {
            cout << vertices[idx].number << " " << vertices[idx].p << "\n";
        }
    }
}

void zoo::read_tsp(vector<cage> &vertices) {
    
    cin >> vertex_count;
    
    vertices.reserve(vertex_count);
    
    double rx;
    double ry;
    uint32_t num = 0;
    
    while (cin >> rx) {
        cin >> ry;
        
        cage new_cage = {num, rx, ry};
        num++;
    
        vertices.push_back(new_cage);
    }
    
}

void zoo::FASTTSP() {
    //read in vertices in proper format
    vector<cage> vertices;
    read_tsp(vertices);
    
    //initialize innies - starts with just our "start" vertex
    // this should just contain vertices that have k marked as "true"
    vector<cage*> innies;
    innies.reserve(vertices.size());
    
    //intialize outies - starts with all verticies except for our "start" vertex
    // this should just contain vertices that have k marked as "false"
    vector<cage*> outies;
    outies.reserve(vertices.size());
    
    //our path, recorded as indices to refer to verticies in "vertices"
    double total = 0;
    
    //pushing all vertices to outies.
    for (auto& vertex : vertices) {
        outies.push_back(&vertex);
    }
    
    //establishing our start
    cage* start = &(vertices[0]);
    start->p = 0;
    start->k = true;
    start->d = 0;
    
    swap(outies[0], outies.back());
    outies.pop_back();
    
    innies.push_back(start);

    
    while (innies.size() < vertices.size())  {
        cage* curr_v = innies.back();
        
        double max_d = 0;
        size_t max_idx = 0;
        size_t i = 0;
        
        for (auto& vertex : outies) {
            double curr_dist = distance(curr_v, vertex);
            if (curr_dist < vertex->d) {
                vertex->p = curr_v->number;
                vertex->d = curr_dist;
            }
            if (vertex->d > max_d) {
                max_d = vertex->d;
                curr_v = vertex;
                max_idx = i;
            }
            i++;
        }
    
        double min_cost = INT_MAX;
        cage* split_v = nullptr;
        double net_cost = 0;
        
        for (auto& vertex : innies) {
            double ik = sqrt(distance(&(vertices[vertex->p]), curr_v));
            double kj = sqrt(distance(curr_v, vertex));

            double total_cost = (ik + kj) - vertex->d;
            
            if (total_cost < min_cost) {
                min_cost = total_cost;
                split_v = vertex;
                net_cost = total_cost;
            }
        }
    
        total += net_cost;
    
        curr_v->p = split_v->p;
        curr_v->d = sqrt(distance(curr_v, &(vertices[curr_v->p])));
    
        split_v->p = curr_v->number;
        split_v->d = sqrt(distance(split_v, &(vertices[split_v->p])));
        
        
        
        curr_v->k = true;
        innies.push_back(curr_v);
        swap(outies[max_idx], outies.back());
        outies.pop_back();
    }
    
    cout << total << "\n";
    cage* print = start;
    cout << print->number << " ";
    print = &(vertices[print->p]);
    while (print != start) {
        cout << print->number << " ";
        print = &(vertices[print->p]);
    }
}

class OPTTSP {
    
    struct vertex {
        uint32_t number = 0;
        int x = 0;
        int y = 0;
        bool k = false;
        double d = INT_MAX;
        uint32_t p = 0;
        uint32_t degree = 0;
    };
    
    struct vert {
        int x;
        int y;
    };
    
    struct mst_v {
        bool k = false;
        double d = numeric_limits<double>::infinity();
        int p = -1;
    };
    
    size_t vertex_count = 0;
    
    vector<vertex> vertices;
    vector<vert> opt_vertices;
    
    vector<uint32_t> curr_path;
    double curr_cost = 0;
    double curr_mst_cost = 0;
    
    vector<uint32_t> opt_path;
    double opt_cost = 0;
    
    
    //NEWNEWNEWNEW
    vector<vector<double>> memo;
    
public:
    
    //reading
    void read_tsp(vector<vertex> &vertices) {
        
        cin >> vertex_count;
        
        vector<double> memo_vec(vertex_count, 0);
        for (size_t i = 0; i < vertex_count; i++) {
            memo.push_back(memo_vec);
        }
        
        vertices.reserve(vertex_count);
        
        int rx;
        int ry;
        uint32_t num = 0;
        
        while (cin >> rx) {
            cin >> ry;
            
            vertex new_vertex = {num, rx, ry};
            num++;
            
            vert new_vert = {rx, ry};
        
            vertices.push_back(new_vertex);
            opt_vertices.push_back(new_vert);
        }
        
    }
    
    //returns the distance SQUARED
    double distance(size_t first, size_t second) {
        if (memo[first][second] != 0) {
            return memo[first][second];
        }
        else {
            double x_diff = opt_vertices[first].x - opt_vertices[second].x;
            double y_diff = opt_vertices[first].y - opt_vertices[second].y;
            
            double val = static_cast<double>((x_diff * x_diff) + (y_diff * y_diff));
            memo[first][second] = val;
            //note that this is not the actual distance, but rather the square of the distance.
            //since if x < y then sqrt(x) < sqrt(y), we can save time by not square rooting.
            return val;
        }
    }
    
    //returns real distance
    double distance_real(size_t first, size_t second) {
        
        if (memo[first][second] != 0) {
            return sqrt(memo[first][second]);
        }
        else {
            
            double x_diff = opt_vertices[first].x - opt_vertices[second].x;
            double y_diff = opt_vertices[first].y - opt_vertices[second].y;
            
            double val = static_cast<double>((x_diff * x_diff) + (y_diff * y_diff));
            memo[first][second] = val;
            //note that this is not the actual distance, but rather the square of the distance.
            //since if x < y then sqrt(x) < sqrt(y), we can save time by not square rooting.
            return sqrt(val);
        }
    }
    
    //calculating the fasttsp
    void FAST() {
        //initialize innies - starts with just our "start" vertex
        // this should just contain vertices that have k marked as "true"
        
        vector<vertex*> innies;
        innies.reserve(vertices.size());
        
        //intialize outies - starts with all verticies except for our "start" vertex
        // this should just contain vertices that have k marked as "false"
        vector<vertex*> outies;
        outies.reserve(vertices.size());
        
        //our path, recorded as indices to refer to verticies in "vertices"
        
        //pushing all vertices to outies.
        for (auto& vertex : vertices) {
            outies.push_back(&vertex);
        }
        
        //establishing our start
        vertex* start = &(vertices[0]);
        start->p = 0;
        start->k = true;
        start->d = 0;
        
        swap(outies[0], outies.back());
        outies.pop_back();
        
        innies.push_back(start);
        
        
        while (innies.size() < vertices.size())  {
            vertex* curr_v = innies.back();
            
            double max_d = 0;
            size_t max_idx = 0;
            size_t i = 0;
            
            for (auto& vertex : outies) {
                double curr_dist = distance(curr_v->number, vertex->number);
                if (curr_dist < vertex->d) {
                    vertex->p = curr_v->number;
                    vertex->d = curr_dist;
                }
                if (vertex->d > max_d) {
                    max_d = vertex->d;
                    curr_v = vertex;
                    max_idx = i;
                }
                i++;
            }
            
            double min_cost = numeric_limits<double>::infinity();
            vertex* split_v = nullptr;
            double net_cost = 0;
            
            for (auto& vertex : innies) {
                double ik = sqrt(distance(vertex->p, curr_v->number));
                double kj = sqrt(distance(curr_v->number, vertex->number));
                
                double total_cost = (ik + kj) - vertex->d;
                
                if (total_cost < min_cost) {
                    min_cost = total_cost;
                    split_v = vertex;
                    net_cost = total_cost;
                }
            }
            
            curr_cost += net_cost;
            
            curr_v->p = split_v->p;
            curr_v->d = sqrt(distance(curr_v->number, curr_v->p));
            
            split_v->p = curr_v->number;
            split_v->d = sqrt(distance(split_v->number, split_v->p));
            
            
            
            curr_v->k = true;
            innies.push_back(curr_v);
            swap(outies[max_idx], outies.back());
            outies.pop_back();
        }
        
        vertex* print = start;
        curr_path.push_back(print->number);
        
        print = &(vertices[print->p]);
        
        while (print != start) {
            curr_path.push_back(print->number);
            print = &(vertices[print->p]);
        }
        
        opt_path = curr_path; //optimal path is the fast tsp at the moment
        opt_cost = curr_cost;
        
        vertices.clear();
        curr_cost = 0;
    }
    
    //MST for promising
    
    double calc_arms(size_t permLength) {
        double arm1 = numeric_limits<double>::infinity();
        double arm2 = numeric_limits<double>::infinity();
        
        for(size_t i = permLength; i < curr_path.size(); i++){
            if(distance_real(curr_path[0], curr_path[i]) < arm1){
                arm1 = distance_real(curr_path[0], curr_path[i]);
            }
        }

        for(size_t i = permLength; i < curr_path.size(); i++){
            if(distance_real(curr_path[permLength - 1], curr_path[i]) < arm2){
                arm2 = distance_real(curr_path[permLength - 1], curr_path[i]);
            }
        }
        
        double out = arm1 + arm2;
        
        return out;
    }
    
    //MST for promising
    double MST(size_t permLength) {
        vector<mst_v> mst_data;
        mst_data.reserve(opt_vertices.size());
        
        for (size_t i = 0; i < opt_vertices.size(); i++) {
            mst_v v;
            mst_data.push_back(v);
        }

        //different than MST since we are starting at the permLength vertex
        mst_data[permLength].d = 0;
        
        for (size_t p = 0; p < opt_vertices.size() - permLength; p++)  {
            
            double min_d = numeric_limits<double>::infinity();
            size_t idx = 0;
            
            //find our next innie
            for (size_t i = permLength; i < mst_data.size(); i++) {
                if (mst_data[i].d < min_d && mst_data[i].k == false) {
                    idx = i;
                    min_d = mst_data[i].d;
                }
            }
            
            curr_mst_cost += static_cast<double>(sqrt(mst_data[idx].d));
            
            mst_data[idx].k = true;
            
            //update paths DIFFERENTLY!!!!
            for (size_t i = 0; i < mst_data.size(); i++) {
                if (!mst_data[i].k) {
                    double curr_dist = distance(curr_path[i], curr_path[idx]);
                    if (curr_dist < mst_data[i].d) {
                        mst_data[i].d = curr_dist;
                        mst_data[i].p = static_cast<int>(i);
                    }
                }
            }
        }
        //clearing for next run.
        mst_data.clear();
        
        double mst_cost = curr_mst_cost;
        curr_mst_cost = 0;
        return mst_cost;
    }
    
    bool promising(size_t permLength) {
        if (curr_path.size() - permLength < 4) {
            return true;
        }
        else if (curr_cost > opt_cost) {
            return false;
        }
        
        if (curr_cost + MST(permLength) + calc_arms(permLength) < opt_cost) {
            return true;
        }
        else {
            return false;
        }

    }
    
//    double calcDist(const vector<cage *> &path)  {
//        double total = 0;
//        
//        for (uint32_t i = 0; i < path.size() - 1; i++) {
//            total += sqrt(distance(path[i], path[i + 1]));
//        }
//        
//        total += sqrt(distance(path[0], path.back()));
//        
//        return total;
//    }
    
    void genPerms(size_t permLength) {
        if (permLength == curr_path.size()) {
            // Do something with the path
            
            curr_cost += distance_real(curr_path[0], curr_path[permLength-1]);

            if(curr_cost < opt_cost){
                opt_cost = curr_cost;
                opt_path = curr_path;
            }
            
            curr_cost -= distance_real(curr_path[0], curr_path[permLength - 1]);
            
            return;
        }  // if ..complete path
        
        if (!promising(permLength)) {
            return;
        }  // if ..not promising
        
        for (size_t i = permLength; i < curr_path.size(); ++i) {
            swap(curr_path[permLength], curr_path[i]);
            
            curr_cost += distance_real(curr_path[permLength-1], curr_path[permLength]);
            
            genPerms(permLength + 1);
            
            curr_cost -= distance_real(curr_path[permLength-1], curr_path[permLength]);
            
            swap(curr_path[permLength], curr_path[i]);
        }  // for ..unpermuted elements
    }  // genPerms()
    
    
    void run_OPTTSP() {
        //1. Read in Vertices / Initialize Path
        read_tsp(vertices);
        
        //2. Generate FASTTSP
        FAST();
        
        //3. Generate Permutations
        genPerms(1);
        
        //4. Printing
        cout << opt_cost << "\n";
        for (auto vertex : opt_path) {
            cout << vertex<< " ";
        }
        
    }
};

void zoo::run_zoo() {

    if (mode == 1) {
        MST();
    }
    else if (mode == 2) {
        FASTTSP();
    }
    else if (mode == 3) {
        class OPTTSP opt;
        opt.run_OPTTSP();
        
//        OPTTSP2 opttsp;
//        opttsp.solve();
    }
}



int main(int argc, char ** argv) {
    // insert code here...
    ios_base::sync_with_stdio(false); // unsync c/c++
    xcode_redirect(argc, argv);
    cout << std::setprecision(2); //Always show 2 decimal places
    cout << std::fixed; //Disable scientific notation for large numbers
    
    zoo thezoo;
    
    thezoo.get_opts(argc, argv);
    
    thezoo.run_zoo();
    
    return 0;
}
