/*
 * $Id: mdrun.c,v 1.139.2.9 2009/05/04 16:13:29 hess Exp $
 *
 *                This source code is part of
 *
 *                 G   R   O   M   A   C   S
 *
 *          GROningen MAchine for Chemical Simulations
 *
 *                        VERSION 3.2.0
 * Written by David van der Spoel, Erik Lindahl, Berk Hess, and others.
 * Copyright (c) 1991-2000, University of Groningen, The Netherlands.
 * Copyright (c) 2001-2004, The GROMACS development team,
 * check out http://www.gromacs.org for more information.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * If you want to redistribute modifications, please consider that
 * scientific software is very special. Version control is crucial -
 * bugs must be traceable. We will be happy to consider code for
 * inclusion in the official distribution, but derived work must not
 * be called official GROMACS. Details are found in the README & COPYING
 * files - if they are missing, get the official version at www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the papers on the package - you can find them in the top README file.
 *
 * For more info, check our website at http://www.gromacs.org
 *
 * And Hey:
 * Gallium Rubidium Oxygen Manganese Argon Carbon Silicon
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include "typedefs.h"
#include "sysstuff.h"
#include "statutil.h"
#include "macros.h"
#include "copyrite.h"
#include "main.h"
#include "gmx_ana.h"

int gmx_membed(int argc,char *argv[])
{
	const char *desc[] = {
			"g_membed embeds a membrane protein into an equilibrated lipid bilayer at the position",
			"and orientation specified by the user.\n",
			"\n",
			"SHORT MANUAL\n------------\n",
			"The user should merge the structure files of the protein and membrane (+solvent), creating a",
			"single structure file with the protein overlapping the membrane at the desired position and",
			"orientation. Box size should be taken from the membrane structure file. The corresponding topology",
			"files should also be merged. Consecutively, create a tpr file (input for g_membed) from these files,"
			"with the following options included in the mdp file.\n",
			" - integrator      = md\n",
			" - energygrp       = Protein (or other group that you want to insert)\n",
			" - freezegrps      = Protein\n",
			" - freezedim       = Y Y Y\n",
			" - energygrp_excl  = Protein Protein\n",
			"The output is a structure file containing the protein embedded in the membrane. If a topology",
			"file is provided, the number of lipid and ",
			"solvent molecules will be updated to match the new structure file.\n",
			"For a more extensive manual see Wolf et al, J Comp Chem 31 (2010) 2169-2174, Appendix.\n",
			"\n",
			"SHORT METHOD DESCRIPTION\n",
			"------------------------\n",
			"1. The protein is resized around its center of mass by a factor -xy in the xy-plane",
			"(the membrane plane) and a factor -z in the z-direction (if the size of the",
			"protein in the z-direction is the same or smaller than the width of the membrane, a",
			"-z value larger than 1 can prevent that the protein will be enveloped by the lipids).\n",
			"2. All lipid and solvent molecules overlapping with the resized protein are removed. All",
			"intraprotein interactions are turned off to prevent numerical issues for small values of -xy",
			" or -z\n",
			"3. One md step is performed.\n",
			"4. The resize factor (-xy or -z) is incremented by a small amount ((1-xy)/nxy or (1-z)/nz) and the",
			"protein is resized again around its center of mass. The resize factor for the xy-plane",
			"is incremented first. The resize factor for the z-direction is not changed until the -xy factor",
			"is 1 (thus after -nxy iteration).\n",
			"5. Repeat step 3 and 4 until the protein reaches its original size (-nxy + -nz iterations).\n",
			"For a more extensive method descrition see Wolf et al, J Comp Chem, 31 (2010) 2169-2174.\n",
			"\n",
			"NOTE\n----\n",
			" - Protein can be any molecule you want to insert in the membrane.\n",
			" - It is recommended to perform a short equilibration run after the embedding",
			"(see Wolf et al, J Comp Chem 31 (2010) 2169-2174, to re-equilibrate the membrane. Clearly",
			"protein equilibration might require longer.\n",
			" - It is now also possible to use the g_membed functionality with mdrun. You should than pass",
			"a data file containing the command line options of g_membed following the -membed option, for",
			"example mdrun -s into_mem.tpr -membed membed.dat.",
			"\n"
	};
	t_filenm fnm[] = {
			{ efTPX, "-f",      "into_mem", ffREAD },
			{ efNDX, "-n",      "index",    ffOPTRD },
			{ efTOP, "-p",      "topol",    ffOPTRW },
			{ efTRN, "-o",      NULL,       ffWRITE },
			{ efXTC, "-x",      NULL,       ffOPTWR },
			{ efSTO, "-c",      "membedded",  ffWRITE },
			{ efEDR, "-e",      "ener",     ffWRITE },
                        { efDAT, "-dat",    "membed",   ffWRITE }
	};
#define NFILE asize(fnm)

	/* Command line options ! */
	real xy_fac = 0.5;
	real xy_max = 1.0;
	real z_fac = 1.0;
	real z_max = 1.0;
	int it_xy = 1000;
	int it_z = 0;
	real probe_rad = 0.22;
	int low_up_rm = 0;
	int maxwarn=0;
	int pieces=1;
        bool bALLOW_ASYMMETRY=FALSE;
        int nstepout=100;
        bool bVerbose=FALSE;
        char *mdrun_path=NULL;

/* arguments relevant to OPENMM only*/
#ifdef GMX_OPENMM
    gmx_input("g_membed not functional in openmm");
#endif

	t_pargs pa[] = {
			{ "-xyinit",   FALSE, etREAL,  {&xy_fac},   	
				"Resize factor for the protein in the xy dimension before starting embedding" },
			{ "-xyend",   FALSE, etREAL,  {&xy_max},
				"Final resize factor in the xy dimension" },
			{ "-zinit",    FALSE, etREAL,  {&z_fac},
		  		"Resize factor for the protein in the z dimension before starting embedding" },
			{ "-zend",    FALSE, etREAL,  {&z_max},
		    		"Final resize faction in the z dimension" },
			{ "-nxy",     FALSE,  etINT,  {&it_xy},
			        "Number of iteration for the xy dimension" },
			{ "-nz",      FALSE,  etINT,  {&it_z},
			        "Number of iterations for the z dimension" },
			{ "-rad",     FALSE, etREAL,  {&probe_rad},
				"Probe radius to check for overlap between the group to embed and the membrane"},
			{ "-pieces",  FALSE,  etINT,  {&pieces},
			        "Perform piecewise resize. Select parts of the group to insert and resize these with respect to their own geometrical center." },
		        { "-asymmetry",FALSE, etBOOL,{&bALLOW_ASYMMETRY}, 
				"Allow asymmetric insertion, i.e. the number of lipids removed from the upper and lower leaflet will not be checked." },
	                { "-ndiff" ,  FALSE, etINT, {&low_up_rm},
			        "Number of lipids that will additionally be removed from the lower (negative number) or upper (positive number) membrane leaflet." },
			{ "-maxwarn", FALSE, etINT, {&maxwarn},		
				"Maximum number of warning allowed" },
			{ "-stepout", FALSE, etINT, {&nstepout},
			        "HIDDENFrequency of writing the remaining runtime" },
			{ "-v",       FALSE, etBOOL,{&bVerbose},
			        "Be loud and noisy" },
			{ "-mdrun_path", FALSE, etSTR, {&mdrun_path},
				"Path to the mdrun executable compiled with this g_membed version" }
	};

        FILE *data_out;
        output_env_t oenv;
        char buf[256],buf2[64];


        parse_common_args(&argc,argv,0, NFILE,fnm,asize(pa),pa,
                    asize(desc),desc,0,NULL, &oenv);

        data_out = ffopen(opt2fn("-dat",NFILE,fnm),"w");
        fprintf(data_out,"nxy = %d\nnz = %d\nxyinit = %f\nxyend = %f\nzinit = %f\nzend = %f\n"
			"rad = %f\npieces = %d\nasymmetry = %s\nndiff = %d\nmaxwarn = %d\n",
			it_xy,it_z,xy_fac,xy_max,z_fac,z_max,probe_rad,pieces,
			bALLOW_ASYMMETRY ? "yes" : "no",low_up_rm,maxwarn);
        fclose(data_out);

        sprintf(buf,"%s -s %s -membed %s -o %s -c %s -e %s -nt 1 -cpt -1",
		    (mdrun_path==NULL) ? "mdrun" : mdrun_path,
		    opt2fn("-f",NFILE,fnm),opt2fn("-dat",NFILE,fnm),opt2fn("-o",NFILE,fnm),
		    opt2fn("-c",NFILE,fnm),opt2fn("-e",NFILE,fnm));
        if (opt2bSet("-n",NFILE,fnm))
	{
		sprintf(buf2," -mn %s",opt2fn("-n",NFILE,fnm));
		strcat(buf,buf2);
        }
	if (opt2bSet("-x",NFILE,fnm))
	{
		sprintf(buf2," -x %s",opt2fn("-x",NFILE,fnm));
                strcat(buf,buf2);
	}
        if (opt2bSet("-p",NFILE,fnm))
        {
                sprintf(buf2," -mp %s",opt2fn("-p",NFILE,fnm));
                strcat(buf,buf2);
        }
	if (bVerbose)
	{
		sprintf(buf2," -v -stepout %d",nstepout);
		strcat(buf,buf2);
	}

        printf("%s\n",buf);
        system(buf);

        fprintf(stderr,"Please cite:\nWolf et al, J Comp Chem 31 (2010) 2169-2174.\n");

	return 0;
}