#include "obj_load.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

void PrintUsage()
{
    printf("Obj to PPM converter\n\n");
    printf("Converts position data from an obj to an PPM image.\n\n");
    printf("Usage: ObjToPPM <position range> <source file> <dest file>\n");
}

int ftoiRange(float f, float range, int iMax)
{
    double d = (f / (range * 2.0)) + 0.5;
    d = std::max(std::min(d, 1.0), 0.0);
    return (int)(d * iMax);
}

int main(int argc, const char** argv)
{
    if(argc != 4)
    {
        PrintUsage();
        return -1;
    }

    float range = atof(argv[1]);

    int colorMax = 65535;

    if(range == 0.0f)
    {
        PrintUsage();
        return -1;
    }

    obj_positions* mesh = obj_load_mesh(argv[2]);

    if(!mesh)
    {
        PrintUsage();
        return -1;
    }

    FILE* outFile = fopen(argv[3], "w");

    if(!outFile)
    {
        delete mesh;
        PrintUsage();
        return -1;
    }

    int imgSide = sqrt(mesh->vertex_count);
    int remainder = mesh->vertex_count - (imgSide * imgSide);

    printf("Converting mesh to image of size %dx%d, dropping a remainder of %d positions.\n", imgSide, imgSide, remainder);

    fprintf(outFile, "P3\n%d %d %d\n", imgSide, imgSide, colorMax);

    for(int y = 0; y < imgSide; y++)
    {
        for(int x = 0; x < imgSide; x++)
        {
            int i = y * imgSide + x;
            int r = ftoiRange(mesh->locations[i].x, range, colorMax);
            int g = ftoiRange(mesh->locations[i].y, range, colorMax);
            int b = ftoiRange(mesh->locations[i].z, range, colorMax);
            fprintf(outFile, "%d %d %d  ", r, g, b);
        }
        fprintf(outFile, "\n");
    }

    delete mesh;
    fclose(outFile);

    return 0;
}
