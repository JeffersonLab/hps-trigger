/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   THPSTrigVect.h
 * Author: rafopar
 *
 * Created on June 21, 2019, 10:27 AM
 */

#ifndef THPSTRIGVECT_H
#define THPSTRIGVECT_H

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>
#include <vector>
#include <map>

// ===== The HPS Calorimeter cluster object

typedef struct {
    int E;
    int Y;
    int X;
    int N;
    int T;
} THPS_Cluster;

// ===== The Singles trig object

typedef struct {
    int instance; // The instance
    bool top_bot; // Top or bottom
    bool H_L1L2X_GEOM_PASS; // 
    bool H_L1L2_GEOM_PASS; // Coincidence between Hodo L1 and L2
    bool H_L2_PASS; // Hodo L2 pass
    bool H_L1_PASS; // Hodo L1 pass
    bool PDE_PASS; // Position dependent Energy cut
    bool MINX_PASS; // if the cluster X is above the min-X
    bool NMIN_PASS; // # of hits in the cluster
    bool EMAX_PASS; // The cluster energy should be below of the EMAX
    bool EMIN_PASS; // The cluster energy should be above of the EMAX
    int T; // Time of the trigger decision
} THPS_Singles_Trg;

// ===== The Pair trig object

typedef struct {
    int instance; // The instance
    int reserved; // has no meaning yet
    bool coplan_pass; // passed the coplanarity cut
    bool Edslope_pass; // passed the energy distance slope
    bool Ediff_pass; // passed the enegy difference cut
    bool summ_pass; // passed the Energy sum cut
    int T; // TIme of the trigger decision
} THPS_PairTrig;

using namespace std;

class THPSTrigVect {
public:
    THPSTrigVect();
    //THPSTrigVect(const THPSTrigVect& orig);
    THPSTrigVect(vector<int> *, vector<int> *); // The constructor with vectors of crate IDs and words
    void SetVectors(vector<int> *, vector<int> *); // Sets vectors of crateIDs and words

    int GetNClust() {
        return fn_HPS_Cl;
    }; // Returns number of clusters

    int GetNTopClust() {
        return fn_HPS_Top_Cl;
    }; // Returns number of Top clusters

    int GetNBotClust() {
        return fn_HPS_Bot_Cl;
    }; // Returns number of Top clusters

    
    int GetNSingleTrg() {return fn_HPS_SingleTrg;}; // Return # of single Trg objects
    int GetNTopSingleTrg() {return fn_HPS_Top_SingleTrg;}; // Return # of Top single Trg objects
    int GetNBotSingleTrg() {return fn_HPS_Bot_SingleTrg;}; // Return # of Bot single Trg objects
   
    
    THPS_Cluster *GetCLuster(int i); // Get i-th cluster
    THPS_Cluster *GetTopCLuster(int i); // Get i-th Top cluster
    THPS_Cluster *GetBotCLuster(int i); // Get i-th Bot cluster

    THPS_Singles_Trg *GetSingleTrg(int i);        // Returns i-th SingleTrg
    THPS_Singles_Trg *GetTopSingleTrg(int i);        // Returns i-th Top SingleTrg
    THPS_Singles_Trg *GetBotSingleTrg(int i);        // Returns i-th Top SingleTrg        

    virtual ~THPSTrigVect();
private:

    static map<int, string> fvtp_sect;
    int fROC_ID;

    int fn_HPS_Cl; // # of all clusters
    int fn_HPS_Top_Cl; // # of top clusters
    int fn_HPS_Bot_Cl; // # of bot clusters

    int fn_HPS_SingleTrg; // # of Single triggers
    int fn_HPS_Top_SingleTrg; // # of Single Top triggers
    int fn_HPS_Bot_SingleTrg; // # of Single Bot triggers

    int fn_HPS_PairTrg; // # of Pair triggers    

    vector<int>::iterator fit_crates;
    vector<ap_int < 32 >> ::iterator fit_words;

    vector<int> *fv_CrID;
    vector<ap_int < 32 >> *fv_words;

    vector<THPS_Cluster> fv_HPSCL;
    vector<int> fv_ind_HPS_TopCL; // Index of Top HPSClusters
    vector<int> fv_ind_HPS_BotCL; // Index of Bot HPSClusters

    vector<THPS_Singles_Trg> fv_HPS_SingleTrg;    // Vector of singleTrigger
    vector<int> fv_ind_HPS_Top_SingleTrg;         // vector of indexes of Top singles Triggers
    vector<int> fv_ind_HPS_Bot_SingleTrg;         // Vector of indexes of Bot singles Triggers
    
    
    bool hasCL;
    bool has_Singles;
    bool has_pair;


    static const unsigned short int type_switch2ndlvl = 12;
    static const unsigned short int type_2ndlvl_HPSCL = 02; // HPS cluster type 
    static const unsigned short int type_2ndlvl_HPSSIngleTrg = 03; // HPS SIngles Cluster
    static const unsigned short int type_2ndlvl_HPSPairTrg = 04; // HPS SIngles Cluster

    static const int f_Top_rocID; // = 11;
    static const int f_Bot_rocID; // = 12;    

    void ResetAll();

    void Check2ndlvltype(); // Since we ran out of types, for type "1100" next 4 bits will be used for checking types

    void ReadCrate(int, int, int); // Crate_ID, index of the iterator, number of words
    void ReadHPSCL(); // Reads HPS Cluster objects
    void ReadHPSSingleTrg(); // This method will read the HPSSingle_Trig (The definition in clonbanks.xml)
    void ReadHPSPairTrg(); // This method will read the HPSSingle_Trig (The definition in clonbanks.xml)

};

#endif /* THPSTRIGVECT_H */

