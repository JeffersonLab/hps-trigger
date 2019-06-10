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
    
    THPS_Cluster *GetCLuster(int i);              // Get i-th cluster
    THPS_Cluster *GetTopCLuster(int i);           // Get i-th Top cluster
    THPS_Cluster *GetBotCLuster(int i);           // Get i-th Bot cluster


private:

    vector<THPS_Cluster> fv_HPSCL;
    vector<int> fv_ind_HPS_TopCL; // Index of Top HPSClusters
    vector<int> fv_ind_HPS_BotCL; // Index of Bot HPSClusters


    bool has_HPSCl;

    std::vector<ap_int<32> >::iterator fit_data;
    evio::evioDOMNode* fhead_node; //pointer to the head node of VTP bank

    static map<int, string> vtp_sect;
    int fROC_ID;

    int fn_HPS_Cl;          // # of all clusters
    int fn_HPS_Top_Cl;      // # of top clusters
    int fn_HPS_Bot_Cl;      // # of bot clusters
       
    static const int f_Top_rocID = 60011;
    static const int f_Bot_rocID = 60011;

    // ================= The 1st four bits are occupied and the rest of trigger objects will be defined using 
    // ================= next four bits =========================================
    static const unsigned short int type_switch2ndlvl = 12;
    static const unsigned short int type_2ndlvl_HPSCL = 02; // HPS cluster type 

    void ResetAll();
    
    void ReadHPSCL(); // This method will read HPSClusters
    void Check2ndlvltype();

};

#endif /* THPSTRIG_H */

