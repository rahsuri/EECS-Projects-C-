//  p1-stats
//
//  Created by Rahul Suri on 1/15/23.
//
// stats.cpp
// Project UID 5366c7e2b77742d5b2142097e51561a5
#include "stats.h"
#include <cassert>
#include <vector>
#include <cmath>
#include "p1_library.h"

using namespace std;

vector<pair<double, int> > summarize(vector<double> v) {
    sort(v);
    vector<pair<double, int>> vp;
    double value = v[0];
    int freq = 0;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == value) {
            freq++;
        }
        else {
            vp.push_back({value, freq});
            value = v[i];
            freq = 1;
        }
    }
    vp.push_back({value, freq});
    return vp;
}

int count(vector<double> v) {
    int count = 0;
    for (int i = 0; i < v.size(); i++) {
        count++;
    }
    return count;
}

double sum(vector<double> v) {
    int sum = 0;
    for (int i = 0; i < v.size(); i++) {
        sum += v[i];
    }
    return sum;
}

double mean(vector<double> v) {
    int count = 0;
    double sum = 0.0;
    double mean;
    for (int i = 0; i < v.size(); i++) {
        sum += v[i];
        count++;
    }
    
    mean = sum / count;
    
    return mean;
}

double median(vector<double> v) {
    double median;
    if (v.size() % 2 == 0) {
        median = (v[(v.size() / 2)] + v[(v.size() / 2) - 1]) / 2;
        return median;
    }
    else {
        median = v[v.size() / 2];
        return median;
    }
}

double mode(vector<double> v) {
    sort(v);
    double mode = 0.0;
    double counting = 0.0;
    vector<pair<double, int>> vp;
    double value = v[0];
    int freq = 0;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == value) {
            freq++;
        }
        else {
            vp.push_back({value, freq});
            value = v[i];
            freq = 1;
        }
    }
    vp.push_back({value, freq});
    for (int j = 0; j < vp.size(); j++) {
        if (vp[j].second > counting) {
            mode = vp[j].first;
            counting = vp[j].second;
        }
    }
    return mode;
}

double min(vector<double> v) {
    double min;
    min = v[0];
    for (int i = 0; i < v.size(); i++) {
        if (v[i] < min) {
            min = v[i];
        }
    }
    return min;
}

double max(vector<double> v) {
    double max;
    max = v[0];
    for (int i = 0; i < v.size(); i++) {
        if (v[i] > max) {
            max = v[i];
        }
    }
    return max;
}

double stdev(vector<double> v) {
    int amt = count(v) - 1;
    double average = mean(v);
    double sum2 = 0;
    double stdev;
    
    for (int j = 0; j < v.size(); j++) {
        sum2 += pow((v[j] - average), 2);
    }
    stdev = sqrt(sum2 / amt);
    
    return stdev;
}

double percentile(vector<double> v, double p) {
    double n;
    double d = 0;
    double k = 0;
    double percentile;
    
    if (p == 100) {
        return max(v);
    }
    sort(v);
    
    n = (p / 100) * (v.size() - 1) + 1;
    d = modf(n , &k);
    percentile = v[k - 1] + (d * (v[k] - v[k - 1]));
    
    return percentile;
}
