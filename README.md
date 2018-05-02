### ReadFieldMap

Read the field mapping data from ISS and create a ROOT file/tree plus a text
file with all measured data points.

```
Usage:
  ReadFieldMap [OPTION...]

  -d, --data_dir /path/to/data  Path to data directory
  -z, --z_shift                 shifted in z
  -p, --phi_shift               shifted in phi
  -x, --ext_offset offset       Offset of the extension in mm
  -h, --help                    Print help
```

The outputted ROOT tree has a number of branches:

```
root [1] .ls
TFile**		ISS_Field_Map_total.root	
TFile*		ISS_Field_Map_total.root	
KEY: TTree	Bmap;1	ISS field mapping data
root [2] Bmap->Show(1)
======> EVENT:1
x               = 321.375
y               = -116.971
z               = 1372.2
r               = 342
phi             = -20
T               = 23.25
Bvec            = (TVector3*)0x7fab4d155c60
Bx              = -0.495308
By              = 0.316031
Bz              = -0.002843
B               = 0.587549
```
The geometry is such that z is the beam direction, x is horizontal across the bore
and y is vertical. The probes do not have this geometry, however.

And you can draw the data quite easily for a given set of conditions, e.g:
```
root [3] Bmap->Draw("B:z:phi","r<1 && r>-1")
```
```
root [4] Bmap->Draw("B:z:r","phi<1 && phi>-1")
```
```
root [5] Bmap->Draw("B:phi:r","z<25 && z>-25")
```

or you can use the plot3D.cc example to build your own histograms, graphs etc.
```
root -l plot3D.cc
```

You can also easily spit out the data that you want, in the format that you want,
in case you decide to plot it in some other way and with some other software.
There is an example file, printData.cc, to do this:
```
root -l -q -b printData.cc > outfile.txt
```
(Note: you may have a spurious ROOT message on the first line of the file if you do it
this crudely. Better to have an ofstream in the macro, but I'm too lazy for the example!)