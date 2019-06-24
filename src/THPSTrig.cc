/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   THPSTrig.cc
 * Author: rafopar
 * 
 * Created on June 9, 2019, 9:14 PM
 */

#include "THPSTrig.h"

using namespace std;

const int THPSTrig::f_Top_rocID = 11;
const int THPSTrig::f_Bot_rocID = 12;

map<int, string> THPSTrig::vtp_sect = {
    {f_Top_rocID, "Top"},
    {f_Bot_rocID, "Bot"}
};

THPSTrig::THPSTrig() {
}

THPSTrig::THPSTrig(evio::evioDOMNode* it, int a_rocID_tag) {
    SetevioDOMENodeSect(it, a_rocID_tag);
}

//THPSTrig::THPSTrig(const THPSTrig& orig) {
//}

void THPSTrig::SetevioDOMENodeSect(evio::evioDOMNode* it, int a_rocID_tag) {

    ResetAll();
    fhead_node = it;

    // ===== Make sure correct rocID is provided
    if (!(a_rocID_tag == f_Top_rocID || a_rocID_tag == f_Bot_rocID)) {
        printf("Wrong Bank Tag is provided in %s Exiting \n", __func__);
        cout << "rocID is " << a_rocID_tag << endl;
        exit(1);
    }

    fROC_ID = a_rocID_tag;


    vector<ap_int<32> > *data_values = (vector<ap_int < 32 >> *)it->getVector<uint32_t>();

    for (fit_data = data_values->begin(); fit_data != data_values->end(); fit_data++) {

        ap_int<1> is_type_def = fit_data->range(31, 31);

        if (is_type_def) {
            ap_uint<4> data_type = fit_data->range(30, 27);

            switch (data_type) {

                case type_switch2ndlvl: Check2ndlvltype();
                    break;

            }
        }


        fn_HPS_Cl = fv_HPSCL.size();
        fn_HPS_Top_Cl = fv_ind_HPS_TopCL.size();
        fn_HPS_Bot_Cl = fv_ind_HPS_BotCL.size();

        fn_HPS_SingleTrg = fv_HPS_SingleTrg.size();
        fn_HPS_Top_SingleTrg = fv_ind_HPS_Top_SingleTrg.size();
        fn_HPS_Bot_SingleTrg = fv_ind_HPS_Bot_SingleTrg.size();
        
        fn_HPS_PairTrg = fv_HPS_PairTrg.size();
    }



}

void THPSTrig::Check2ndlvltype() {

    ap_uint<4> type_2ndlvl = fit_data->range(26, 23);

    switch (type_2ndlvl) {

        case type_2ndlvl_HPSCL: ReadHPSCL();
            break;
        case type_2ndlvl_HPSSIngleTrg: ReadHPSSingleTrg();

    }
}

void THPSTrig::ReadHPSCL() {
    has_HPSCl = true;

    THPS_Cluster cur_cl;

    cur_cl.E = fit_data->range(22, 10);
    // === These are signed number, therefore using ap_int for easier conversion
    ap_int<4> tmp_cly = fit_data->range(9, 6);

    cur_cl.Y = tmp_cly.to_int();
    ap_int<6> tmp_clx = fit_data->range(5, 0);
    cur_cl.X = tmp_clx.to_int();

    // ==== The vtp reports cl_x from -22 to 23, to make it consistent with 
    // ==== other definitions, we subtract 1, when x<= 0
    if (cur_cl.X <= 0) {
        cur_cl.X = cur_cl.X - 1;
    }

    // ===== Goint to the next word =====
    fit_data = std::next(fit_data, 1);
    cur_cl.N = fit_data->range(13, 10);
    cur_cl.T = fit_data->range(9, 0);

    fv_HPSCL.push_back(cur_cl);

    if (cur_cl.Y > 0) {
        fv_ind_HPS_TopCL.push_back(fv_HPSCL.size() - 1);
    } else if (cur_cl.Y < 0) {
        fv_ind_HPS_BotCL.push_back(fv_HPSCL.size() - 1);
    } else {
        cout << "The HPS cluster Y is 0. This should not happen" << endl;
        cout << "Exiting " << endl;
        exit(1);
    }
}

void THPSTrig::ReadHPSSingleTrg() {

    has_Singles = true;

    THPS_Singles_Trg cur_trg;
    cur_trg.instance = fit_data->range(22, 20);
    cur_trg.top_bot = fit_data->range(19, 19);
    cur_trg.H_L1L2X_GEOM_PASS = fit_data->range(18, 18);
    cur_trg.H_L1L2_GEOM_PASS = fit_data->range(17, 17);
    cur_trg.H_L2_PASS = fit_data->range(16, 16);
    cur_trg.H_L1_PASS = fit_data->range(15, 15);
    cur_trg.PDE_PASS = fit_data->range(14, 14);
    cur_trg.MINX_PASS = fit_data->range(13, 13);
    cur_trg.NMIN_PASS = fit_data->range(12, 12);
    cur_trg.EMAX_PASS = fit_data->range(11, 11);
    cur_trg.EMIN_PASS = fit_data->range(10, 10);
    cur_trg.T = fit_data->range(9, 0);

    fv_HPS_SingleTrg.push_back(cur_trg);

    if (cur_trg.top_bot > 0) {
        fv_ind_HPS_Top_SingleTrg.push_back(fv_HPS_SingleTrg.size() - 1); // Trigger is in the Top half
    } else {
        fv_ind_HPS_Bot_SingleTrg.push_back(fv_HPS_SingleTrg.size() - 1); // Trigger is in the Bot half        
    }
}

void THPSTrig::ReadHPSPairTrg() {
    has_pair = true;

    THPS_PairTrig cur_trg;

    cur_trg.instance = fit_data->range(22, 20);
    cur_trg.reserved = fit_data->range(19, 14);
    cur_trg.coplan_pass = fit_data->range(13, 13);
    cur_trg.Edslope_pass = fit_data->range(12, 12);
    cur_trg.Ediff_pass = fit_data->range(11, 11);
    cur_trg.summ_pass = fit_data->range(10, 10);
    cur_trg.T = fit_data->range(9, 0);

    fv_HPS_PairTrg.push_back(cur_trg);
}

THPS_Cluster * THPSTrig::GetCLuster(int ind) {

    if (ind >= fn_HPS_Cl || ind < 0) {
        cout << "Requested wrong cluster index=" << ind << endl;
        cout << "# of clusters is " << fn_HPS_Cl << "    So he index should be between 0  and" << (fn_HPS_Cl - 1) << endl;
    }

    return &fv_HPSCL.at(ind);
}

THPS_Cluster * THPSTrig::GetTopCLuster(int ind) {

    if (ind >= fn_HPS_Top_Cl || ind < 0) {
        cout << "Requested wrong cluster index=" << ind << endl;
        cout << "# of Top clusters is " << fn_HPS_Top_Cl << "    So he index should be between 0  and" << (fn_HPS_Top_Cl - 1) << endl;
    }

    return &fv_HPSCL.at(fv_ind_HPS_TopCL.at(ind));
}

THPS_Cluster * THPSTrig::GetBotCLuster(int ind) {

    if (ind >= fn_HPS_Bot_Cl || ind < 0) {
        cout << "Requested wrong cluster index=" << ind << endl;
        cout << "# of Bot clusters is " << fn_HPS_Bot_Cl << "    So he index should be between 0  and" << (fn_HPS_Bot_Cl - 1) << endl;
    }

    return &fv_HPSCL.at(fv_ind_HPS_BotCL.at(ind));
}

THPS_Singles_Trg *THPSTrig::GetSingleTrg(int ind) {
    if (ind >= fn_HPS_SingleTrg || ind < 0) {
        cout << "Requested wrong SingleTrg index=" << ind << endl;
        cout << "# of SingleTrg objects is " << fn_HPS_SingleTrg << "    So he index should be between 0  and" << (fn_HPS_SingleTrg - 1) << endl;
    }

    return &fv_HPS_SingleTrg.at(ind);
}

THPS_Singles_Trg *THPSTrig::GetTopSingleTrg(int ind) {
    if (ind >= fn_HPS_Top_SingleTrg || ind < 0) {
        cout << "Requested wrong Top SingleTrg index=" << ind << endl;
        cout << "# of Top SingleTrg objects is " << fn_HPS_Top_SingleTrg << "    So he index should be between 0  and" << (fn_HPS_Top_SingleTrg - 1) << endl;
    }

    return &fv_HPS_SingleTrg.at(fv_ind_HPS_Top_SingleTrg.at(ind));
}

THPS_Singles_Trg *THPSTrig::GetBotSingleTrg(int ind) {
    if (ind >= fn_HPS_Bot_SingleTrg || ind < 0) {
        cout << "Requested wrong Bot SingleTrg index=" << ind << endl;
        cout << "# of Bot SingleTrg objects is " << fn_HPS_Bot_SingleTrg << "    So he index should be between 0  and" << (fn_HPS_Bot_SingleTrg - 1) << endl;
    }

    return &fv_HPS_SingleTrg.at(fv_ind_HPS_Bot_SingleTrg.at(ind));
}

THPS_PairTrig* THPSTrig::GetPairTrg(int ind) {
       if (ind >= fn_HPS_PairTrg || ind < 0) {
        cout << "Requested wrong Bot PairTrg index=" << ind << endl;
        cout << "# of PairTrig is " << fn_HPS_PairTrg << "    So he index should be between 0  and" << (fn_HPS_PairTrg - 1) << endl;
    }
 
       
}


THPSTrig::~THPSTrig() {
}

void THPSTrig::ResetAll() {

    has_HPSCl = false;
    has_Singles = false;

    fv_HPSCL.clear();
    fv_HPSCL.shrink_to_fit();

    fv_ind_HPS_TopCL.clear();
    fv_ind_HPS_TopCL.shrink_to_fit();

    fv_ind_HPS_BotCL.clear();
    fv_ind_HPS_BotCL.shrink_to_fit();

    fv_HPS_SingleTrg.clear();
    fv_HPS_SingleTrg.shrink_to_fit();

    fv_ind_HPS_Top_SingleTrg.clear();
    fv_ind_HPS_Top_SingleTrg.shrink_to_fit();

    fv_ind_HPS_Bot_SingleTrg.clear();
    fv_ind_HPS_Bot_SingleTrg.shrink_to_fit();

    fv_HPS_PairTrg.clear();
    fv_HPS_PairTrg.shrink_to_fit();

    fn_HPS_Cl = 0;
    fn_HPS_Top_Cl = 0;
    fn_HPS_Bot_Cl = 0;

    fn_HPS_SingleTrg = 0;
    fn_HPS_Top_SingleTrg = 0;
    fn_HPS_Bot_SingleTrg = 0;

    fn_HPS_PairTrg = 0;
}