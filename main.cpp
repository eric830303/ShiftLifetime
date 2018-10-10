//
//  main.cpp
//  RegionClassification
//
//  Created by TienHungTseng on 2017/7/3.
//  Copyright © 2017年 Eric Tseng. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <math.h>
#define n 100
using namespace std ;


bool comp( double a, double b)//Used in sort(),which is STL func.
{
    if ( a < b ) return true  ;
    else         return false ;
}

double CDF( double x, double m, double s )
{
    
	//double cdf = 0.5*( 1 + erf( (year - avg)/(sd*sqrt(2)) ) );
    double cdf = 0.5 * erfc(-(x-m)/(s*sqrt(2))) ;
	return cdf ;
}

void mission( FILE *foutput1 , FILE *foutput2, vector<double> &vLower, vector<double> &vUpper, double LB, double UB )
{
    double range= ( UB - LB )/n        ;
    
    printf( "粒度＝%f\n", range );
    int    *vctrL= new int [ n ]          ;
    int    *vctrU= new int [ n ]          ;
    double  *vprob= new double  [ 1000 ]          ;
    
    
    //---- Initialization ----------------------------
    for( int i = 0 ; i < n ; i++ ){ vctrL[i] = vctrU[i] = 0 ; }
    
	int index = 0 ;
    for( int i = 0 ; i < vLower.size() ; i++  )
    {
        index = (vLower[i]-LB)/range ;
        vctrL[index] =vctrL[index] + 1 ;
        index = (vUpper[i]-LB)/range ;
        vctrU[index] =vctrU[index] + 1 ;
    }
    
    double avg_prob = 0 ;
	double mean = (UB+LB)/2 ;
	printf("設定中間值=%f\n", mean);
    for( int i = 0 ; i < vLower.size() ; i++  )
    {
        double avg_ins = ( vLower[i] + vUpper[i] )/2 ;
        double sd_ins  = ( vUpper[i] - vLower[i] )*0.95/6;
		double cdf1 =  CDF( mean*1.1, avg_ins, sd_ins ) ;
		double cdf2 =  CDF( mean*0.9, avg_ins, sd_ins ) ;
        vprob[i] = cdf1 - cdf2 ;
        avg_prob += vprob[i] ;
		//printf("avg_ins = %f, sd_ins = %f : %f %f\n", avg_ins, sd_ins, cdf1, cdf2);
    }
    //---- Output -----------------------------------
    for( int i = 0 ; i < n ; i++ )
    {
        fprintf( foutput1, "%f \t %d %d\n", LB + i*range, vctrL[i], vctrU[i] );
    }
    for( int i = 0 ; i < vLower.size()  ; i++ )
    {
        fprintf( foutput2, "Instance(%d): \t %f %f %f \n", i, vLower[i], vUpper[i], vprob[i] );
    }
    avg_prob /= vLower.size() ;
    printf( "平均機率＝%f\n", avg_prob );
}




int main(int argc, const char * argv[])
{
    if( argc < 2 )
    {
        printf("Ex:./dist bench_A_BmV_inst.txt \n") ;
        return -1 ;
    }
    //----- Read parameter.txt ------------------------------------
    string benchmark ;
    double LB = 0, UB = 0 ;
    double Lp_Shift = 0 ;
    double Rp_Shift = 0 ;
    double Vth_pv   = 0 ;
    fstream fpar;
    fpar.open("./parameter.txt")    ;
    string  line    ;
    if( !fpar ) cerr << "Cannot read parameter.txt\n" ;
    while( getline(fpar, line) )
    {
        if     ( line.find("Benchmark")     != string::npos )    benchmark = line.c_str() + 10 ;
        else if( line.find("LB")            != string::npos )    LB        = atof(line.c_str() + 2 )    ;
        else if( line.find("UB")            != string::npos )    UB        = atof(line.c_str() + 2 )    ;
        else if( line.find("Lp_Shift")      != string::npos )    Lp_Shift  = atof(line.c_str() + 8 )    ;
        else if( line.find("Rp_Shift")      != string::npos )    Rp_Shift  = atof(line.c_str() + 8 )    ;
        else if( line.find("Vth_pv")        != string::npos )    Vth_pv    = atof(line.c_str() + 6 )    ;
    }
    //----- Read *_inst.txt ------------------------------------
    ifstream  finst ;
    finst.open( argv[1], ios::in )  ;
    if( !finst ) cerr << "Cannot read " << argv[1] << endl ;
    vector< double > vUpper ;
    vector< double > vLower ;
    getline( finst, line );
    string garbage = "";
    double garbage2= 0;
    while( getline( finst, line ) )
    {
        double L = 0 ;
        double U = 0 ;
        istringstream   token( line )     ;
        token >> garbage >> L >> U >> garbage2;
        
        vUpper.push_back(U) ;
        vLower.push_back(L) ;
    }
    if( vLower.size() != vUpper.size() ) cerr << "vLower.size() != vUpper.size()\n" ;
    //----- Shift ------------------------------------
    double Mean = ( UB + LB )/2 ;
    string distFile = "./shifted_" + benchmark + "_" + to_string( (int)Mean ) + "_" + to_string( (int)Vth_pv ) + "mV_dist.txt" ;
    string instFile = "./shifted_" + benchmark + "_" + to_string( (int)Mean ) + "_" + to_string( (int)Vth_pv ) + "mV_inst.txt" ;
    FILE *foutput1 = fopen(distFile.c_str(),"w+t") ;
    FILE *foutput2 = fopen(instFile.c_str(),"w+t") ;
    
    
    for( int i = 0 ; i < vLower.size() ; i++ )
    {
        vLower[i] += Lp_Shift ;
        vUpper[i] += Rp_Shift ;
    }
    
    
    
    mission( foutput1, foutput2, vLower, vUpper, LB, UB ) ;

}
