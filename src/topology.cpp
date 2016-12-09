// random_shuffle example
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <stdio.h>

int _x, _y, _z, _t;

void set(int a)
{
  _x = a / 64; a -= _x*64;
  _y = a / 8; a -= _y*8;
  _z = a / 1; a -= _z*1;
  _t = a;
}

// random generator function:
int myrandom (int i) { return std::rand()%i;}

int main () {
  std::srand ( unsigned ( std::time(0) ) );
  std::vector<int> myvector;

  // set some values:
  for (int i=0; i<512; ++i) myvector.push_back(i);

  // using built-in random generator:
  std::random_shuffle ( myvector.begin(), myvector.end() );

  // using myrandom:
  std::random_shuffle ( myvector.begin(), myvector.end(), myrandom);

  FILE *f = fopen("data/topology.map", "w");

  // print out content:
  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
  {
    set(*it);
    fprintf(f, "%d %d %d 0\n", _x, _y, _z);
    if(_x > 7 || _y > 7 || _z > 7 || _x < 0 || _y < 0 || _z < 0)
    {
      fprintf(stderr, "Все плохо: генерация мэппинга выдала (%d, %d, %d) на число %d\n", _x, _y, _z, *it);
      exit(2);
    }
  }
  return 0;
}