#include <iostream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"

#include "DataFormat/IOManager.h"
#include "DataFormat/EventImage2D.h"

int main( int nargs, char** argv ) {

  std::cout << "Check LArFlow image 3D-cycle consistency" << std::endl;

  larcv::IOManager io( larcv::IOManager::kREAD );
  io.add_in_file( "larcv.root" );
  io.initialize();

  size_t nentries = io.get_n_entries();

  enum { kU2V=0, kU2Y, kV2U, kV2Y, kY2U, kY2V };

  TFile* out = new TFile("output_check_flow.root", "recreate" );
  
  // per-event tree
  TTree* evtree  = new TTree("evflow", "Event-level label quality metrics" );
  
  // per-pixel tree
  TTree* pixtree = new TTree("pixflow", "Pixel-level label quality metrics" );
  int   dy1; //< difference from cylce: path1
  int   dy2; //< difference from cylce: path2
  int  ycol; //< start point
  float adc; //< pix value
  pixtree->Branch("ycol", &ycol, "ycol/I" );
  pixtree->Branch("dy1",  &dy1,  "dy1/I" );
  pixtree->Branch("dy2",  &dy2,  "dy2/I" );
  pixtree->Branch("adc",  &adc,  "adc/F" );


  for (size_t ientry=0; ientry<nentries; ientry++) {
    io.read_entry(ientry);
    std::cout << "====================" << std::endl;
    std::cout << "Entry " << ientry << std::endl;
    std::cout << "====================" << std::endl;

    
    larcv::EventImage2D* ev_larflow = (larcv::EventImage2D*)io.get_data( larcv::kProductImage2D, "larflow" );
    larcv::EventImage2D* ev_wire    = (larcv::EventImage2D*)io.get_data( larcv::kProductImage2D, "wire" );
    
    const std::vector<larcv::Image2D>& larflow_v = ev_larflow->Image2DArray();
    const std::vector<larcv::Image2D>& img_v     = ev_wire->Image2DArray();    

    std::cout << "Number of images in larflow_v: " << larflow_v.size() << std::endl;

    // check flow:
    // Y->U->V->Y
    // Y->V->U->Y
    
    int flowpaths[2][3] = { {kY2U, kU2V, kV2Y},
			    {kY2V, kV2U, kU2Y} };

    // quality metrics
    int goodpaths[2] = {0}; //< ideal: equal to above_thresh_ypixels_tot
    int badpaths[2]  = {0}; //< ideal: zero
    int above_thresh_ypixels_tot = 0;     // ideal: is what it is
    int above_thresh_ypixels_nolabel = 0; // ideal: zero
    int below_thresh_ypixels_wlabel  = 0; // ideal: zero


    const larcv::Image2D& imgp2    = img_v.at(2);
    const larcv::ImageMeta& metap2 = imgp2.meta();

    std::stringstream hname;
    hname << "hdy_" << ientry;
    TH2D hdy(hname.str().c_str(), "", metap2.cols(), 0, metap2.cols(), metap2.rows(),0,metap2.rows());    
    

    for (size_t irow=0; irow<metap2.rows(); irow++) {
      for (size_t icol=0; icol<metap2.cols(); icol++) {

	adc = imgp2.pixel(irow,icol);
	if ( adc<10.0 )
	  continue; // below threshold
	
	// follow the flow paths
	for (int ipath=0; ipath<2; ipath++) {

	  int path[3] = {0,0,0};
	  int nextcol = icol;
	  for (int ip=0; ip<3; ip++) {

	    if ( nextcol<0 || nextcol >=metap2.cols() ) {
	      nextcol = -1;
	      break;
	    }
	    
	    const larcv::Image2D& flowimg = larflow_v.at( flowpaths[ipath][ip] );
	    nextcol = (int)flowimg.pixel( irow, nextcol );
	    
	  }

	  // did we come back?
	  if ( nextcol>=0 ) {
	    if ( ipath==0 )
	      dy1 = nextcol-icol; 
	    else if ( ipath==1)
	      dy2 = nextcol-icol;
	  }
	  else {
	    // set to sentinal value
	    if (ipath==0)
	      dy1 = 4000;
	    else
	      dy2 = 4000;
	  }
	}//end of loops over 2 paths

	hdy.SetBinContent( icol+1, irow+1, fabs(dy1)+1.0 );

	pixtree->Fill();
      }//end of col loop
    }//end of row loop

    hdy.Write();
  }
  
  out->Write();
  
  return 0;
}
