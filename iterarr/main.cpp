#include <cassert>
#include <iostream>

#include <vector>
#include <sstream>
#include <fstream>
#include <random>
#include <time.h>

using namespace std;

enum Mode {
    STRAIT = 0, REVERSE, PSEUDO_RANDOM, RANDOM
};

// https://stackoverflow.com/questions/13772567/get-cpu-cycle-count
uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
string string_of_mode(Mode m) {
  switch (m) {
  case STRAIT: return "Strait";
  case REVERSE: return "Reverse";
  case PSEUDO_RANDOM: return "Pseudo";
  case RANDOM:   return "Random";
  }
  return "";
}

int foo(int x) {
  return x + 1;
}
uint64_t f(int n, Mode m) {
    uint64_t t1, t2;
    vector<int> arr(n);
    vector<int> idx(n);
    int x;
    if (m == STRAIT) {
        vector<int> arr(n);
	//auto it = arr.begin(), it2 = arr.end();
        t1 = rdtsc();
        for (int i=0; i<n; ++i) x = arr[i];
        //for (; it != it2; ++it) x = (*it);
        t2 = rdtsc();
    } else if (m == REVERSE) {
        t1 = rdtsc();
        for (int i=n-1; i>=0; --i) x = arr[i];
        t2 = rdtsc();
    } else if (m==PSEUDO_RANDOM) {
        for (int i=0; i<n; ++i) idx[i] = rand()%n;;

        t1 = rdtsc();
        for (int i=0; i<n; ++i)     x = arr[ idx[i] ];
        t2 = rdtsc();
    } else if (m==RANDOM) {
        for (int i=0; i<n; ++i) idx[i] = rand() % n;

        t1 = rdtsc();
        for (int i=0; i<n; ++i)     x = arr[ idx[i] ];
        t2 = rdtsc();
    } else {
        assert(false);
    }

    foo(x);
    uint64_t d = (uint64_t)(t2-t1);
    return d;
}

template<typename Functor>
uint64_t myavg(Functor f)
{
    uint64_t avg = 0.0;
    int n = 10;
    for (int i=0; i<n; ++i)
        avg += f();

    return avg/n;
}

int main() {
    long ans;
    cout.precision(10);
    long _32k = 32768;
    long sizes[] = { _32k, _32k << 1, _32k << 2, _32k << 3, 
                     _32k * 16,
                     _32k * 32,   // 1Mb
                     _32k * 64,   // 2Mb
                     _32k * 128,  // 4Mb
                     _32k * 256,  // 8Mb
                     _32k * 512,  // 16Mb
                     _32k * 600,  
                     _32k * 700,  
                     _32k * 800,  
                     _32k * 900,  
                     _32k * 1024, // 32Mb
    };
    int sizes_count = sizeof(sizes)/sizeof(long);
    uint64_t results[sizes_count][6];
	
    for (int i=0; i<sizes_count; ++i) {
      results[i][0] = i;
      results[i][1] = sizes[i];
      for (int j=2; j<6; j++)
        results[i][j]=0;
    }

    std::ofstream ofs;
    ofs.open ("cpp.out", std::ofstream::out);
    ofs.precision(10);
    /*
    for (int i=0; i<sizes_count; ++i) {
      for (int j=0; j<6; ++j) {
        ofs << results[i][j]  << " ";
      }
      ofs << endl;
    } */
 
   for (int m=0; m<=2; ++m) {
      Mode mode = (Mode)m;
      for (int i=0; i<sizes_count; ++i) {
        long size = sizes[i];
        cout << string_of_mode(mode) << " for " << size << "  takes ";
        ans = myavg([mode, size]() { return f(size, mode); });
	cout << ans << endl;
        results[i][2+m] = ans;
      }
    }
    
    Mode mode = RANDOM;
    
    for (int i=0; i<sizes_count; ++i) {
      long size=sizes[i];
        cout << string_of_mode(mode) << " for " << size << "  takes ";
        ans = myavg([mode, size]() { return f(size, mode); });
	cout << ans << endl;
	results[i][5] = ans;
    }
    // printing results;
    for (int i=0; i<sizes_count; ++i) {
      for (int j=0; j<6; ++j) {
 	uint64_t ans = (j<2) ? results[i][j] : (results[i][j]/1000);
        ofs << ans << " ";
      }
      ofs << endl;
    }
    ofs.close();
    
    return 0;
}
