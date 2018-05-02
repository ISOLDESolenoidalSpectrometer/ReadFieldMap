void plot3D() {
	
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
	
	TH3F *h = new TH3F( "h", "ISS field map;z (mm);x (mm);y (mm)",
					   450, -1800, 1800, 200, -800, 800, 200, -800, 800 );
	
	int nentries = tree->GetEntries();
	
	for( int i = 0; i < nentries; i++ ) {
		
		tree->GetEntry(i);
		
		h->Fill( z, x, y, B );
		
	}
	
	return;
	
}