#Notes on how it is right now

* CC3D installs on ubuntu and windows

* Using global pressure and global volume works. Cell type volume + global 
pressure segfaults

* by cell type seg faults

* by cell id seg faults

* not using XML need to be tested

# The issue seems to be with 

vtkDataArrayTemplate<float>::GetTuple(long long, double*)