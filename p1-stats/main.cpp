//
//  main.cpp
//  p1-stats
//
//  Created by Rahul Suri on 1/15/23.
// Project UID 5366c7e2b77742d5b2142097e51561a5

#include <cstdio>
#include "stats.h"
#include "p1_library.h"
#include <iostream>

using namespace std;

int main(int argc, const char * argv[]) {
    vector<pair<double, int>> vp;
    int vcount;
    double vsum;
    double vmean;
    double vmedian;
    double vmode;
    double vmin;
    double vmax;
    double vstdev;
    double v0pct;
    double v25pct;
    double v50pct;
    double v75pct;
    
    #ifdef __APPLE__
    freopen("main_test.in", "r", stdin);
    #endif
    
    string filename;
    string column_name;
    
    cout << "enter a filename" << endl;
    cin >> filename;
    
    cout << "enter a column name" << endl;
    cin >> column_name;
    
    cout << "reading column " << column_name << " from " << filename << endl;
    
    vector<double> v = extract_column(filename, column_name);
    
    vp = summarize(v);
    cout << "Summary (value: frequency)" << endl;
    for (int i = 0; i < vp.size(); i++) {
        cout << vp[i].first << ": " << vp[i].second << endl;
    }
    cout << endl;
    
    vcount = count(v);
    cout << "count = " << vcount << endl;
    
    vsum = sum(v);
    cout << "sum = " << vsum << endl;
    
    vmean = mean(v);
    cout << "mean = " << vmean << endl;
    
    vstdev = stdev(v);
    cout << "stdev = " << vstdev << endl;
    
    vmedian = median(v);
    cout << "median = " << vmedian << endl;
    
    vmode = mode(v);
    cout << "mode = " << vmode << endl;
    
    vmin = min(v);
    cout << "min = " << vmin << endl;
    
    vmax = max(v);
    cout << "max = " << vmax << endl;
    
    v0pct = percentile(v, 0);
    cout << "  0th percentile = " << v0pct << endl;
    v25pct = percentile(v, 25);
    cout << " 25th percentile = " << v25pct << endl;
    v50pct = percentile(v, 50);
    cout << " 50th percentile = " << v50pct << endl;
    v75pct = percentile(v, 75);
    cout << " 75th percentile = " << v75pct << endl;
//    v100pct = percentile(v, 100);
//    cout << "100th percentile = " << v100pct << endl;
    cout << "100th percentile = " << max(v) << endl;
    
    return 0;
}
