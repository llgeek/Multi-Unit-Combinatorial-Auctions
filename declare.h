//
//  declare.h
//  auction
//
//  Created by lchen on 2/15/16.
//  Copyright © 2016 lchen. All rights reserved.
//

#ifndef declare_h
#define declare_h

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator> 
#include <vector>
#include <map>
#include <utility>

using namespace std;

// const int UNITS_max = 5;
// const int AVGPRICE_base = 50;
// const int AVGPRICE_var = 25;

// const double PROB1 = 0.8;
// const double PROB2 = 0.65;
// const double PRICE_var = 0.5;


class Goods{
public:
    int units;
    int aveprice;
    int id;
    
    int unitsrest;
    int unitsrestforsetbidcopy;
    
    
public:
    void setUnits(int units)
    {
        this->units = units;
    }
    void setUnitsRest (int units)
    {
        this->unitsrest = units;
    }
    void setAveprice(int aveprice)
    {
        this->aveprice = aveprice;
    }
    
    int getUnits()
    {
        return units;
    }
    int getUnitsRest()
    {
        return unitsrest;
    }
    int getAveprie()
    {
        return aveprice;
    }
    void decreaseOneUnitsRest()
    {
        unitsrest --;
    }
    
};

class Bidder{
public:
    //	std::vector< int> bidset;
    std::map<int, int> bidset;		//goods id and copies
    std::vector<int> bidsetid;		//only contains bid id, for purpose of reducing radom choosing conplexity
    double bidprice;
    int	totalunits;
    int id;
};


class Auction{
public:
    std::vector<Goods> goods;
    std::vector<Bidder> bidder;
    
private:
    double greedygain;
    double optimumgain;
    
    int num_goods;
    int num_bids;
    int total_units;
    
public:
    Auction(int numgoods, int numbids);
    ~Auction();
    
    void goodsSetUnits();
    void checkUnits();
    
    void goodsSetPrice();
    
    void bidderSetGoodsID();
    void bidderSetGoodsCopy();
    
    void bidderSetBidPrice();
    
    
    void OptimumAuction();
    
    void GreedyAuction();
    
    //	void optimumgainAddValue(double value);
    double optimumgainGetVaule()
    {
        return optimumgain;
    }
    
    //	void greedygainAddValue(double value);
    double greedygainGetValue()
    {
        return greedygain;
    }
    double totalUnitsValue()
    {
        return total_units;
    }
    
    
private:
    bool checkWetherExist(map<int, int> _bidset, vector<Goods> _goods);		//check whether more units left
    void decreaseUnitsRestByBidder(map<int, int> _bidset, vector<Goods>& _goods);		//decrease the rest units number according to the previous bidder
    
    double checkWetherCoExist(vector<int> _com); //check whether could co-exist togather
};


#endif /* declare_h */
