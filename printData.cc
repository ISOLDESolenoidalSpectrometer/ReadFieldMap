void printData() {
	
	gErrorIgnoreLevel = kError;
	
	float x, y, z, r, phi;
	float B, Bx, By, Bz;
	
	TFile *f = new TFile( "ISS_Field_Map_total.root", "READ");
	TTree *tree = (TTree*)f->Get("Bmap");
	
	tree->SetBranchAddress( "x", &x );
	tree->SetBranchAddress( "y", &y );
	tree->SetBranchAddress( "z", &z );
	tree->SetBranchAddress( "r", &r );
	tree->SetBranchAddress( "phi", &phi );
	tree->SetBranchAddress( "B", &B );
	tree->SetBranchAddress( "Bx", &Bx );
	tree->SetBranchAddress( "By", &By );
	tree->SetBranchAddress( "Bz", &Bz );
	
	cout << "z\tr\tphi\tBz\n";

	int nentries = tree->GetEntries();
	for( int i = 0; i < nentries; i++ ) {
		
		tree->GetEntry(i);
		
		if( z > -100. && z < 100. )
			
			cout << z << "\t" << r << "\t" << phi << "\t" << Bz << endl;
		
	}
	
	return;
	
}