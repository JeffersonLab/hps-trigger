#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TChain.h>

#include <evioUtil.hxx>
#include <evioFileChannel.hxx>

#include <iostream>

#include "THPSTrig.h"

using namespace evio;
using namespace std;

int main(int argc, char **argv) {


    const int vtp_tag = 57634;

    TChain *ch1 = new TChain();

    if (argc < 2) {
        cout << "Please Specify input File name(s) " << endl;
        cout << "Exiting" << endl;
        exit(1);
    }

    int n_events_to_Analyze = 100000; // Default value

    if (argc >= 3) {
        n_events_to_Analyze = atoi(argv[2]);
    }


    string inp_file_patttern = argv[1];
    cout << inp_file_patttern << endl;
    ch1->Add(inp_file_patttern.c_str());

    TObjArray *fname_list = ch1->GetListOfFiles();

    int n_files = fname_list->GetEntries();
    cout << "Number of Files is " << n_files << endl;
    cout << "Number of Tot. events to analyze1 " << n_events_to_Analyze << endl;

    int run = 9152;

    TFile *file_out = new TFile(Form("../Data/HPS_VTP_out_%d.root", run), "Recreate");
    
    TH1D *h_N_clust1 = new TH1D("h_N_clust1", "", 15, -0.5, 14.5);
    TH2D *h_clust_YXc1 = new TH2D("h_clust_YXc1", "", 51, -25.5, 25.5, 11, -5.5, 5.5);

    TH1D *h_cl_E1 = new TH1D("h_cl_E1", "", 200, 0., 4500);
    TH1D *h_cl_T1 = new TH1D("h_cl_T1", "", 101, -0.5, 100.5);
    TH1D *h_cl_Nhit1 = new TH1D("h_cl_Nhit1", "", 11, -0.5, 10.5);
    
    TH2D *h_N_Sing_Top_Bot1 = new TH2D("h_N_Sing_Top_Bot1", "", 26, -0.5, 25.5, 26, -0.5, 25.5);
    
    TH2D *h_N_Sing_triggers1 = new TH2D("h_N_Sing_triggers1", "", 6, -2.5, 3.5, 31, -0.5, 30.5);

    TH1D *h_N_pair_trg1 = new TH1D("h_N_pair_trg1", "", 21, -0.5, 2.5);
    
    try {

        unsigned int i_ev = 0;

        for (int ifile = 0; ifile < n_files; ifile++) {
            
            evioFileChannel chan(fname_list->At(ifile)->GetTitle(), "r");
            cout << "Running the file " << fname_list->At(ifile)->GetTitle() << endl;
            chan.open();

            while (chan.read()) {

                i_ev = i_ev + 1;

                evioDOMTree tree(chan);

                evioDOMNodeListP nodelist1 = tree.getNodeList();

                // ====== LET'S CHECK THE TRIGGER BIT SO IT WILL RUN OVER THE EVENT TWISE =====

                int crate_id_tag = -1;

                THPSTrig trig;
                for (std::list<evioDOMNode*>::iterator it = nodelist1.get()->begin(); it != nodelist1.get()->end(); it++) {


                    //cout<<"Cur tag is "<<(*it)->tag<<endl;
                    
                    if ((*it)->tag > 10 && (*it)->tag < 12) {
                        crate_id_tag = (*it)->tag;
                    }


                    if( (*it)->tag == vtp_tag ){
                        
                        trig.SetevioDOMENodeSect(*it, crate_id_tag );
                        
                        
                        
                        // ================= Clusters ================
                        int n_clust = trig.GetNClust();
                        
                        h_N_clust1->Fill(n_clust);
                        
                        for( int i_cl = 0; i_cl < n_clust; i_cl++ ){
                            
                            THPS_Cluster *vtp_cl = trig.GetCLuster(i_cl);
                            
                            h_clust_YXc1->Fill(vtp_cl->X, vtp_cl->Y);
                            //cout<<vtp_cl->X<<"    "<<vtp_cl->Y<<endl;
                            
                            h_cl_E1->Fill(vtp_cl->E);
                            h_cl_T1->Fill(vtp_cl->T);
                            h_cl_Nhit1->Fill(vtp_cl->N);
                        }
                        
                        // ================= SingleTrg ================
                        
                        int n_singleTrg = trig.GetNSingleTrg();
                        
                        int n_topSingleTrg = trig.GetNTopSingleTrg();
                        int n_botSingleTrg = trig.GetNBotSingleTrg();
                        
                        if( n_singleTrg !=  n_topSingleTrg + n_botSingleTrg ){
                            cout<<" = = = = = Oho something is wrong, N_Top + N_bot shoule be equal to N_tot"<<endl;
                            cout<<"N_Top = "<<n_topSingleTrg<<"     N_Bot = "<<n_botSingleTrg<<"     N_Tot = "<<n_singleTrg<<endl;
                            exit(1);
                        }
                        
                        
                        h_N_Sing_Top_Bot1->Fill(n_botSingleTrg, n_topSingleTrg);
                        
                        for( int iS = 0; iS < n_singleTrg; iS++ ){
                            
                            bool top_bot = trig.GetSingleTrg(iS)->top_bot;
                            bool L1L2 = trig.GetSingleTrg(iS)->H_L1L2_GEOM_PASS;
                            
                            h_N_Sing_triggers1->Fill( 4.*(top_bot - 0.5) + L1L2, n_singleTrg );
                            
                        }
                        
                        
                        // ================= PairTrg ================
                        
                        int n_pair_trg = trig.GetNPairTrg();
                        
                        h_N_pair_trg1->Fill(n_pair_trg);
                        
                    }

                }



            }

        }


    } catch (evioException e) {
        cerr << endl << e.toString() << endl;
        exit(EXIT_FAILURE);
    }
    
    gDirectory->Write();
    file_out->Close();        

}


