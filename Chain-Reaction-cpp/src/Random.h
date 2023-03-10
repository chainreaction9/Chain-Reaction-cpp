#ifndef RANDOM_H
#define RANDOM_H

#ifndef _PRECOMPILED_H
#include "PrecompiledHeader.h"
#endif // !_PRECOMPILED_H

using namespace std;

class Random{
	public:
		static double random(double a, double b){
			static int SEED_DEFINED = 0;
			if (!SEED_DEFINED) set_seed(&SEED_DEFINED);
			double len = b - a;
			double val = (double) rand()/(RAND_MAX + 1) * len + a;
			return val;
		}
	private:
		static double init_random(double a, double b){
			double len = b - a;
			double val = (double) rand()/(RAND_MAX + 1) * len + a;
			return val;
		};

		static void set_seed(int *seed_indicator){
			*seed_indicator = 1;

			unsigned int init_seed = (unsigned) time(NULL);
			unsigned int seed;
			int i;

			srand(init_seed);
			int break_point;
			for (i=0; i<50; i++) break_point = (int) init_random(33,100);
			for(i=0 ; i<100; i++){
				seed = (int) init_random(init_seed, init_seed*33);
				if (i == break_point) break;
			}
			srand(seed);
		}
};
#endif // !RANDOM_H
