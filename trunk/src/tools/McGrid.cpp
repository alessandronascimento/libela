#include <iostream>
#include <cstdlib>
#include <vector>
#include "../LiBELa/Grid.cpp"
#include "../LiBELa/PARSER.cpp"
#include "../LiBELa/Mol2.cpp"
#include "../LiBELa/COORD_MC.cpp"
#include "../LiBELa/WRITER.cpp"
#include "../LiBELa/FindHB.cpp"

using namespace std;

int main(int argc, char* argv[]){

        if (argc < 2){
                printf("Usage: %s input_file\n", argv[0]);
                exit(1);
        }

        printf("****************************************************************************************************\n");
        printf("****************************************************************************************************\n");
        printf("*                  McGrid - Energy Grid Computation for Molecular Docking with McLiBELa            *\n");
        printf("*                                                                                                  *\n");
        printf("* University of Sao Paulo                                                                          *\n");
        printf("* More Info:                                                                                       *\n");
        printf("*      http://www.biotechmol.ifsc.usp.br/                                                          *\n");
        printf("*                                                                                                  *\n");
        printf("****************************************************************************************************\n");
        printf("****************************************************************************************************\n");


        clock_t start, end;

        start = clock();

        PARSER* Input = new PARSER();
        Input->set_parameters(argv[1]);
        Input->write_grids = true;

        WRITER* Writer = new WRITER(Input);

        Mol2* Rec = new Mol2(Input, Input->rec_mol2);
        Mol2* Lig = new Mol2(Input, Input->reflig_mol2);

        FindHB* HB = new FindHB;

        HB->find_ligandHB(Input->reflig_mol2, Lig);
        for (int i=0; i< Rec->residue_pointer.size()-1; i++){
            HB->parse_residue(Rec->residue_pointer[i]-1, Rec->residue_pointer[i+1]-2, Rec->resnames[i], Rec, Lig, 9.0);
        }

        vector<double> com;
        COORD_MC* Coord = new COORD_MC();
        com = Coord->compute_com(Lig);
        printf("Ligand and Receptor files read!\n");
        printf("Found %5lu HB donors and %5lu HB acceptor around the active site in a 9.0 Ang search sphere radius.\n", Rec->HBdonors.size(), Rec->HBacceptors.size());
        printf("Computing grids centered at %10.5f %10.5f %10.5f.\n", com[0], com[1], com[2]);
        printf("This may take a while...Coffee time, maybe?\n");
        delete Lig;
        delete Coord;
        delete HB;
        Grid* Grids = new Grid(Input, Writer, Rec, com);
        printf("Computed energy grids with %d points spaced by %5.3f Angstroms in each directon.\n", Grids->npointsx*Grids->npointsy*Grids->npointsz, Grids->grid_spacing);
        printf("Grid X limits: %10.4f %10.4f.\n", Grids->xbegin, Grids->xend);
        printf("Grid Y limits: %10.4f %10.4f.\n", Grids->ybegin, Grids->yend);
        printf("Grid Z limits: %10.4f %10.4f.\n", Grids->zbegin, Grids->zend);
        end = clock();
        printf("Grid computation took %d seconds.\n", int((end-start)/CLOCKS_PER_SEC));

        Writer->write_box(com, Grids->xbegin, Grids->ybegin, Grids->zbegin, Grids->xend, Grids->yend, Grids->zend);

        delete Rec;
        delete Writer;
        delete Input;

        printf("****************************************************************************************************\n");
        printf("****************************************************************************************************\n");


        return 0;
}
