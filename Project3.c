#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include <string.h>

FILE *fp;

unsigned int *p1;

int taglength, setlength, offsetlength;

int L, K, C;

int row;

float total=0, miss=0;
int i, j;


//defining the methods

int tagBits(unsigned int address);

int whichSet(unsigned int address);

void insert(unsigned int *address, unsigned int **cache, int **lru, int column);

int hitWay(unsigned int *address,  unsigned int **cache, int column);

void updateOnHit(unsigned int **cache, unsigned int *address, int index, int **lru);

void updateOnMiss(unsigned int *address, unsigned int **cache, int **lru, int column);

int offsetLength();

int setIndexLength();



//L is the block size, K is the set associativity, C is the cache size

void main(int argc, char **argv)

{

K= atoi(argv[1]);

L= atoi(argv[2]);

C= atoi(argv[3]);

char * file;

file = argv[4];

row = (C*1024)/(K*L);



int column=K;

taglength= 32-(setIndexLength()+offsetLength());

setlength= setIndexLength();

offsetlength= offsetLength();

//creates cache with row size of cache/(set*block size)

//and column equal to # of blocks per set, K. we use malloc to find space for

//each set. Set size is equal to set*block size

unsigned int **cache;

int **lru;



cache = (unsigned int **) malloc(row * sizeof(unsigned int *));

for (i = 0; i < row; ++i) {

  cache[i] = (unsigned int *) malloc(K * sizeof(unsigned int));

}



lru = (int **) malloc(row * sizeof(int *));

for (i = 0; i < row; ++i) {

   lru[i] = (int *) malloc(K * sizeof(int));

}//going to pointers 2 deep and setting the value of that

                       //location equal to 0

//creates lru same as cache but intead size of will only stor an int



      //set values=-1 for lru array

       for (i = 0; i <  row; i++){

       for (j = 0; j < column; j++){

          lru[i][j] = -1;

          cache[i][j] = 0;

        }

      }

p1 =(int*) malloc(8*sizeof(int));



fp = fopen(file, "r");

  while(fscanf(fp, "%x", p1) != EOF) {

//TEST PRINTING

//printf("%x\n",*p1);

//printf("%x tag\n",*tagBits(p1));

//printf("%x set\n\n",*whichSet(p1));



  insert(p1, cache, lru, K);
  p1 = realloc(p1,8);
}


float missrate= miss/total;

printf("%s %d %d %d %.4f", file, C, K, L, missrate);

fclose(fp);


}

  int offsetLength()

  {

    int temp = L, count=0;

    while(!(temp<=1))

    {

    temp = temp/2;

    count++;

    }

    return count;

  }



      int setIndexLength()

  {

    double temp = row, count=0;



    while(!(temp<=1))

    {

    temp = temp/2;

    count++;

    }

    return count;



  }



    int tagBits(unsigned int address)

    {

      int shiftamount=(setIndexLength()+offsetLength());

        address = address>>shiftamount;

            return address; //return tag bits

    }



    int whichSet(unsigned int address)

    {

      //unsigned int *new;

      //  *new = *address>>2;



          //  return address;

          int shiftamount=(setIndexLength()+offsetLength());

          //unsigned int *new;

            address = address<<taglength;

            address = address>>(offsetLength()+taglength);

                return address;

    }



    int hitWay(unsigned int *address,  unsigned int **cache,  int column)

    {

        //printf("  ");

        int set= whichSet(*address);

        int tag= tagBits(*address);

          for(i=0; i<column; ++i)
          {
          if(cache[set][i]==tag)
            return i;
          }

        return -1;

    }



    void insert(unsigned int *address, unsigned int **cache, int **lru, int column)

    {



      int check= hitWay(address, cache, column);

      if(check!=-1)

          updateOnHit(cache, address, check, lru);

        //check if already in

        else

         updateOnMiss(address, cache, lru, column);

    }



    void updateOnHit(unsigned int **cache, unsigned int *address, int index, int **lru)

    {printf("%n");

      total++;

      //printf("%f\n",total);

      int set= whichSet(*address);
      //printf("%d",set);
      //cache[set][index]=tagBits(*address);

      for(j=0; j<K;j++)
      if(lru[set][j]!=-1&&(lru[set][j]<lru[set][index]))
          lru[set][j]++;

      lru[set][index]=0;

    }



void updateOnMiss(unsigned int *address, unsigned int **cache, int **lru, int column)

{printf("%n");



  miss++;

  //printf("%f\n",miss);

  total++;

  int set= whichSet(*address);

  int top=-1;

  int spot=0;



  for(i=0;i<column;i++)

  {

  if(lru[set][i]==-1)

    {
      spot=i;
    break;
    }

  else if(lru[set][i]>top)

    {

    top=lru[set][i];

    spot=i;

    }

  }



  cache[set][spot]=tagBits(*address);

  for(j=0; j<K;j++)
  if(lru[set][j]!=-1&&(lru[set][j]<lru[set][spot]))
      lru[set][j]++;

    lru[set][spot]=0;

    return;

}