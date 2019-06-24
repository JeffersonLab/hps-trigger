/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   THPSTrigVect.cc
 * Author: rafopar
 * 
 * Created on June 21, 2019, 10:27 AM
 */

#include "THPSTrigVect.h"
#include <vector>

using namespace std;

const int THPSTrigVect::f_Top_rocID = 11;
const int THPSTrigVect::f_Bot_rocID = 12;

THPSTrigVect::THPSTrigVect() {
}

//THPSTrigVect::THPSTrigVect(const THPSTrigVect& orig) {
//}

THPSTrigVect::THPSTrigVect(vector<int> *v_crID, vector<int> *v_words) {

    SetVectors(v_crID, v_words);
}

void THPSTrigVect::SetVectors(vector<int> *v_crID, vector<int> *v_words) {

    fv_CrID = v_crID;
    fv_words = (vector<ap_int < 32 >> *)v_words;

    for (fit_words = fv_words->begin(); fit_words != fv_words->end();) {
        int n_words = fit_words->range(31, 0);
        int ind = std::distance(fv_words->begin(), fit_words);

        int crate_id = v_crID->at(ind);

        //  cout<<"cr_ID,  ind,   n_words:  "<<crate_id<<"    " <<ind<<"    "<<n_words<<endl;


        // The information isidentical in the TOP and Bottom VTPs, so wee need to read only one crate
        if (crate_id == f_Top_rocID) {
            ReadCrate(crate_id, ind, n_words);
        } else {
            fit_words = fv_words->end(); // Finish reading vtp words
        }
    }

    fn_HPS_Cl = fv_HPSCL.size();
    fn_HPS_Top_Cl = fv_ind_HPS_TopCL.size();
    fn_HPS_Bot_Cl = fv_ind_HPS_BotCL.size();

    fn_HPS_SingleTrg = fv_HPS_SingleTrg.size();
    fn_HPS_Top_SingleTrg = fv_ind_HPS_Top_SingleTrg.size();
    fn_HPS_Bot_SingleTrg = fv_ind_HPS_Bot_SingleTrg.size();

}

THPSTrigVect::~THPSTrigVect() {
}

void THPSTrigVect::ReadCrate(int CrateID, int ind, int nWords) {

    for (fit_words = fv_words->begin() + ind + 2; fit_words < fv_words->begin() + ind + nWords + 1; fit_words++) {

        ap_int<1> is_type_def = fit_words->range(31, 31);

        if (is_type_def) {

            ap_uint<4> data_type = fit_words->range(30, 27);

            switch (data_type) {

                case type_switch2ndlvl: Check2ndlvltype();
                    break;


            }

        }
    }
}

void THPSTrigVect::ReadHPSCL() {

    hasCL = false;


    //ap_int<n_max_DC_segments> dc_segm_mask = fit_words->range(22, 17);

    THPS_Cluster cur_cl;

    cur_cl.E = fit_words->range(22, 10);

    // === These are signed number, therefore using ap_int for easier conversion
    ap_int<4> tmp_cly = fit_words->range(9, 6);

    cur_cl.Y = tmp_cly.to_int();
    ap_int<6> tmp_clx = fit_words->range(5, 0);
    cur_cl.X = tmp_clx.to_int();

    // ==== The vtp reports cl_x from -22 to 23, to make it consistent with 
    // ==== other definitions, we subtract 1, when x<= 0
    if (cur_cl.X <= 0) {
        cur_cl.X = cur_cl.X - 1;
    }


    // Go to the next word
    fit_words = std::next(fit_words, 1);

    cur_cl.N = fit_words->range(13, 10);
    cur_cl.T = fit_words->range(9, 0);

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

void THPSTrigVect::ReadHPSSingleTrg() {
    has_Singles = true;

    THPS_Singles_Trg cur_trg;

    cur_trg.instance = fit_words->range(22, 20);
    cur_trg.top_bot = fit_words->range(19, 19);
    cur_trg.H_L1L2X_GEOM_PASS = fit_words->range(18, 18);
    cur_trg.H_L1L2_GEOM_PASS = fit_words->range(17, 17);
    cur_trg.H_L2_PASS = fit_words->range(16, 16);
    cur_trg.H_L1_PASS = fit_words->range(15, 15);
    cur_trg.PDE_PASS = fit_words->range(14, 14);
    cur_trg.MINX_PASS = fit_words->range(13, 13);
    cur_trg.NMIN_PASS = fit_words->range(12, 12);
    cur_trg.EMAX_PASS = fit_words->range(11, 11);
    cur_trg.EMIN_PASS = fit_words->range(10, 10);
    cur_trg.T = fit_words->range(9, 0);

    fv_HPS_SingleTrg.push_back(cur_trg);

    if (cur_trg.top_bot > 0) {
        fv_ind_HPS_Top_SingleTrg.push_back(fv_HPS_SingleTrg.size() - 1); // Trigger is in the Top half
    } else {
        fv_ind_HPS_Bot_SingleTrg.push_back(fv_HPS_SingleTrg.size() - 1); // Trigger is in the Bot half        
    }


}

void THPSTrigVect::Check2ndlvltype() {

    ap_uint<4> type_2ndlvl = fit_words->range(26, 23);

    switch (type_2ndlvl) {

        case type_2ndlvl_HPSCL: ReadHPSCL();
            break;
        case type_2ndlvl_HPSSIngleTrg: ReadHPSSingleTrg();

    }


}

THPS_Cluster * THPSTrigVect::GetCLuster(int ind) {

    if (ind >= fn_HPS_Cl || ind < 0) {
        cout << "Requested wrong cluster index=" << ind << endl;
        cout << "# of clusters is " << fn_HPS_Cl << "    So he index should be between 0  and" << (fn_HPS_Cl - 1) << endl;
    }

    return &fv_HPSCL.at(ind);
}

THPS_Cluster * THPSTrigVect::GetTopCLuster(int ind) {

    if (ind >= fn_HPS_Top_Cl || ind < 0) {
        cout << "Requested wrong cluster index=" << ind << endl;
        cout << "# of Top clusters is " << fn_HPS_Top_Cl << "    So he index should be between 0  and" << (fn_HPS_Top_Cl - 1) << endl;
    }

    return &fv_HPSCL.at(fv_ind_HPS_TopCL.at(ind));
}

THPS_Cluster * THPSTrigVect::GetBotCLuster(int ind) {

    if (ind >= fn_HPS_Bot_Cl || ind < 0) {
        cout << "Requested wrong cluster index=" << ind << endl;
        cout << "# of Bot clusters is " << fn_HPS_Bot_Cl << "    So he index should be between 0  and" << (fn_HPS_Bot_Cl - 1) << endl;
    }

    return &fv_HPSCL.at(fv_ind_HPS_BotCL.at(ind));
}

THPS_Singles_Trg *THPSTrigVect::GetSingleTrg(int ind) {
    if (ind >= fn_HPS_SingleTrg || ind < 0) {
        cout << "Requested wrong SingleTrg index=" << ind << endl;
        cout << "# of SingleTrg objects is " << fn_HPS_SingleTrg << "    So he index should be between 0  and" << (fn_HPS_SingleTrg - 1) << endl;
    }

    return &fv_HPS_SingleTrg.at(ind);
}

THPS_Singles_Trg *THPSTrigVect::GetTopSingleTrg(int ind) {
    if (ind >= fn_HPS_Top_SingleTrg || ind < 0) {
        cout << "Requested wrong Top SingleTrg index=" << ind << endl;
        cout << "# of Top SingleTrg objects is " << fn_HPS_Top_SingleTrg << "    So he index should be between 0  and" << (fn_HPS_Top_SingleTrg - 1) << endl;
    }

    return &fv_HPS_SingleTrg.at(fv_ind_HPS_Top_SingleTrg.at(ind));
}

THPS_Singles_Trg *THPSTrigVect::GetBotSingleTrg(int ind) {
    if (ind >= fn_HPS_Bot_SingleTrg || ind < 0) {
        cout << "Requested wrong Bot SingleTrg index=" << ind << endl;
        cout << "# of Bot SingleTrg objects is " << fn_HPS_Bot_SingleTrg << "    So he index should be between 0  and" << (fn_HPS_Bot_SingleTrg - 1) << endl;
    }

    return &fv_HPS_SingleTrg.at(fv_ind_HPS_Bot_SingleTrg.at(ind));
}



void THPSTrigVect::ResetAll() {

    hasCL = false;
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

    fn_HPS_Cl = 0;
    fn_HPS_Top_Cl = 0;
    fn_HPS_Bot_Cl = 0;

    fn_HPS_SingleTrg = 0;
    fn_HPS_Top_SingleTrg = 0;
    fn_HPS_Bot_SingleTrg = 0;

}