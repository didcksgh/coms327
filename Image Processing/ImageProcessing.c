#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *o;

  if (!(o = fopen(file, "w"))) {
    perror(file);

    return -1;
  }

  fprintf(o, "P5\n%u %u\n255\n", x, y);

  /* Assume input data is correctly formatted. *
   * There's no way to handle it, otherwise.   */

  if (fwrite(image, 1, x * y, o) != (x * y)) {
    perror("fwrite");
    fclose(o);

    return -1;
  }

  fclose(o);

  return 0;
}

/* A better implementation of this function would read the image dimensions *
 * from the input and allocate the storage, setting x and y so that the     *
 * user can determine the size of the file at runtime.  In order to         *
 * minimize complication, I've written this version to require the user to  *
 * know the size of the image in advance.                                   */
int read_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *f;
  char s[80];
  unsigned i, j;

  if (!(f = fopen(file, "r"))) {
    perror(file);

    return -1;
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P6\n");

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (sscanf(s, "%u %u", &i, &j) != 2 || i != x || j != y) {
    fprintf(stderr, "Expected x and y dimensions %u %u\n", x, y);
    fclose(f);

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (strncmp(s, "255", 3)) {
    fprintf(stderr, "Expected 255\n");
    fclose(f);

    return -1;
  }

  if (fread(image, 1, x * y, f) != x * y) {
    perror("fread");
    fclose(f);

    return -1;
  }

  fclose(f);

  return 0;
}

void SobelFilter(int8_t image[1024][1024], int8_t out[1024][1024], int width, int height) {


    int kernelX[3][3] = { {-1, 0, 1}, 
                          {-2, 0, 2}, 
                          {-1, 0, 1}};
    int kernelY[3][3] = {{-1, -2, -1}, 
                         {0, 0, 0}, 
                         {1, 2, 1}};

    for(int r = 1; r < height; r++){
      for(int c = 1; c < width; c++){

        int x_accumulator = 0;
        int y_accumulator = 0;

        for(int j = 0; j < 3; j++){
          for(int i = 0; i < 3; i++){

            x_accumulator +=  kernelX[j][i] * image[r+(j-(3/2))][c+(i-(3/2))];
            y_accumulator += kernelY[j][i] * image[r+(j-(3/2))][c+(i-(3/2))];

          }

        }
        int result = sqrt( x_accumulator * x_accumulator + y_accumulator * y_accumulator);

        out[r][c] = result;

      }

    }
}

int main(int argc, char *argv[])
{
  int8_t image[1024][1024];
  int8_t out[1024][1024];
  char fileName[80];
  char r_fileName[80];
  char w_fileName[80];

/**  

  read_pgm("motorcycle.pgm", image, 1024, 1024);

  applySobelFilter(image, out, 1024, 1024);

      
  write_pgm("motorcycle.edge.pgm", out, 1024, 1024);


  read_pgm("bigger_digger.pgm", image, 1024, 1024);

  applySobelFilter(image, out, 1024, 1024);

  write_pgm("bigger_digger.edge.pgm", out, 1024, 1024);

  **/

  printf("Enter the file name excluding .pgm: ");

  scanf("%s", fileName);

  //printf("%s\n", fileName);

  strcpy(r_fileName, fileName);

  strcat(r_fileName, ".pgm");

  strcpy(w_fileName, fileName);

  strcat(w_fileName, ".edge.pgm");

  //printf("%s\n" , r_fileName);

  read_pgm(r_fileName, image, 1024, 1024);

  SobelFilter(image, out, 1024, 1024);
  
  //printf("%s\n", w_fileName);

  //printf("%s", w_fileName);
  

  write_pgm(w_fileName, out, 1024, 1024);

  
  return 0;
}