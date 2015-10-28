#include <Rand.h>
#include <stdlib.h>
#include <TRandom3.h>
#include <iostream>

TRandom3
Rand::fRandomGen = TRandom3();

double
Rand::Uniform(double max_){
    return max_ * ((double) rand() / (RAND_MAX));
}

double 
Rand::Gaus(double mean_, double sigma_){
    //    rand3.SetSeed();
    return fRandomGen.Gaus(mean_, sigma_);
}
