/* 
 * File:   THPSTrig.h
 * Author: rafopar
 *
 * Created on June 9, 2019, 9:14 PM
 */

#ifndef THPSTRIG_H
#define THPSTRIG_H

#include <evioUtil.hxx>
#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>
#include <vector>

// ===== The HPS Calorimeter cluster object

typedef struct {
    int E;
    int Y;
    int X;
    int N;
    int T;
} THPS_Cluster;

typedef struct{
    int instance; // The instance
    bool top_bot; // Top or bottom
    bool H_L1L2X_GEOM_PASS;  // 
    bool H_L1L2_GEOM_PASS;   // Coincidence between Hodo L1 and L2
    bool H_L2_PASS;  // Hodo L2 pass
    bool H_L1_PASS;  // Hodo L1 pass
    bool PDE_PASS;   // Position dependent Energy cut
    bool MINX_PASS;  // if the cluster X is above the min-X
    bool NMIN_PASS;  // # of hits in the cluster
    bool EMAX_PASS;  // The cluster energy should be below of the EMAX
    bool EMIN_PASS;  // The cluster energy should be above of the EMAX
    int  T;          // Time of the trigger decision
}THPS_Singles_Trg;


typedef struct{
    int instance;           // The instance
    int reserved;           // has no meaning yet
    bool coplan_pass;       // passed the coplanarity cut
    bool Edslope_pass;      // passed the energy distance slope
    bool Ediff_pass;        // passed the enegy difference cut
    bool summ_pass;         // passed the Energy sum cut
    int T;                  // TIme of the trigger decision
}THPS_PairTrig;

class THPSTrig {
public:
    THPSTrig();
    THPSTrig(evio::evioDOMNode*, int);
    //THPSTrig(const THPSTrig& orig);
    virtual ~THPSTrig();

    void SetevioDOMENodeSect(evio::evioDOMNode*, int);

    int GetNClust() {return fn_HPS_Cl;}; // Returns number of clusters
    int GetNTopClust() {return fn_HPS_Top_Cl;}; // Returns number of Top clusters
    int GetNBotClust() {return fn_HPS_Bot_Cl;}; // Returns number of Top clusters
    
    int GetNSingleTrg() {return fn_HPS_SingleTrg;}; // Return # of single Trg objects
    int GetNTopSingleTrg() {return fn_HPS_Top_SingleTrg;}; // Return # of Top single Trg objects
    int GetNBotSingleTrg() {return fn_HPS_Bot_SingleTrg;}; // Return # of Bot single Trg objects
        
    int GetNPairTrg() {return fn_HPS_PairTrg;}; // Return # of Pair Trg objects
    
    THPS_Cluster *GetCLuster(int i);              // Get i-th cluster
    THPS_Cluster *GetTopCLuster(int i);           // Get i-th Top cluster
    THPS_Cluster *GetBotCLuster(int i);           // Get i-th Bot cluster

    THPS_Singles_Trg *GetSingleTrg(int i);        // Returns i-th SingleTrg
    THPS_Singles_Trg *GetTopSingleTrg(int i);        // Returns i-th Top SingleTrg
    THPS_Singles_Trg *GetBotSingleTrg(int i);        // Returns i-th Top SingleTrg

    THPS_PairTrig *GetPairTrg(int i);        // Returns i-th Pair trigger

private:

    vector<THPS_Cluster> fv_HPSCL;
    vector<int> fv_ind_HPS_TopCL; // Index of Top HPSClusters
    vector<int> fv_ind_HPS_BotCL; // Index of Bot HPSClusters

    vector<THPS_Singles_Trg> fv_HPS_SingleTrg;    // Vector of singleTrigger
    vector<int> fv_ind_HPS_Top_SingleTrg;         // vector of indexes of Top singles Triggers
    vector<int> fv_ind_HPS_Bot_SingleTrg;         // Vector of indexes of Bot singles Triggers

    vector<THPS_PairTrig> fv_HPS_PairTrg;         // Vector of PairTrigger
    
    bool has_HPSCl;
    bool has_Singles;
    bool has_pair;

    std::vector<ap_int<32> >::iterator fit_data;
    evio::evioDOMNode* fhead_node; //pointer to the head node of VTP bank

    static map<int, string> vtp_sect;
    int fROC_ID;

    int fn_HPS_Cl;          // # of all clusters
    int fn_HPS_Top_Cl;      // # of top clusters
    int fn_HPS_Bot_Cl;      // # of bot clusters
    
    int fn_HPS_SingleTrg;       // # of Single triggers
    int fn_HPS_Top_SingleTrg;   // # of Single Top triggers
    int fn_HPS_Bot_SingleTrg;   // # of Single Bot triggers

    int fn_HPS_PairTrg;       // # of Pair triggers
       
    static const int f_Top_rocID; // = 11;
    static const int f_Bot_rocID; // = 12;

    // ================= The 1st four bits are occupied and the rest of trigger objects will be defined using
    // ================= next four bits =========================================
    static const unsigned short int type_switch2ndlvl = 12;
    static const unsigned short int type_2ndlvl_HPSCL = 02;             // HPS cluster type 
    static const unsigned short int type_2ndlvl_HPSSIngleTrg = 03;      // HPS SIngles Cluster
    static const unsigned short int type_2ndlvl_HPSPairTrg = 04;        // HPS SIngles Cluster

    void ResetAll();
    
    void ReadHPSCL(); // This method will read HPSClusters
    void ReadHPSSingleTrg(); // This method will read the HPSSingle_Trig (The definition in clonbanks.xml)
    void ReadHPSPairTrg(); // This method will read the HPSSingle_Trig (The definition in clonbanks.xml)
    void Check2ndlvltype();

};

#endif /* THPSTRIG_H */

