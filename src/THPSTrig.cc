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
    }



}

void THPSTrig::Check2ndlvltype() {

    ap_uint<4> type_2ndlvl = fit_data->range(26, 23);

    switch (type_2ndlvl) {

        case type_2ndlvl_HPSCL: ReadHPSCL();
            break;

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

THPSTrig::~THPSTrig() {
}

void THPSTrig::ResetAll() {

    has_HPSCl = false;

    fv_HPSCL.clear();
    fv_HPSCL.shrink_to_fit();

    fv_ind_HPS_TopCL.clear();
    fv_ind_HPS_TopCL.shrink_to_fit();

    fv_ind_HPS_BotCL.clear();
    fv_ind_HPS_BotCL.shrink_to_fit();


    fn_HPS_Cl = 0;
    fn_HPS_Top_Cl = 0;
    fn_HPS_Bot_Cl = 0;


}