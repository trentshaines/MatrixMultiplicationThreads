/*
 * @Author:  Trent Haines
 * @Program: MatMult
 * @Description: 
 *    This program takes in two matrices with given input files, multiplies them, and stores the resulting matrix
 *    in the specified output file. It computes the multiplication using a specified number of threads. If the number
 *    of threads exceeds the number of rows, then we will limit them and assign each row its own thread.
 * @Date: 3/25/2021
 */

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static vector<vector<int>> inputMat1;
static vector<vector<int>> inputMat2;
static vector<vector<int>> outputMat;

static int u;
static int v;
static int w;

static int numThreads;
static int curThrd = 0;

void *multiply(void*);

int main(int argc, char *argv[]) 
{
    if (argc != 8) // make sure that the valid arguments are provided
    {
        cout << "Please enter a valid number of arguments\n";
        exit(0);
    }


    istringstream iss1(argv[1]); // read in the first argument, number of rows in the first matrix
    iss1 >> u;
    istringstream iss2(argv[2]); // read in the second argument, the number of columns in the first matrix
    iss2 >> v;
    istringstream iss3(argv[3]); // read in the th  ird argument, the number of columns in the second matrix
    iss3 >> w;
    istringstream iss4(argv[7]); // read in the seventh argument, the number of threads to use
    iss4 >> numThreads;
  
    if(numThreads > u) // if we have more threads than rows, limit the to one thread per row
        numThreads = u;

if (u < 0 || v < 0 || w < 0 || numThreads < 0) // make sure that  valid arguments are provided
    {
        cout << "Please enter a positive number for u, v, w, and numThreads\n";
        exit(0);
    }

    outputMat.resize(u); // allocate u*w sized 2d vector for the output, intialize the values to 0 so that we can add products to each entry
    for (size_t i = 0; i < u; ++i) 
    {
        outputMat[i].resize(w);
        for(size_t j = 0; j < w; ++j)
        {
          outputMat[i][j] = 0;
        }
    }
    
    inputMat1.resize(u); // allocate u*v sized 2d vector for the first input
    for (size_t i = 0; i < u; ++i) 
    {
        inputMat1[i].resize(v);
    }

    inputMat2.resize(v); // allocate v*w sized 2d vector for the first input
    for (size_t i = 0; i < v; ++i) 
    {
        inputMat2[i].resize(w);
    }

    ifstream if1(argv[4]); // read the first input file into the 2d vector
    for (size_t i = 0; i < u; ++i) 
    {
        for (size_t j = 0; j < v; ++j) 
        {
            if(!(if1 >> inputMat1[i][j]))
              {
              cout << "Failed to read from input1";
              exit(0);
              } 
        }
    }
    if1.close();

    ifstream if2(argv[5]); // read the second input file into the 2d vector
    for (size_t i = 0; i < v; ++i) 
    {
        for (size_t j = 0; j < w; ++j) 
        {    
          if(!(if2 >> inputMat2[i][j]))
          {
            cout << "Failed to read from input2";
            exit(0);
          }
        }
    }
    if2.close();

    pthread_t threads[numThreads]; // declare the approriate number of threads

    for (size_t i = 0; i < numThreads; ++i)  // create a thread running the multiplication process for each requested thread
    {
        int p = i;
        
        pthread_create(&threads[i], NULL, multiply, &p);
    }

    for (size_t i = 0; i < numThreads; ++i) // join threads back together after producing the output matrix
    {
        pthread_join(threads[i], NULL);
    }
    
    ofstream myOF; 
    myOF.open(argv[6]); // write the output matrix to the specified output file
    for(size_t i = 0; i < u; ++i)
    {
        for(size_t j = 0; j < w; ++j)
        {
            if(j == w-1)
              myOF << outputMat[i][j] << endl; // if end of row, go to next row
            else
              myOF << outputMat[i][j] << " "; // put spaces between entries
        }
    }
    myOF.close();
}

void *multiply(void* args) // function responsible for multiplying with threads
{
    size_t thd = curThrd++; // keeps track of which thread is currently using the function
    for (size_t i = thd * u / numThreads; i < (thd + 1) * u / numThreads; i++) // each thread is responsible for a an equal number of rows (rounded by int division)
    {
      for (size_t j = 0; j < w; j++)
      {
          for (size_t k = 0; k < v; k++) // a full loop through k sums over the dot product for the i,j entry
          {
              outputMat[i][j] += inputMat1[i][k] * inputMat2[k][j]; 
          }      
      }
    }    
}
