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
	
	// Example 1
	TH3F *model = new TH3F( "model", "ISS field map;z (mm);x (mm);y (mm)",
					   450, -1800, 1800, 200, -800, 800, 200, -800, 800 );
	
	// Example 2
	TH2F *surf = new TH2F( "surf", "ISS field map;r (mm);phi (deg);B (T)",
					   200, -800, 800, 360, 0, 360 );
	
	// Example 3
	TGraph *gated = new TGraph();
	gated->SetTitle( "ISS field map;phi (deg);B (T)" );
	
	int nentries = tree->GetEntries();
	
	for( int i = 0; i < nentries; i++ ) {
		
		tree->GetEntry(i);
		
		// Example 1
		model->Fill( z, x, y, B );
		
		// Example 2
		if( z > -25. && z < 25. )
			surf->Fill( r, phi, B );
		
		// Example 3
		if( z > -25. && z < 25. && r < -400 )
			gated->SetPoint( i, phi, B );
		
	}
	
	TCanvas *c1 = new TCanvas();
	TCanvas *c2 = new TCanvas();
	TCanvas *c3 = new TCanvas();
	
	// Example 1
	c1->cd();
	model->Draw();
	
	// Example 2
	c2->cd();
	surf->Draw("surf");
	
	// Example 3
	c3->cd();
	gated->Sort();
	gated->GetYaxis()->SetRangeUser( 2.460, 2.471 );
	gated->Draw("A*C");
	
	return;
	
}