#include <glob.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>

#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

vector<int> probes;
vector<float> rvec; // mm

// Branches for TTree
int probeID;
TVector3 Bvec;
float temperature;
float x_real, y_real, z_real, r_real, phi_real;
float Bx, By, Bz, B;


vector<string> globVector( const string& pattern ) {
	
	glob_t glob_result;
	glob( pattern.c_str(), GLOB_TILDE, NULL, &glob_result );
	vector<string> files;

	for( unsigned int i = 0; i < glob_result.gl_pathc; ++i ) {
		files.push_back(string(glob_result.gl_pathv[i]));
	}

	globfree(&glob_result);

	return files;

}

string convertInt( int number ) {
	
	stringstream ss;
	ss << number;
	return ss.str();
	
}

string convertFloat( float number ) {
	
	stringstream ss;
	ss << number;
	return ss.str();
	
}

void CreateTree( TTree *tree ) {
	
	tree->Branch( "x", &x_real );
	tree->Branch( "y", &y_real );
	tree->Branch( "z", &z_real );
	tree->Branch( "r", &r_real );
	tree->Branch( "phi", &phi_real );
	tree->Branch( "T", &temperature );
	tree->Branch( "Bvec", &Bvec );
	tree->Branch( "Bx", &Bx );
	tree->Branch( "By", &By );
	tree->Branch( "Bz", &Bz );
	tree->Branch( "B", &B );
	
	return;
	
}

void SetProbes( float ext_offset ) {
	
	// Normal probes
	probes.push_back(  75 );	rvec.push_back(  399.0 );
	probes.push_back( 132 );	rvec.push_back(  342.0 );
	probes.push_back(  49 );	rvec.push_back(  285.0 );
	probes.push_back( 134 );	rvec.push_back(  228.0 );
	probes.push_back( 135 );	rvec.push_back(  171.0 );
	probes.push_back( 136 );	rvec.push_back(  114.0 );
	probes.push_back( 137 );	rvec.push_back(   57.0 );
	probes.push_back( 138 );	rvec.push_back(    0.0 );
	probes.push_back( 139 );	rvec.push_back(  -28.5 );
	probes.push_back( 140 );	rvec.push_back(  -85.5 );
	probes.push_back( 141 );	rvec.push_back( -142.5 );
	probes.push_back( 142 );	rvec.push_back( -199.5 );
	probes.push_back( 143 );	rvec.push_back( -256.5 );
	probes.push_back( 144 );	rvec.push_back( -313.5 );
	probes.push_back( 145 );	rvec.push_back( -370.5 );
	probes.push_back( 146 );	rvec.push_back( -427.5 );
	
	// Extension bar
	float F = 1.0;
	if( ext_offset < 0 ) F = -1.0;
	probes.push_back(  14 );	rvec.push_back( ext_offset + F*0.0 );
	probes.push_back(  15 );	rvec.push_back( ext_offset + F*57.0 );
	probes.push_back(  16 );	rvec.push_back( ext_offset + F*114.0 );
	probes.push_back( 147 );	rvec.push_back( ext_offset + F*171.0 );
	probes.push_back( 148 );	rvec.push_back( ext_offset + F*228.0 );
	
	return;
	
}

void GetZPhi( string filename, int &z, int &phi ) {

	size_t start = filename.find_last_of( "/" );
	size_t end = filename.find_last_of( "." );
	string raw = filename.substr( start+1, end-start-1 );
	
	string zphi = raw;
	zphi.replace(0,1,"");
	zphi.replace(2,4," ");
	
	stringstream ss;
	ss << zphi;
	ss >> z >> phi;
	
	return;

}

float GetRealZ( int _z, bool zflag ) {
	
	// Normal configuration
	if( !zflag ) {
		
		return ( _z - 26 ) * 50.0 - 27.8; // mm from origin
		
	}
	
	// Shifted-z configuration
	else {
		
		return ( _z - 26 ) * 50.0 - 358.8; // mm from origin
		
	}
	
}

float GetRealR( int _probeID, bool zflag ) {
	
	bool flag = false;
	float _r;
	
	for( unsigned int i = 0; i < probes.size(); i++ ) {
		
		if( ( _probeID > 0 && _probeID == probes.at(i) ) ||
		    ( _probeID < 0 && _probeID == (int) (i+1) * -1 ) ) {
			
			_r = rvec.at(i);
			flag = true;
		
		}
		
	}
	
	if( flag ) {
		
		if( zflag ) return -1.0 * _r; // flip the bar!
		else return _r;
	
	}
	
	else {
		
		cerr << "Couldn't find probe " << _probeID << endl;
		exit(1);
	
	}
}

float GetRealPhi( int _phi, bool phiflag ) {
	
	float phi;
	
	// Normal phi configuration
	if( !phiflag ) {
		
		phi = ( _phi - 5 ) * 5.0; // degrees from horizontal
		
	}
	
	// Shifted-phi configuration
	else {
		
		phi = ( _phi - 5 ) * 5.0 - 2.5; // degrees from horizontal
		
	}
	
	if( phi < 0.0 ) phi += 360.;
	
	return phi;
	
}

float GetRealX( float _r, float _phi ) {
	
	return _r * TMath::Cos( _phi * TMath::DegToRad() );
	
}

float GetRealY( float _r, float _phi ) {
	
	return _r * TMath::Sin( _phi * TMath::DegToRad() );
	
}

int CheckProbes( string filename, vector<int> &probeIDs ) {
	
	ifstream in;
	in.open( filename.c_str() );
	
	string line, info, s_tmp;
	string qry = "CONFIG ";
	stringstream ss;
	int id_tmp, n_tmp;
	
	if( in.is_open() ) {
		
		getline( in, line );
		while( !in.eof() ) {
			
			if( line.compare( 0, qry.size(), qry ) == 0 ) {
				
				info = line.substr( qry.size()+1, 13 );
				ss.str("");
				ss.clear();
				ss << info;
				ss >> n_tmp >> s_tmp >> id_tmp;
				
				if( id_tmp > 0 ) probeIDs.push_back( id_tmp );
				else probeIDs.push_back( n_tmp * -1 );
				
			}

			getline( in, line );
			
		}
		
		in.close();
		
	}
	
	else {
		
		cerr << "Couldn't open " << filename << endl;
		exit(1);
		
	}
	
	return probeIDs.size();
	
}

TVector3 GetBvector( string filename, int _probeID, float &_temperature ) {
	
	ifstream in;
	in.open( filename.c_str() );
	
	string line, date, time;
	string qry = " 2017-";
	stringstream ss;
	int id_tmp, Nmeasure = 0;
	float Bx_tmp, By_tmp, Bz_tmp, T_tmp;
	float _Bx = 0, _By = 0, _Bz = 0, _T = 0;
	
	if( in.is_open() ) {
		
		getline( in, line );
		while( !in.eof() ) {
			
			if( line.compare( 0, qry.size(), qry ) == 0 ) {
				
				ss.str("");
				ss.clear();
				ss << line;
				ss >> date >> time >> id_tmp;
				ss >> Bz_tmp >> Bx_tmp >> By_tmp >> T_tmp;
				
				if( ( _probeID > 0 && id_tmp == _probeID ) ||
				    ( _probeID < 0 && id_tmp == _probeID * -1 ) ) {
					
					Nmeasure++;
					_Bx += Bx_tmp;
					_By += By_tmp;
					_Bz += Bz_tmp;
					_T  += T_tmp;
					
				}

			}
			
			getline( in, line );
			
		}
		
		in.close();
		
	}
	
	else {
		
		cerr << "Couldn't open " << filename << endl;
		exit(1);
		
	}
	
	_Bx /= (float)Nmeasure;
	_By /= (float)Nmeasure;
	_Bz /= (float)Nmeasure;
	_temperature = _T / (float)Nmeasure;
	
	return TVector3( _Bx, _By, _Bz );
	
}

void RotateVector( int _probeID, float _phi, bool zflag, TVector3 &_vec ) {
	
	_vec.RotateZ( -1.0 * _phi * TMath::DegToRad() ); // rotate the probe axis in phi
	
	if( zflag ) {

		_vec.RotateX( TMath::Pi() ); // flip the bar!
		_vec.RotateY( TMath::Pi() ); // flip the bar!
	
	}
	
	if( _probeID == 139 )
		_vec.RotateZ( TMath::Pi() ); // probe was upside down
	
	return;
	
}