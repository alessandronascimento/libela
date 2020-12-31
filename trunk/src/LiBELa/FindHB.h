#ifndef FINDHB_H
#define FINDHB_H

#include <iostream>
#include <cstdio>
#include <map>
#include <openbabel/atom.h>
#include <openbabel/mol.h>
#include <openbabel/obiter.h>
#include <openbabel/obconversion.h>
#include <openbabel/forcefield.h>
#include <openbabel/math/align.h>
#include "Mol2.h"
#include "PARSER.h"
#include "COORD_MC.h"


class FindHB
{
public:
    FindHB();
    int find_atom(string atomname, Mol2* Rec, int astart, int aend);
    double distance(vector<double> xyz1, vector<double> xyz2);
    double distance_squared(vector<double> xyz1, vector<double> xyz2);
    double angle(vector<double> xyz1, vector<double> xyz2, vector<double> xyz3);
    void find_ligandHB(string molfile, Mol2* Lig);
    void parse_residue(int atom_start, int atom_end, string resname, Mol2* Rec, Mol2* Lig, double dist_cutoff=10.);
};

#endif // FINDHB_H