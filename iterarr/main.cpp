#include <cassert>
#include <iostream>
//#include <chrono>

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

uint64_t f(int n, Mode m) {
    uint64_t t1, t2;
    string msg;
    if (m == STRAIT) {
        vector<int> arr(n);
        int x;
        auto it = arr.begin(), it2 = arr.end();
        t1 = rdtsc();
        for (; it != arr.end(); ++it) x = (*it);
        //for (auto& i : arr)     x=i;
        t2 = rdtsc();
    } else if (m == REVERSE) {
        vector<int> arr(n);
        int x;
        auto it = arr.rbegin();
        t1 = rdtsc();
        for (; it != arr.rend(); ++it) x = (*it);
        t2 = rdtsc();
    } else if (m==PSEUDO_RANDOM) {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, n);
        vector<int> idx(n);
        for (int i=0; i<n; ++i) idx[i] = distribution(generator);
        int x;
        vector<int> arr(n);

        t1 = rdtsc();//start = chrono::steady_clock::now();
        for (int i=0; i<n; ++i)     x = arr[ idx[i] ];
        t2 = rdtsc();//end   = chrono::steady_clock::now();
    } else if (m==RANDOM) {
        std::random_device rd;
        std::default_random_engine generator( rd() );
        std::uniform_int_distribution<int> distribution(0, n);
        vector<int> idx(n);
        for (int i=0; i<n; ++i) idx[i] = distribution(generator);
        int x;
        vector<int> arr(n);

        t1 = rdtsc();//start = chrono::steady_clock::now();
        for (int i=0; i<n; ++i)     x = arr[ idx[i] ];
        t2 = rdtsc();//end   = chrono::steady_clock::now();
    } else {
        assert(false);
    }


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
    long sizes[] = { _32k, _32k << 1, _32k << 2, _32k << 3, _32k << 4,
                     _32k << 5,  // 1Mb
                     _32k << 6,  // 2Mb
                     _32k << 7,  // 4Mb
                     _32k << 8,  // 8Mb
                     _32k << 9,  // 16Mb
                     _32k << 10, // 32Mb
    };

    std::ofstream ofs;
    ofs.open ("cpp.out", std::ofstream::out);
    ofs.precision(10);

    for (int i=0; i< sizeof(sizes)/sizeof(long); ++i) {
        long size = sizes[i];
        ofs << i << " " << size << " ";

        ans = myavg([size]() { return f(size, STRAIT); });
        ofs << fixed << ans << " ";
        cout << "Strait        for " << size << "  takes " << ans << endl;

        ans = myavg([size]() { return f(size, REVERSE); });
        ofs << fixed << ans << " ";
        cout << "Reverse       for " << size << "  takes " << ans  << endl;

        ans = myavg([size]() { return f(size, PSEUDO_RANDOM); });
        ofs << fixed << ans << " ";
        cout << "Pseudo-random for " << size << "  takes " << ans  << endl;

        ans = myavg([size]() { return f(size, RANDOM); });
        ofs << fixed << ans << " ";
        cout << "Really-random for " << size << "  takes " << ans << endl;

        ofs << "\n";
    }
    ofs.close();
    return 0;
}
