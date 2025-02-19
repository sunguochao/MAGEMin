/**

Function to allocate memory for solid-solutions        

Holland et al., 2018 - Melting of peridotites through to granites                 
Igneous dataset to use with tc-ds633.txt                                          
"bi","cpx","cd","ep","fl","g","hb","ilm","liq","mu", "ol", "opx","pl4T","spn" 
 
PP & SS_flags
-------------
Declare flags needed for leveling and pge algorithms

+-------+-------+------+------+-------+-------+------+
| SS/PP |  IN	| ACT  | HLD  |  RMV  |  CYC  | REIN |
+=======+=======+======+======+=======+=======+======+
| [0]   | 0/1   |  0/1 | 0/1  | 0/1   | 0/n   | 0/1  |
+-------+-------+------+------+-------+-------+------+
| [1]   | 0/1   |  0/1 | 0/1  | 0/1   | 0/n   | 0/1  |
+-------+-------+------+------+-------+-------+------+
| [2]   | 0/1   |  0/1 | 0/1  | 0/1   | 0/n   | 0/1  |
+-------+-------+------+------+-------+-------+------+
|  ...  | 0/1   |  0/1 | 0/1  | 0/1   | 0/n   | 0/1  |
+-------+-------+------+------+-------+-------+------+
| [m]	| 0/1   |  0/1 | 0/1  | 0/1   | 0/n   | 0/1  |
+-------+-------+------+------+-------+-------+------+
													 
- IN:   allowed phase (satisfying bulk rock constraints)
- ACT:  considered phase (part of the active set of phases)
- HLD:  on hold (not in the active set but still scanned at every iteration)
- RMV:  removed (not considered anymore)
- REIN: phase reintroduced
- m: number of PP or SS
- n: number of cycles

*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <complex.h> 

#include "MAGEMin.h"
#include "gss_init_function.h"

/** 
  allocate memory to store considered phases
*/
csd_phase_set CP_INIT_function(csd_phase_set cp, global_variable gv){
	
	int n 			= gv.len_ox + 1;
	
	/* initialize fractions flags and cycle arrays with zeros */
	cp.ss_flags 	= malloc (gv.n_flags  * sizeof(int));
	
	cp.name   		= malloc (20 * sizeof (char)		);
	cp.p_em   		= malloc (n  * sizeof (double) 		);
	cp.z_em   		= malloc (n  * sizeof (double) 		);
	cp.xi_em  		= malloc (n  * sizeof (double) 		);
	cp.dguess 		= malloc (n  * sizeof (double) 		);
	cp.lvlxeos 		= malloc (n  * sizeof (double) 		);
	cp.xeos   		= malloc (n  * sizeof (double) 		);
	cp.delta_mu		= malloc (n  * sizeof (double) 		);
	cp.dfx   		= malloc (n  * sizeof (double) 		);
	cp.mu    		= malloc (n  * sizeof (double) 		);
	cp.gbase    	= malloc (n  * sizeof (double) 		);
	cp.mu0    		= malloc (n  * sizeof (double) 		);
	cp.ss_comp		= malloc (n  * sizeof (double) 		);
	cp.sf			= malloc ((n*2)  * sizeof (double) 	);
	
	cp.phase_density  		= 0.0;
	cp.phase_cp				= 0.0;
	cp.phase_expansivity	= 0.0;
	
	cp.dpdx 		= malloc ((gv.len_ox+1) * sizeof (double*) 		); 
	for (int i = 0; i < (gv.len_ox+1); i++){
		cp.dpdx[i] 	= malloc ((gv.len_ox) * sizeof (double) 		);
	}
	
	return cp;
}

/** 
  allocate memory for biotite
*/
SS_ref G_SS_bi_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){		
	SS_ref SS_ref_bi_db;
	
	SS_ref_bi_db.is_liq    = 0;	
	SS_ref_bi_db.symmetry  = 1;	
	SS_ref_bi_db.n_sf      = 10;
	SS_ref_bi_db.n_em      = 6;
	SS_ref_bi_db.n_w       = 15;
	SS_ref_bi_db.n_xeos    = 5;
	SS_ref_bi_db.CstFactor = 0;
	
	return SS_ref_bi_db;
}

/** 
  allocate memory for clinopyroxene
*/
SS_ref G_SS_cpx_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_cpx_db;
	
	SS_ref_cpx_db.is_liq    = 0;	
	SS_ref_cpx_db.symmetry  = 0;						  					  
	SS_ref_cpx_db.n_sf      = 13;
	SS_ref_cpx_db.n_em      = 10;
	SS_ref_cpx_db.n_v       = 10;
	SS_ref_cpx_db.n_w       = 45;
	SS_ref_cpx_db.n_xeos    = 9;	
	SS_ref_cpx_db.CstFactor = 1;		  
		
	return SS_ref_cpx_db;
}

/** 
  allocate memory for cordierite
*/
SS_ref G_SS_cd_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_cd_db;
	
	SS_ref_cd_db.is_liq    = 0;		
	SS_ref_cd_db.symmetry  = 1;				  					  
	SS_ref_cd_db.n_sf      = 4;
	SS_ref_cd_db.n_em      = 3;
	SS_ref_cd_db.n_w       = 3;
	SS_ref_cd_db.n_xeos    = 2;
	SS_ref_cd_db.CstFactor = 0;
	
	return SS_ref_cd_db;	
}

/** 
  allocate memory for epidote
*/
SS_ref G_SS_ep_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_ep_db;
	
	SS_ref_ep_db.is_liq     = 0;	
	SS_ref_ep_db.symmetry   = 1;						  					  
	SS_ref_ep_db.n_sf       = 4;
	SS_ref_ep_db.n_em       = 3;
	SS_ref_ep_db.n_w        = 3;
	SS_ref_ep_db.n_xeos     = 2;
	SS_ref_ep_db.CstFactor  = 1;
  
  
	return SS_ref_ep_db;	
}

/** 
  allocate memory for fluid
*/
SS_ref G_SS_fl_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_fl_db;
	
	SS_ref_fl_db.is_liq     = 0;	
	SS_ref_fl_db.symmetry   = 1;						  					  
	SS_ref_fl_db.n_sf       = 12;
	SS_ref_fl_db.n_em       = 11;
	SS_ref_fl_db.n_w        = 55;
	SS_ref_fl_db.n_xeos     = 10;
	SS_ref_fl_db.CstFactor  = 0;

	return SS_ref_fl_db;
}

/** 
  allocate memory for garnet
*/
SS_ref G_SS_g_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_g_db;
	
	SS_ref_g_db.is_liq      = 0;	
	SS_ref_g_db.symmetry    = 0;						  					  
	SS_ref_g_db.n_sf        = 7;
	SS_ref_g_db.n_em        = 6;
	SS_ref_g_db.n_v         = 6;
	SS_ref_g_db.n_w         = 15;
	SS_ref_g_db.n_xeos      = 5;
	SS_ref_g_db.CstFactor   = 1;

	return SS_ref_g_db;
}

/** 
  allocate memory for hornblende
*/
SS_ref G_SS_hb_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_hb_db;
	
	SS_ref_hb_db.is_liq     = 0;	
	SS_ref_hb_db.symmetry   = 0;					  					  
	SS_ref_hb_db.n_sf       = 17;
	SS_ref_hb_db.n_em       = 11;
	SS_ref_hb_db.n_v        = 11;
	SS_ref_hb_db.n_w        = 55;
	SS_ref_hb_db.n_xeos     = 10;
	SS_ref_hb_db.CstFactor  = 0;
	
			  
	return SS_ref_hb_db;	
}

/** 
  allocate memory for ilmenite
*/
SS_ref G_SS_ilm_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_ilm_db;
	
	SS_ref_ilm_db.is_liq    = 0;	
	SS_ref_ilm_db.symmetry  = 1;							  					  
	SS_ref_ilm_db.n_sf      = 6;
	SS_ref_ilm_db.n_em      = 3;
	SS_ref_ilm_db.n_w       = 3;
	SS_ref_ilm_db.n_xeos    = 2; 
	SS_ref_ilm_db.CstFactor = 1;


	return SS_ref_ilm_db;										  
}

/** 
  allocate memory for liquid (melt)
*/
SS_ref G_SS_liq_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_liq_db;

	SS_ref_liq_db.is_liq     = 1;	
	SS_ref_liq_db.symmetry   = 0;							  					  
	SS_ref_liq_db.n_sf       = 18;
	SS_ref_liq_db.n_em       = 12;
	SS_ref_liq_db.n_v        = 12;
	SS_ref_liq_db.n_w        = 66;
	SS_ref_liq_db.n_xeos     = 11;
	SS_ref_liq_db.CstFactor  = 0;

	return SS_ref_liq_db;	
}

/** 
  allocate memory for muscovite
*/
SS_ref G_SS_mu_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){		
	SS_ref SS_ref_mu_db;
	
	SS_ref_mu_db.is_liq     = 0;	
	SS_ref_mu_db.symmetry   = 0;						  					  
	SS_ref_mu_db.n_sf       = 10;
	SS_ref_mu_db.n_em       = 6;
	SS_ref_mu_db.n_v        = 6;
	SS_ref_mu_db.n_w        = 15;
	SS_ref_mu_db.n_xeos     = 5;
	SS_ref_mu_db.CstFactor  = 1;

	
	return SS_ref_mu_db;
}

/** 
  allocate memory for olivine
*/
SS_ref G_SS_ol_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){		
	SS_ref SS_ref_ol_db;
	
	SS_ref_ol_db.is_liq     = 0;	
	SS_ref_ol_db.symmetry   = 1;						  					  
	SS_ref_ol_db.n_sf       = 5;
	SS_ref_ol_db.n_em       = 4;
	SS_ref_ol_db.n_w        = 6;
	SS_ref_ol_db.n_xeos     = 3;
	SS_ref_ol_db.CstFactor  = 1;

	
	return SS_ref_ol_db;
}

/** 
  allocate memory for orthopyroxene
*/
SS_ref G_SS_opx_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){		
	SS_ref SS_ref_opx_db;
	
	SS_ref_opx_db.is_liq    = 0;	
	SS_ref_opx_db.symmetry  = 0;					  					  
	SS_ref_opx_db.n_sf      = 12;
	SS_ref_opx_db.n_em      = 9;
	SS_ref_opx_db.n_v       = 9;
	SS_ref_opx_db.n_w       = 36;
	SS_ref_opx_db.n_xeos    = 8;
	SS_ref_opx_db.CstFactor = 1;

 	
	return SS_ref_opx_db;
}

/** 
  allocate memory for plagioclase
*/
SS_ref G_SS_pl4T_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){
	SS_ref SS_ref_pl4T_db;
	
	SS_ref_pl4T_db.is_liq     = 0;
	SS_ref_pl4T_db.symmetry   = 0;
	SS_ref_pl4T_db.n_sf       = 5;
	SS_ref_pl4T_db.n_em       = 3;
	SS_ref_pl4T_db.n_v        = 3;
	SS_ref_pl4T_db.n_w        = 3;
	SS_ref_pl4T_db.n_xeos     = 2;
	SS_ref_pl4T_db.CstFactor  = 1;
		  		  
	return SS_ref_pl4T_db;										  
}

/** 
  allocate memory for spn
*/
SS_ref G_SS_spn_init_function(SS_ref SS_ref_db, int EM_database, global_variable gv){		
	SS_ref SS_ref_spn_db;
	
	SS_ref_spn_db.is_liq    = 0;
	SS_ref_spn_db.symmetry  = 1;						  					  
	SS_ref_spn_db.n_sf      = 10;
	SS_ref_spn_db.n_em      = 8;
	SS_ref_spn_db.n_w       = 28;
	SS_ref_spn_db.n_xeos    = 7;
	SS_ref_spn_db.CstFactor = 1;
	
	return SS_ref_spn_db;
}

/**
  attributes the right solution phase to the solution phase array
*/
SS_ref G_SS_INIT_EM_function(SS_ref SS_ref_db, int EM_database, char *name, global_variable gv){
								  
	/** if (EM_database == _tc_ds633_) { **/	
	//"bi","cd","cpx","ep","fl","g","hb","ilm","liq","mu","ol","opx","pl4T","spn"	
	if      (strcmp( name, "bi")  == 0 ){
		SS_ref_db  = G_SS_bi_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "cd")  == 0){
		SS_ref_db  = G_SS_cd_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "cpx") == 0){
		SS_ref_db  = G_SS_cpx_init_function(SS_ref_db, EM_database, gv); 		}	
	else if (strcmp( name, "ep")  == 0){
		SS_ref_db  = G_SS_ep_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "fl")  == 0){
		SS_ref_db  = G_SS_fl_init_function(SS_ref_db, EM_database, gv); 		}		
	else if (strcmp( name, "g")   == 0){
		SS_ref_db  = G_SS_g_init_function(SS_ref_db, EM_database, gv); 			}
	else if (strcmp( name, "hb")  == 0){
		SS_ref_db  = G_SS_hb_init_function(SS_ref_db, EM_database, gv); 		}	
	else if (strcmp( name, "ilm") == 0){
		SS_ref_db  = G_SS_ilm_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "liq") == 0){
		SS_ref_db  = G_SS_liq_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "mu")  == 0){
		SS_ref_db  = G_SS_mu_init_function(SS_ref_db, EM_database, gv); 		}	
	else if (strcmp( name, "ol")  == 0){
		SS_ref_db  = G_SS_ol_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "opx") == 0){
		SS_ref_db  = G_SS_opx_init_function(SS_ref_db, EM_database, gv); 		}
	else if (strcmp( name, "pl4T") == 0){
		SS_ref_db  = G_SS_pl4T_init_function(SS_ref_db, EM_database, gv); 		}	
	else if (strcmp( name, "spn") == 0){
		SS_ref_db  = G_SS_spn_init_function(SS_ref_db, EM_database, gv); 		}
	else{
		printf("\nsolid solution '%s' is not in the database, cannot be initiated\n", name);	
	}	
	
	/**
		Allocate memory sollution phase minimization
	*/
	int n_em   = SS_ref_db.n_em;
	int n_xeos = SS_ref_db.n_xeos;
	int n_sf   = SS_ref_db.n_sf;
	int sym    = SS_ref_db.symmetry;
	
	SS_ref_db.EM_list 		 = malloc ((n_em) * sizeof (char*)	);
	for (int i = 0; i < n_em; i++){ 
		SS_ref_db.EM_list[i] = malloc(20 * sizeof(char)		);		
	}

	if (sym == 0){
		SS_ref_db.W   		= malloc (SS_ref_db.n_w * sizeof (double) ); 
		SS_ref_db.v   		= malloc (SS_ref_db.n_v * sizeof (double) ); 
	}
	else if (sym == 1){
		SS_ref_db.W   		= malloc (SS_ref_db.n_w * sizeof (double) ); 
	}
	
	/* initialize fractions flags and cycle arrays with zeros */
	SS_ref_db.ss_flags   = malloc (gv.n_flags  * sizeof(int));
	for (int j = 0; j < gv.n_flags; j++){	
		SS_ref_db.ss_flags[j]   = 0;
	}
	SS_ref_db.ss_n = 0.0;

	for (int i = 0; i < gv.len_ox; i++){
		SS_ref_db.solvus_id = malloc (gv.len_ox * sizeof (int)  	);
	}

	/* dynamic memory allocation of data to send to NLopt */
	SS_ref_db.box_bounds 	= malloc (n_xeos * sizeof (double*)  ); 
	for (int i = 0; i < n_xeos; i++){
		SS_ref_db.box_bounds[i] = malloc (2 * sizeof (double) );
	}
	
	SS_ref_db.eye 			= malloc (n_em 			* sizeof (double*)); 
	SS_ref_db.dp_dx 		= malloc (n_em 			* sizeof (double*)); 
	SS_ref_db.Comp 			= malloc (n_em 			* sizeof (double*)); 
	for (int i = 0; i < n_em; i++){
		SS_ref_db.eye[i] 	= malloc (n_em	 		* sizeof (double) );
		SS_ref_db.Comp[i] 	= malloc (gv.len_ox 	* sizeof (double) );
		SS_ref_db.dp_dx[i] 	= malloc (n_xeos 		* sizeof (double) );
	}
	
	SS_ref_db.gbase   		= malloc (n_em   	 	* sizeof (double) ); 
	SS_ref_db.gb_lvl  		= malloc (n_em   	 	* sizeof (double) ); 
	SS_ref_db.z_em    		= malloc (n_em   	 	* sizeof (double) ); 
	SS_ref_db.density 		= malloc (n_em   	 	* sizeof (double) ); 
	SS_ref_db.dguess 		= malloc (n_xeos 		* sizeof (double) );
	SS_ref_db.iguess  		= malloc (n_xeos   	  	* sizeof (double) );
	SS_ref_db.p       		= malloc (n_em       	* sizeof (double) ); 
	SS_ref_db.ape      		= malloc (n_em       	* sizeof (double) ); 
	SS_ref_db.mat_phi 		= malloc (n_em       	* sizeof (double) ); 
	SS_ref_db.mu_Gex  		= malloc (n_em       	* sizeof (double) ); 
	SS_ref_db.sf      		= malloc (n_sf       	* sizeof (double) ); 
	SS_ref_db.dsf      		= malloc ((n_sf*n_xeos) * sizeof (double) ); 
	SS_ref_db.mu      		= malloc (n_em       	* sizeof (double) ); 
	SS_ref_db.dfx    		= malloc (n_xeos     	* sizeof (double) ); 
	SS_ref_db.ss_comp		= malloc (gv.len_ox  	* sizeof (double) ); 
	SS_ref_db.xi_em   		= malloc (n_em   	 	* sizeof (double) ); 
	SS_ref_db.xeos    		= malloc (n_xeos     	* sizeof (double) ); 	

	/* memory allocation to store all gbase */
	SS_ref_db.mu_array = malloc ((gv.n_Diff) * sizeof (double*) ); 
	for (int i = 0; i < (gv.n_Diff); i++){
		SS_ref_db.mu_array[i] = malloc (n_em * sizeof (double) );
	}	
	
	/* dynamic memory allocation of data to send to NLopt */
	SS_ref_db.box_bounds_default = malloc ((n_xeos) * sizeof (double*) ); 
	for (int i = 0; i < (n_xeos); i++){
		SS_ref_db.box_bounds_default[i] = malloc (2 * sizeof (double) );
	}
	
	/* dynamic memory allocation of data to send to NLopt */
	SS_ref_db.ub   		= malloc ((n_xeos) * sizeof (double) ); 
	SS_ref_db.lb   		= malloc ((n_xeos) * sizeof (double) ); 
	SS_ref_db.tol_sf   	= malloc ((n_sf) * sizeof (double) ); 
	for (int j = 0; j < n_sf; j++){
		SS_ref_db.tol_sf[j] = gv.ineq_res;
	}

	for (int j = 0; j < n_xeos; j++){
		SS_ref_db.dguess[j]  = 0.0;
		SS_ref_db.iguess[j]  = 0.0;
	}
	
	/**
		Allocate memory for pseudocompounds 
	*/
	SS_ref_db.tot_pc 	= 0;
	SS_ref_db.id_pc  	= 0;
	SS_ref_db.n_pc   	= gv.n_pc;								/** maximum number of pseudocompounds to store */
	
	SS_ref_db.G_pc   	= malloc ((SS_ref_db.n_pc) * sizeof (double) ); 
	SS_ref_db.DF_pc 	= malloc ((SS_ref_db.n_pc) * sizeof (double) ); 
	SS_ref_db.factor_pc = malloc ((SS_ref_db.n_pc) * sizeof (double) ); 
	SS_ref_db.n_swap 	= malloc ((SS_ref_db.n_pc) * sizeof (int) 	 ); 
	SS_ref_db.info  	= malloc ((SS_ref_db.n_pc) * sizeof (int) 	 ); 
	SS_ref_db.p_pc 		= malloc ((SS_ref_db.n_pc) * sizeof (double*)); 
	SS_ref_db.mu_pc 	= malloc ((SS_ref_db.n_pc) * sizeof (double*)); 
	
	for (int i = 0; i < (SS_ref_db.n_pc); i++){
		SS_ref_db.p_pc[i] 	 = malloc ((n_em) * sizeof (double) 	);
		SS_ref_db.mu_pc[i] 	 = malloc ((n_em) * sizeof (double) 	);
	}
	SS_ref_db.comp_pc = malloc ((SS_ref_db.n_pc) * sizeof (double*) ); 
	for (int i = 0; i < (SS_ref_db.n_pc); i++){
		SS_ref_db.comp_pc[i] = malloc (gv.len_ox * sizeof (double) 	);
	}
	SS_ref_db.xeos_pc = malloc ((SS_ref_db.n_pc) * sizeof (double*) ); 
	for (int i = 0; i < (SS_ref_db.n_pc); i++){
		SS_ref_db.xeos_pc[i] = malloc ((n_xeos)  * sizeof (double) 	);
	}	
	for (int i = 0; i < SS_ref_db.n_pc; i++){
		SS_ref_db.factor_pc[i] 	= 0.0;
		SS_ref_db.n_swap[i] 	= 0;
		SS_ref_db.info[i]   	= 0;
		SS_ref_db.G_pc[i]   	= 0.0;
		SS_ref_db.DF_pc[i]  	= 0.0;
		for (int j = 0; j < gv.len_ox; j++){
			SS_ref_db.comp_pc[i][j]  = 0.0;	
		}
		for (int j = 0; j < n_em; j++){
			SS_ref_db.p_pc[i][j] = 0.0;	
			SS_ref_db.mu_pc[i][j]= 0.0;	
		}
		for (int j = 0; j < (n_xeos); j++){
			SS_ref_db.xeos_pc[i][j] = 0.0;
		}
	}
	
	SS_ref_db.lb_pc   	 = malloc ((n_em)   * sizeof (double) ); 
	SS_ref_db.ub_pc   	 = malloc ((n_em)   * sizeof (double) ); 
	SS_ref_db.xeos_sf_ok = malloc ((n_xeos) * sizeof (double) );
	
	for (int i = 0; i < (n_xeos); i++){
		SS_ref_db.xeos_sf_ok[i] = 0.0;
	}
	for (int i = 0; i < n_em; i++){
		for (int j = 0; j < n_em; j++){
			SS_ref_db.eye[i][j] = 0.0;
		}	
	}
	
	for (int j = 0; j < n_em; j++){
		SS_ref_db.eye[j][j]= 1.0;
		SS_ref_db.p[j]     = 0.0;
		SS_ref_db.ape[j]   = 0.0;
		SS_ref_db.lb_pc[j] = 0.0;
		SS_ref_db.ub_pc[j] = 1.0;
	}
	SS_ref_db.forced_stop = 0; 
	SS_ref_db.min_mode    = 1;
	SS_ref_db.nlopt_verb  = 0; // no output by default

	return SS_ref_db;
};

