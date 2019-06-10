#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>

#include <iostream>

using namespace std;

int main( int argc, char **argv ){

  TChain *ch1 = new TChain();

  if (argc < 2) {
    cout << "Please Specify input File name(s) " << endl;
    cout << "Exiting" << endl;
    exit(1);
  }

  string inp_file_patttern = argv[1];
  cout << inp_file_patttern << endl;
  ch1->Add(inp_file_patttern.c_str());

  TObjArray *fname_list = ch1->GetListOfFiles();

  int n_files = fname_list->GetEntries();
  cout << "Number of Files is " << n_files << endl;
  cout << "Number of Tot. events to analyze1 " << n_events_to_Analyze << endl;

  int run = 9152;
  
  TFile *file_out = new TFile(Form("Data/HPS_VTP_out_%d.root", run), "Recreate");


  try{


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

	int crate_id_trig = -1;
		
	TTrigBit tr_bit;
	

	for (std::list<evioDOMNode*>::iterator it = nodelist1.get()->begin(); it != nodelist1.get()->end(); it++) {
		  
		  
	  if( (*it)->tag > 60000 && (*it)->tag < 62000 )
	    {
	      crate_id_trig = (*it)->tag;
	    }
		  

	  
	  
	}
		

	
      }
      
    }

    
  } catch (evioException e) {
        cerr << endl << e.toString() << endl;
        exit(EXIT_FAILURE);
    }

}


