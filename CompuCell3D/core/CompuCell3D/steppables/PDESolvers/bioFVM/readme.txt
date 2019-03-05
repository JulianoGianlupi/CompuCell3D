BioFVM: Finite Volume Solver for Biological Problems. 

Version:      1.1.6
Release date: 19 August 2017

Homepage:     http://BioFVM.MathCancer.org
Downloads:    http://BioFVM.sf.net

Summary:  
This update includes small bug fixes and enhancements for 
compatibiltiy with PhysiCell. It is focused on improving 
its handling of default settings and Dirichlet conditions. 

New features:
+ Added ability to enable / disable Dirichlet conditions for 
  individual substrates. Use: 
  
  void Microenvironment::set_substrate_dirichlet_activation(
       int substrate_index , bool new_value )
  
  to set substrate with index substrate_index to apply or 
  not apply Dirichlet conditions, for all defined Dirichlet 
  nodes. (e.g., you want to set Dirichlet conditions for the
  first substrate, but not teh others). 
  
+ Added Boolean new use_oxygen_as_first_field to Microenvironment_Options,
  so that initialize_microenvironment() only uses these if the flag 
  is set to true. By default, it is set to true. 
  
+ Calling "Microenvironment.set_density()" or 
  "Microenvironment.add_density()" now sets: 
  
  default_microenvironment_options.use_oxygen_as_first_field = false
  
  so that calling initialize_microenvironment() after defining fields 
  does not overwrite prior user choices. 
  
  These functions also correctly resize 
  
  default_microenvironment_options.Dirichlet_condition_vector and 
  default_microenvironment_options.Dirichlet_activation_vector
  
+ Updated all the add/update_density functions in the Mircoenvironment,
  so that adding a new density sets   
  
+ Updated Microenvironment::apply_dirichlet_conditions() so that 
  it only applies to the substrates j for which 
  
  microenvironment.dirichlet_activation_vector[j] == true 

Bugfixes: 
+ Fixed initialize_microenvironment() so that it now only applies Dirichlet 
  conditions if 
  
  default_microenvironment_options.outer_Dirichlet_conditions == true 
  
+ Updated the makefiles to use ARCH := native, which simplifies setup. 
  This generally has your compiler query your processor for available 
  hardware optimizations, so you don't have to choose this yourself 
  (e.g., ARCH := haswell). I suggest using a compiler at least as 
  recent as gcc/g++ 6.5.x   
  
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  
Version:      1.1.5
Release date: 19 July 2017

Homepage:     http://BioFVM.MathCancer.org
Downloads:    http://BioFVM.sf.net

Summary:  
This update includes small bug fixes and enhancements for 
compatibiltiy with PhysiCell. 

New features:
+ Added Microenvironment::find_substrate_index( std::string ) to make it 
  easier to find . 
  
+ Added Basic_Agent::nearest_gradient( int substrate_index ) to directly 
  access the gradient of the indicated substrate at the agent's  
  current_voxel_index. 
  
+ Added Basic_Agent::nearest_gradient_vector( void ) to directly 
  access a vector of gradients (one for each substrate in the 
  microenvironment) at the agent's current_voxel_index. 
 
+ Added Microenvironment::is_dirichlet_node( int voxel_index ) to 
  easily check if the voxel is a Dirichlet node. 
  
+ Added new class Microenvironment_Options, with a default 
  default_microenvironment_options, to simplify Microenvironment
  setup. The defaults are dx = dy = dz = 20 microns, on 1 cubic mm. 
  
+ Added function initialize_microenvironment() to set up the 
  microenvironment using the options in 
  default_microenvironment_options. The code sets oxygen as the 
  default field, with D = 1e5 micron^2/min, and decay rate = 0.1 1/min
  (a 1 mm diffusion length scale). If 
  default_microenvironment_options.outer_Dirichlet_conditions = true, 
  then we set a 38 mmHg condition on the outer edges, corresponding to
  5% oxygenation (physioxic conditions). 

+ Updated the makefile to default to MARCH := native, which allows
  automated performance tuning without user editing of the Makefile.

Bugfixes: 
+ correct typos in citation information in all source files 

+ updated citation information 

+ added void set_default_microenvironment( Microenvironment* M ) declaration to 
  BioFVM_Microenvironment.h
 
+ set volume_is_changed = false in Basic_Agent::set_internal_uptake_constants(); 

+ Set the MultiCellDS options Booleans to extern bool in BioFVM_MultiCellDS.h 
  so that PhysiCell can read these options. 

+ Updated the simlified_data field in MultiCellDS XML output to include a 
  new "source" attribute with value "BioFVM".

+ Updated Microenvironment::update_dirichlet_node() and 
  Microenvironment::remove_dirichlet_node() to check against 
  mesh.voxels[].is_Dirichlet to provide a cheap and early exit 
  if the node isn't Dirichlet in the first place. 
  
+ Changed to a thread-safe data structure for Dirichlet nodes 
  (e.g., if a custom cell function in an OMP loop adds or removes 
  Dirichlet nodes). 
  
+ Fixed convergence_test3.cpp, convergence_test4_1.cpp, 
  convergence_test4_2.cpp, convergence_test5.cpp, 
  performance_test_numcells.cpp, tutorial1_BioFVM.cpp, 
  and main_experiment.cpp to use Basic_Agent::set_total_volume(). 

+ Fixed tutorial3_BioFVM.cpp to use dirichlet_one instead of 
  dirichlet_zero. 
