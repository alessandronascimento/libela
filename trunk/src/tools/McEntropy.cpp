#include<iostream>
#include<zlib.h>
#include<cstdlib>
#include<cstdio>
#include<stdio.h>
#include<cmath>
#include<string>
#include <vector>
#include "../LiBELa/PARSER.cpp"
#include "../LiBELa/Mol2.cpp"
#include "../LiBELa/COORD_MC.cpp"

using namespace std;

int main(int argc, char* argv[]){

    int MAX_STEPS = 10000000;
    string infile, ligfile;
    int c;
    int rot_bins=360, trans_bins=60, translation_window=30;
    double Temp = 100.0;
    double k=0.001987;
    PARSER* Input = new PARSER;

    if (argc < 2){
        printf("Usage %s -i <inputfile> -t <number of bins for translation> -r <number of bins for rotation> -l <ligand mol2 file> [-h]\n", argv[0]);
        exit(1);
    }

    while ((c = getopt(argc, argv, "i:t:r:l:h")) != -1)
        switch (c){
        case 'i':
            infile = string(optarg);
            break;
        case 'h':
            printf("Usage %s -i <inputfile> -t <number of bins for translation> -r <number of bins for rotation> [-h]\n", argv[0]);
            break;
            exit(1);
        case '?':
            printf("Usage %s -i <inputfile> -t <number of bins for translation> -r <number of bins for rotation> [-h]\n", argv[0]);
            break;
            exit(1);
        case 't':
            trans_bins = atoi(optarg);
            break;
        case 'r':
            rot_bins = atoi(optarg);
            break;
        case 'l':
            ligfile = string(optarg);
            break;
        }

    Mol2* Lig = new Mol2(Input, ligfile);
    COORD_MC* Coord = new COORD_MC;
    vector<double> com = Coord->compute_com(Lig);
    printf("Ligand COM: %7.3f %7.3f %7.3f\n", com[0], com[1], com[2]);

    delete Coord;
    delete Lig;
    delete Input;

    float translation_step = translation_window*1.0/trans_bins;       //typically 0.5 Ang
    float rotation_step = 360.0/rot_bins;                             //typically 1.0 degree

    float hist_x[trans_bins];
    float hist_y[trans_bins];
    float hist_z[trans_bins];

    for (unsigned i=0; i< trans_bins; i++){
        hist_x[i]= 0.0;
        hist_y[i]= 0.0;
        hist_z[i]= 0.0;
    }

    float hist_alpha[rot_bins];
    float hist_beta[rot_bins];
    float hist_gamma[rot_bins];

    for (unsigned i=0; i< rot_bins; i++){
        hist_alpha[i] = 0.0;
        hist_beta[i] = 0.0;
        hist_gamma[i] = 0.0;
    }

    gzFile inpfile = gzopen(infile.c_str(), "r");
    char str[250];
    char tstr[20];
    float tfloat;
    int tint, n_rot;
    float x, y, z, alpha, beta, gamma;

    gzgets(inpfile, str, 250);
    gzgets(inpfile, str, 250);
    sscanf(str, "%s %f %s %f %s %f %s %f", tstr, &tfloat, tstr, &tfloat, tstr, &tfloat, tstr, &tfloat);
    Temp = double(tfloat);
    gzgets(inpfile, str, 250);
    sscanf(str, "%s %s %d", tstr, tstr, &n_rot);
    gzgets(inpfile, str, 250);
    gzgets(inpfile, str, 250);

    printf("Parsing file %s. Temp = %7.3f K. N_rot = %3d\n", infile.c_str(), Temp, n_rot);

//    float** torsion = new float*[MAX_STEPS];

    int count=0;

    while ((! gzeof(inpfile)) and (count < MAX_STEPS)){
        gzgets(inpfile, str, 250);
        count++;

        float* torsion = new float[n_rot];

//        sscanf(str, "%d %f %f %f %f %f %f %f %f %d %f %{%f%}", &tint, &tfloat, &tfloat, &x, &y, &z, &alpha,
//               &beta, &gamma, &tint, &tfloat, &torsion);
        sscanf(str, "%d %f %f %f %f %f %f %f %f %d %f %f %f}", &tint, &tfloat, &tfloat, &x, &y, &z, &alpha,
               &beta, &gamma, &tint, &tfloat, &torsion[0], &torsion[1]);

//        printf("%7.3f %7.3f\n", torsion[0], torsion[1]);

        hist_x[int((x-com[0]+(translation_window*1.0/2.))/(translation_step))] += 1.0;
        hist_y[int((y-com[1]+(translation_window*1.0/2.))/(translation_step))] += 1.0;
        hist_z[int((z-com[2]+(translation_window*1.0/2.))/(translation_step))] += 1.0;

        hist_alpha[int(alpha/rotation_step)] += 1.0;
        hist_gamma[int(alpha/rotation_step)] += 1.0;
        hist_beta[int(alpha/rotation_step)] += 1.0;

        delete [] torsion;

        /*
         * process torsions here
         */
    }

    double Strans=0.0;
    double Srot=0.0;

    for (unsigned i=0; i< trans_bins; i++){
        hist_x[i] = hist_x[i]/count;
        if (hist_x[i] > 0.0){
            Strans += hist_x[i] * log(hist_x[i]);
        }
         printf("%d %7.3f %7.3f", i, (i*1.0*translation_step)+com[0]-(translation_window*1.0/2.), hist_x[i]);


        hist_y[i] = hist_y[i]/count;
        if (hist_y[i] > 0.0){
            Strans += hist_y[i] * log(hist_y[i]);
        }
        printf("\t %7.3f %7.3f", (i*1.0*translation_step)+com[1]-(translation_window*1.0/2.), hist_y[i]);


        hist_z[i] = hist_z[i]/count;
        if (hist_z[i] > 0.0){
            Strans += hist_z[i] * log(hist_z[i]);
        }
        printf("\t %7.3f %7.3f \t\t %7.3f\n", (i*1.0*translation_step)+com[2]-(translation_window*1.0/2.), hist_z[i], Strans);

    }

    for (unsigned i=0; i< rot_bins; i++){
        hist_alpha[i] = hist_alpha[i]/count;
        if (hist_alpha[i]> 0){
            Srot += hist_alpha[i] * log(hist_alpha[i]);
        }

        hist_beta[i] = hist_beta[i]/count;
        if (hist_beta[i]> 0){
            Srot += hist_beta[i] * log(hist_beta[i]);
        }

        hist_gamma[i] = hist_gamma[i]/count;
        if (hist_gamma[i]> 0){
            Srot += hist_gamma[i] * log(hist_gamma[i]);
        }
    }


    Strans = -k*Strans;
    Srot = -k*Srot;
    printf("Strans: %10.3f\n", Strans);
    printf("Srot: %10.3f\n", Srot);
    double S = Strans + Srot;
    printf("-TS = %10.3f\n", -Temp*S);

    return 0;
}
