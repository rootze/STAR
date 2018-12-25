#include "SoloCB.h"

bool inputFeatureUmi(fstream *strIn, int32 featureType, uint32 &feature, uint32 &umi)
{
    if (!(*strIn >> umi)) //end of file
        return false;
    
    if (featureType==0) {
        *strIn >> feature;
    } else {
    };
    
    return true;
        
};

void SoloCB::inputUMIfeatureCBrecords(uint32 ** cbP, uint32 *cbReadCountExactTotal, vector<array<uint64,2>> sjAll) 
{
    {//load exact matches
        strU_0->flush();
        strU_0->seekg(0,ios::beg);
        uint32 cb, feature, umi;
        while (inputFeatureUmi(strU_0,featureType, feature, umi)) {
            *strU_0 >> cb;
            if (feature != (uint32)(-1)){
                cbP[cb][0]=feature;
                cbP[cb][1]=umi;
                cbP[cb]+=2;
                stats.V[stats.nExactMatch]++;
            };
        };
    };
    
    {//load 1MM
        strU_1->flush();
        strU_1->seekg(0,ios::beg);
        uint32 cb, feature, umi;
        while (inputFeatureUmi(strU_1,featureType, feature, umi)) {
            *strU_1 >> cb;
            if (cbReadCountExactTotal[cb]>0) {
                if (feature != (uint32)(-1)){
                    cbP[cb][0]=feature;
                    cbP[cb][1]=umi;
                    cbP[cb]+=2;
                };
            } else {
                stats.V[stats.nNoExactMatch]++;
            };
        };
    };
    
    {//load 2MM
        strU_2->flush();
        strU_2->seekg(0,ios::beg);
        uint32 cb=0, feature, umi, ncb;
        while (inputFeatureUmi(strU_2,featureType, feature, umi)) {
            if (feature == (uint32) (-1)) {
                strU_2->ignore('\n',(uint32) (-1));//ignore until the end of the line
                continue; //nothing to record
            };
            *strU_2 >> ncb;
            float ptot=0.0,pmax=0.0;
            for (uint32 ii=0; ii<ncb; ii++) {
                uint32 cbin;
                char  qin;
                float pin;
                *strU_2 >> cbin >> qin;
                if (cbReadCountExactTotal[cbin]>0) {//otherwise this cbin does not work
                    qin -= pSolo.QSbase;
                    qin = qin < pSolo.QSbase ? qin : pSolo.QSbase;
                    pin=cbReadCountExactTotal[cbin]*pow(10.0,-qin/10.0);
                    ptot+=pin;
                    if (pin>pmax) {
                        cb=cbin;
                        pmax=pin;
                    };
                };
            };
            if (ptot>0.0 && pmax>=pSolo.cbMinP*ptot) {
                cbP[cb][0]=feature;
                cbP[cb][1]=umi;
                cbP[cb]+=2;
            } else {
                stats.V[stats.nTooMany]++;
            };        
        };
    };
};
