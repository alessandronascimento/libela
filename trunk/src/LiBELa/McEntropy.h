#ifndef MCENTROPY_H
#define MCENTROPY_H

#include<iostream>
#include<zlib.h>
#include<cstdlib>
#include<cstdio>
#include<stdio.h>
#include<cmath>
#include<string>
#include <vector>
#include<sstream>
#include "PARSER.h"
#include "Mol2.h"
#include "COORD_MC.h"


class McEntropy
{
public:
    int rot_bins;
    int trans_bins;
    int translation_window;
    int n_rot;
    double translation_step;
    double rotation_step;
    vector<double> hist_x;
    vector<double> hist_y;
    vector<double> hist_z;
    vector<double> hist_alpha;
    vector<double> hist_beta;
    vector<double> hist_gamma;
    vector<double> com;
    float** hist_torsions;

    PARSER* Input;
    COORD_MC* Coord;
    double k;

    struct entropy_t{
        double Strans;
        double Srot;
        double Storsion;
        double S;
        double TS;
    };

    McEntropy(PARSER* _Input, COORD_MC* _Coord, vector<double> _com, int _n_rot);
    ~McEntropy();
    void update(double x, double y, double z, double alpha, double beta, double gamma, vector<double> torsion);
    void get_results(entropy_t* entropy, int count);
};

#endif // MCENTROPY_H