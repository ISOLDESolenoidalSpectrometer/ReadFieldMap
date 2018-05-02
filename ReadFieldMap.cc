#include "ReadFieldMap.hh"
#include "cxxopts.hh"

int main( int argc, char* argv[] ) {
	
	// Parameters
	string data_dir;
	string mode_dir = "normal";
	float ext_offset = -500.; // mm
	bool shifted_z = false;
	bool shifted_phi = false;
	
	///////////////////////////
	// Options parser
	try {
		
		cxxopts::Options options( "ReadFieldMap",
						"Read the field mapping data from ISS" );
		
		options.add_options()
		( "d,data_dir", "Path to data directory", cxxopts::value<string>(), "/path/to/data" )
		( "z,z_shift", "shifted in z" )
		( "p,phi_shift", "shifted in phi" )
		( "x,ext_offset", "Offset of the extension in mm", cxxopts::value<float>(), "offset" )
		( "h,help", "Print help" )
		;

		auto result = options.parse(argc, argv);
		
		// Do help
		if( result.count("help") ) {
			
			cout << options.help({""}) << endl;
			return 0;
			
		}
		
		// Get data directory
		if( result.count("d") ) {

			data_dir = result["d"].as<string>();

		}
		
		else {

			cout << options.help({""}) << endl;
			return 0;
			
		}
		
		// Check shifts
		if( result.count("z") ) {
			
			shifted_z = true;
			mode_dir = "shifted_z";
			
		}
		
		if( result.count("p") ) {
			
			shifted_z = true;
			shifted_phi = true;
			mode_dir = "shifted_z_phi";
			
		}
		
		if( result.count("x") ) {
			
			ext_offset = result["x"].as<float>();
			
		}
		
	} // End of options parser
	
	// Check for errors
	catch( const cxxopts::OptionException& e ) {
		
		cerr << "error parsing options: " << e.what() << endl;
		exit(1);
		
	}
	///////////////////////////
	
	// Create ROOT output file and tree
	string outname = mode_dir + ".root";
	TFile *rootout = new TFile( outname.c_str(), "RECREATE" );
	TTree *tree = new TTree( "Bmap", "ISS field mapping data" );
	CreateTree( tree );
	
	// Create text output file
	outname = mode_dir + ".txt";
	ofstream txtout;
	txtout.open( outname.c_str() );

	// Setup probe ID and positions
	SetProbes( ext_offset );
	
	// List of files
	string dir_ls = data_dir + "/" + mode_dir + "/*/*";
	vector<string> files = globVector( dir_ls );
	
	///////////////////
	// Process files //
	///////////////////
	
	int z_frame, phi_frame;
	unsigned int Nprobes;
	vector<int> probeIDs;
	
	txtout << "ProbeID\tT \tr \tphi \tx \ty \tz \tBx \tBy \tBz \tB" << endl;
	
	for( unsigned int i = 0; i < files.size(); i++ ) {
	
		GetZPhi( files.at(i), z_frame, phi_frame );
		z_real = GetRealZ( z_frame, shifted_z );
		
		Nprobes = CheckProbes( files.at(i), probeIDs );
		if( Nprobes == 0 ) return 1;
		
		for( unsigned int j = 0; j < Nprobes; j++ ) {
			
			r_real = GetRealR( probeIDs.at(j) );
			phi_real = GetRealPhi( phi_frame, shifted_phi );
			x_real = GetRealX( r_real, phi_real );
			y_real = GetRealY( r_real, phi_real );
			
			Bvec = GetBvector( files.at(i), probeIDs.at(j), temperature );
			RotateVector( phi_real, shifted_z, Bvec );
			
			Bx = Bvec.X();
			By = Bvec.Y();
			Bz = Bvec.Z();
			B = Bvec.Mag();
			
			// Text file
			txtout << probeIDs.at(j) << "\t";
			txtout << temperature << "\t";
			txtout << r_real << "\t";
			txtout << phi_real << "\t";
			txtout << x_real << "\t";
			txtout << y_real << "\t";
			txtout << z_real << "\t";
			txtout << Bx << "\t";
			txtout << By << "\t";
			txtout << Bz << "\t";
			txtout << B << "\t";
			txtout << endl;
			
			// Write to ROOT tree
			tree->Fill();
			
		}
		
		probeIDs.clear();
		vector<int>().swap( probeIDs );
		
	}
	
	// Close files
	tree->Write();
	rootout->Close();
	txtout.close();
	
	return 0;

}