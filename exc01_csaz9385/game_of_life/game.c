#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERR -1
#define FDIR "out"
#define FPREFIX "gol_"

#define FPOS(f, x, y) *(f + width*((y+height)%height) + ((x+width)%width))

typedef enum Cell { dead, live } cell;

int width, height;

void printUsage(const char* programName) {
    printf("usage: %s <width> <height> <density> <steps>\n", programName);
}

int numNeighbours(const cell *field, const int x, const int y) {
    int num = 0;

    if (FPOS(field, x-1, y-1)) num++;
    if (FPOS(field, x  , y-1)) num++;
    if (FPOS(field, x+1, y-1)) num++;

    if (FPOS(field, x-1, y  )) num++;
    if (FPOS(field, x+1, y  )) num++;

    if (FPOS(field, x-1, y+1)) num++;
    if (FPOS(field, x  , y+1)) num++;
    if (FPOS(field, x+1, y+1)) num++;

    return num;
}

int makePBM(const char *fname, const cell *field) {
    FILE *outfile = fopen(fname, "w");

    if (!outfile) return ERR;

    fprintf(outfile, "P2 %d %d %d ", width, height, live);
    for (int i = 0; i < width*height; i++) {
        fprintf(outfile, "%d ", field[i]);
    }

    fclose(outfile);
    return 0;
}

void simStep(cell *src, cell *dst) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cell current = FPOS(src, x, y);
            int nbs = numNeighbours(src, x, y);

            // Game rules are implemented here
            if (current == live && (nbs < 2 || nbs > 3))
                FPOS(dst, x, y) = dead;
            else if (current == dead && nbs == 3)
                FPOS(dst, x, y) = live;
            else
                FPOS(dst, x, y) = current;
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != 5) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    const double density = atof(argv[3]);
    const int steps = atoi(argv[4]);

    printf("width:   %4d\n", width);
    printf("height:  %4d\n", height);
    printf("density: %4.0f%%\n", density * 100);
    printf("steps:   %4d\n", steps);

    // Seeding the random number generator so we get a different starting field
    // every time.
    srand(time(NULL));

    // Allocating game fields
    cell *field1 = malloc(width * height * sizeof(*field1)),
         *field2 = malloc(width * height * sizeof(*field2));

    // Generate random game field
    for (int i = 0; i < width*height; i++)
        field1[i] = ((double)rand() / (double)RAND_MAX) < density ? live : dead;
    
    system("rm -rf " FDIR);
    system("mkdir " FDIR);
    makePBM(FDIR "/" FPREFIX "00000.pbm", field1);

    // Run the simulation
    cell *src = field1,
         *dst = field2;
    char fname[30] = { '\0' };
    for (int s = 1; s <= steps; s++) {
        simStep(src, dst);
        snprintf(fname, 30, FDIR "/" FPREFIX "%05d.pbm", s);
        makePBM(fname, dst);

        cell *tmp = src;
        src = dst;
        dst = tmp;
    }

    puts("Generating GIF, please stand by...");
    system("convert -filter point -resize 300%x300% -delay 50 " FDIR "/" FPREFIX "*.pbm gol.gif");
    system("xdg-open gol.gif");

    free(field1);
    free(field2);

    return EXIT_SUCCESS;
}
