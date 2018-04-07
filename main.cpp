//
//  main.cpp
//  auction
//
//  Created by lchen on 2/15/16.
//  Copyright  © 2016 lchen. All rights reserved.
//

#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <functional>
#include <cmath>
#include <list>
#include <fstream>
#include "./declare.h"

using namespace std;

const int UNITS_max = 5;
const int AVGPRICE_base = 50;
const int AVGPRICE_var = 25;

const double PROB1 = 0.8;
const double PROB2 = 0.65;
const double PRICE_var = 0.5;


int intUniforRandom(int max, int min)
{
    //   int i = ((int) rand() / RAND_MAX * (max-min) + min);
    //   int i = rand() % (max-min+1) + min;
    int k = ((double)rand() / RAND_MAX) * (max - min) + min;
    // return  ((int) rand() / RAND_MAX * (max-min) + min);
    return k;
}

double doubleUnitforRandom(double max, double min)
{
    double i = ((double)rand() / RAND_MAX) * (max - min) + min;
    //return ((double) rand() /RAND_MAX * (max - min) + min);
    return i;
}

int factorial_n(int n)
{
    if (n == 0)
        return 1;
    else
        return n*factorial_n(n-1);
}

Auction::Auction(int numbids, int numgoods)
{
    this->num_goods = numgoods;
    this->num_bids = numbids;
    
    greedygain = 0;
    optimumgain = 0;
    
    goods.resize(num_goods);
    bidder.resize(num_bids);
    
    for (int i = 0; i < numgoods; i++)
    {
        goods[i].id = i;
        goods[i].units = 0;
        goods[i].unitsrest = 0;
        goods[i].unitsrestforsetbidcopy = 0;
    }
    for(int i = 0; i < numbids; i++)
    {
        bidder[i].id = i;
        bidder[i].totalunits = 0;
        bidder[i].bidprice = 0;
    }
    
    checkUnits();
    goodsSetPrice();
    bidderSetGoodsID();
    bidderSetGoodsCopy();
    bidderSetBidPrice();
}

Auction::~Auction()
{
    goods.clear();
    bidder.clear();
    
}


void Auction::goodsSetUnits()
{
    for(int i = 0; i < num_goods; i ++)
    {
        //	goods[i].units = rand() % UNITS_max + 1;
        int randnum = rand() % UNITS_max + 1;
        goods[i].setUnits(randnum);
        goods[i].setUnitsRest(randnum);
        goods[i].unitsrestforsetbidcopy = randnum;
    }
}

void Auction::checkUnits()
{
    int temp_units = 1;
    int temp_exp = 0;
    
    //	goodsSetUnits();
    
    while(temp_units != ((num_goods * temp_exp/UNITS_max)))
    {
        goodsSetUnits();
        temp_units = 0;
        temp_exp = 0;
        for (int i = 0; i < num_goods; i++)
        {
            temp_units += goods[i].getUnits();
        }
        total_units = temp_units;
        for (int i = 0; i < UNITS_max; i++)
        {
            temp_exp += i;
        }
        
    }
}


void Auction::goodsSetPrice()
{
    for(int i = 0; i < num_goods; i++)
    {
        goods[i].setAveprice(intUniforRandom(AVGPRICE_base + AVGPRICE_var, AVGPRICE_base - AVGPRICE_var));
    }
}


void Auction::bidderSetGoodsID()
{
    double prob_choose = 0;
    int goods_candidate = -1;
    int goods_rest = num_goods;
    for (int i = 0; i < num_bids; i++)
    {
        prob_choose = 0;
        goods_candidate = -1;
        goods_rest = num_goods;
        while ( prob_choose < PROB1 & goods_rest > 0)
        {
            goods_candidate = intUniforRandom(num_goods - 1, 0);
            if (bidder[i].bidset.count(goods_candidate) == 0)
            {
                bidder[i].bidset.insert(pair<int, int>(goods_candidate, 1));
                bidder[i].bidsetid.push_back(goods_candidate);
                bidder[i].totalunits ++;
                goods[goods_candidate].unitsrestforsetbidcopy --;
                goods_rest --;
            }
            prob_choose = doubleUnitforRandom(1, 0);
        }
    }
}

void Auction::bidderSetGoodsCopy()
{
    double prob_choose = 0;
    int goods_candidate = -1;
    int randnum = 0;
    
    std::vector<Goods> goodstemp;
    
    
    for (int i = 0; i < num_bids; i++)
    {
        randnum = intUniforRandom(bidder[i].bidsetid.size() - 1, 0);
        goods_candidate = bidder[i].bidsetid[randnum];
        prob_choose = doubleUnitforRandom(1, 0);
        goodstemp = goods;
        while (prob_choose < PROB2 && goodstemp[goods_candidate].unitsrestforsetbidcopy > 0)
        {
            bidder[i].bidset[goods_candidate] ++;
            bidder[i].totalunits ++;
            goodstemp[goods_candidate].unitsrestforsetbidcopy --;
            
            randnum = intUniforRandom(bidder[i].bidsetid.size() - 1, 0);
            goods_candidate = bidder[i].bidsetid[randnum];
            prob_choose = doubleUnitforRandom(1, 0);
        }
    }
}

void Auction::bidderSetBidPrice()
{
    double price = 0;
    std::map<int, int>::iterator it;
    for (int i = 0; i < num_bids; i++)
    {
        price = 0;
        for (it = bidder[i].bidset.begin(); it != bidder[i].bidset.end(); it++)
        {
            price += goods[it->first].getAveprie() * it->second;
        }
        price *= doubleUnitforRandom (1 + PRICE_var, 1 - PRICE_var);
        bidder[i].bidprice = price;
    }
}

/* n! complexity
 ** re-write with 2^n complexity
 **
 void Auction::OptimumAuction()
 {
 std::vector<int> enumbidderid;
 std::vector<Goods> goodstemp;
 
 for (int i = 0; i < num_bids; i++)
 enumbidderid.push_back(i);
 std::sort (enumbidderid.begin(), enumbidderid.end());
 double pricetemp = 0;
 do
 {
 goodstemp = goods;
 pricetemp = 0;
 for(int i = 0; i < enumbidderid.size(); i++)
 {
 if (checkWetherExist(bidder[enumbidderid[i]].bidset, goodstemp))
 {
 decreaseUnitsRestByBidder(bidder[enumbidderid[i]].bidset, goodstemp);
 pricetemp += bidder[enumbidderid[i]].bidprice;
 }
 }
 if (pricetemp > optimumgain)
 optimumgain = pricetemp;
 }while (next_permutation(enumbidderid.begin(), enumbidderid.end()));
 }
 */

void Auction::OptimumAuction()
{
    vector<int> numcombination;
    unsigned long nbits = 1 << num_bids;
    for (unsigned long i = 0; i < nbits; i++)
    {
        numcombination.clear();
        for(int j = 0; j < num_bids; j++)
        {
            unsigned long tmp = 1 << j;
            if((tmp & i) != 0)
                numcombination.push_back(j);
        }
        double pricetmp = checkWetherCoExist(numcombination);
        
        if (pricetmp > optimumgain)
            optimumgain = pricetmp;
        
    }
}

double Auction::checkWetherCoExist(vector<int> _com)
{
    double pricetmp = 0;
    vector<Goods> goodstmp = goods;
    
    for(int i = 0; i < _com.size(); i++)
    {
        if(checkWetherExist(bidder[_com[i]].bidset, goodstmp))
        {
            decreaseUnitsRestByBidder(bidder[_com[i]].bidset, goodstmp);
            pricetmp += bidder[_com[i]].bidprice;
        }
        else
            return 0;
        
    }
    return pricetmp;
}


typedef pair<int, double> PAIR;
bool CmpByValueDecrease(const PAIR& m1, const PAIR& m2){
    return m1.second > m2.second;
}

void Auction::GreedyAuction()
{
    std::vector<PAIR> priceperbid;
    std::vector<Goods> goodstemp;
    
    for (int i = 0; i < num_bids; i++)
    {
        priceperbid.push_back(pair<int, double>(bidder[i].id, bidder[i].bidprice/sqrt(double(bidder[i].totalunits))));
    }
    sort(priceperbid.begin(), priceperbid.end(), CmpByValueDecrease);
    
    goodstemp = goods;
    for (int i = 0; i < priceperbid.size(); i++)
    {
        if (checkWetherExist(bidder[priceperbid[i].first].bidset, goodstemp))
        {
            decreaseUnitsRestByBidder(bidder[priceperbid[i].first].bidset, goodstemp);
            
            greedygain += bidder[priceperbid[i].first].bidprice;
        }
    }
}

bool Auction::checkWetherExist(map<int, int> _bidset, vector<Goods> _goods)
{
    map<int, int>::iterator it;
    for (it = _bidset.begin(); it != _bidset.end(); it++)
    {
        if (it->second > _goods[it->first].unitsrest)
            return false;
    }
    return true;
}

void Auction::decreaseUnitsRestByBidder(map<int, int> _bidset, std::vector<Goods>& _goods)
{
    map<int, int>::iterator it;
    for (it = _bidset.begin(); it != _bidset.end(); it++)
    {
        _goods[it->first].unitsrest -= it->second;
    }
}



int main()
{
    srand((unsigned)time(NULL));
    
    int numofbid[6] = {5, 10, 15, 20, 25, 30};
    int numofgoods[6] = {10, 20, 30, 40, 50, 60};
    
    int totalcopy[6][6] = {0};
    double ratio_avg[6][6] = {0.0};
    double ratio_largest[6][6] = {0.0};
    std::vector<double> ratio[6][6];
    
    double tmp = 0.0;
    
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            switch (i) {
                case 0:
                case 1:
                case 2:
                case 3:
                    totalcopy[i][j] = 0;
                    ratio_avg[i][j] = 0.0;
                    ratio_largest[i][j] = 0.0;
                    for(int k = 0; k < 10; k++)
                    {
                        Auction auction(numofbid[i], numofgoods[j]);
                        auction.GreedyAuction();
                        auction.OptimumAuction();
                        totalcopy[i][j] += auction.totalUnitsValue();
                        tmp = (double)auction.optimumgainGetVaule() / auction.greedygainGetValue();
                        ratio_avg[i][j] += tmp;
                        ratio[i][j].push_back(tmp);
                        if (tmp > ratio_largest[i][j])
                            ratio_largest[i][j] = tmp;
                        //           ratio[i][j] += ((double)auction.optimumgainGetVaule() / auction.greedygainGetValue());
                    }
                    totalcopy[i][j] /= 10;
                    ratio_avg[i][j] /= 10;
                    
                    break;
                case 4:
                    totalcopy[i][j] = 0;
                    ratio_avg[i][j] = 0.0;
                    ratio_largest[i][j] = 0.0;
                    for(int k = 0; k < 5; k++)
                    {
                        Auction auction(numofbid[i], numofgoods[j]);
                        auction.GreedyAuction();
                        auction.OptimumAuction();
                        totalcopy[i][j] += auction.totalUnitsValue();
                        tmp = (double)auction.optimumgainGetVaule() / auction.greedygainGetValue();
                        ratio_avg[i][j] += tmp;
                        ratio[i][j].push_back(tmp);
                        if (tmp > ratio_largest[i][j])
                            ratio_largest[i][j] = tmp;
                        //   ratio[i][j] += ((double)auction.optimumgainGetVaule() / auction.greedygainGetValue());
                    }
                    totalcopy[i][j] /= 5;
                    ratio_avg[i][j] /= 5;
                    
                    break;
                case 5:
                    totalcopy[i][j] = 0;
                    ratio_avg[i][j] = 0.0;
                    ratio_largest[i][j] = 0.0;
                    for(int k = 0; k < 3; k++)
                    {
                        Auction auction(numofbid[i], numofgoods[j]);
                        auction.GreedyAuction();
                        auction.OptimumAuction();
                        totalcopy[i][j] += auction.totalUnitsValue();
                        tmp = (double)auction.optimumgainGetVaule() / auction.greedygainGetValue();
                        ratio_avg[i][j] += tmp;
                        ratio[i][j].push_back(tmp);
                        if (tmp > ratio_largest[i][j])
                            ratio_largest[i][j] = tmp;
                        //    ratio[i][j] += ((double)auction.optimumgainGetVaule() / auction.greedygainGetValue());
                    }
                    totalcopy[i][j] /= 3;
                    ratio_avg[i][j] /= 3;
                    break;
                default:
                    break;
            }
        }
    }
    
    ofstream result("result.csv");
    if(!result)
    {
        cout<<"Failed to open file."<<endl;
        return 1;
    }
    result<<"ID, Total Bidders #, Total Copies #, Approximation Ratio, Largest Ratio"<<endl;
    int id = 1;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            result<<id<<", "<<numofbid[i]<<", "<<totalcopy[i][j]<<", "<<ratio_avg[i][j]<<", "<<ratio_largest[i][j]<<endl;
            id++;
        }
    }
    result.close();
    
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            cout<< "#bid is: "<<numofbid[i] <<" #goods is: "<< numofgoods[j]<<endl;
            copy (ratio[i][j].begin(), ratio[i][j].end(), ostream_iterator<double> (cout, "\t"));
            cout<<endl<<endl<<endl;
        }
    }
    
    return 0;
}
