/*
 * This file is part of the GROMACS molecular simulation package.
 *
 * Copyright (c) 2014,2015, by the GROMACS development team, led by
 * Mark Abraham, David van der Spoel, Berk Hess, and Erik Lindahl,
 * and including many others, as listed in the AUTHORS file in the
 * top-level source directory and at http://www.gromacs.org.
 *
 * GROMACS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GROMACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GROMACS; if not, see
 * http://www.gnu.org/licenses, or write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *
 * If you want to redistribute modifications to GROMACS, please
 * consider that scientific software is very special. Version
 * control is crucial - bugs must be traceable. We will be happy to
 * consider code for inclusion in the official distribution, but
 * derived work must not be called official GROMACS. Details are found
 * in the README & COPYING files - if they are missing, get the
 * official version at http://www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the research papers on the package. Check out http://www.gromacs.org.
 */

#ifndef GMX_SIMD_IMPL_INTEL_MIC_COMMON_H
#define GMX_SIMD_IMPL_INTEL_MIC_COMMON_H

/* Intel Xeon Phi, or
 * the-artist-formerly-known-as-Knight's-corner, or
 * the-artist-formerly-formerly-known-as-MIC, or
 * the artist formerly-formerly-formerly-known-as-Larrabee
 * 512-bit SIMD instruction wrappers.
 */

/* Capability definitions for Xeon Phi SIMD */
#define GMX_SIMD                                   1
#define GMX_SIMD_HAVE_FLOAT                        1
#define GMX_SIMD_HAVE_DOUBLE                       1
#define GMX_SIMD_HAVE_LOADU                        1
#define GMX_SIMD_HAVE_STOREU                       1
#define GMX_SIMD_HAVE_LOGICAL                      1
#define GMX_SIMD_HAVE_FMA                          1
#define GMX_SIMD_HAVE_FRACTION                     0
#define GMX_SIMD_HAVE_FINT32                       1
#define GMX_SIMD_HAVE_FINT32_EXTRACT               1
#define GMX_SIMD_HAVE_FINT32_LOGICAL               1
#define GMX_SIMD_HAVE_FINT32_ARITHMETICS           1
#define GMX_SIMD_HAVE_DINT32                       1
#define GMX_SIMD_HAVE_DINT32_EXTRACT               1
#define GMX_SIMD_HAVE_DINT32_LOGICAL               1
#define GMX_SIMD_HAVE_DINT32_ARITHMETICS           1
#define GMX_SIMD4_HAVE_FLOAT                       1
#define GMX_SIMD4_HAVE_DOUBLE                      1

/* Implementation details */
#define GMX_SIMD_FLOAT_WIDTH                      16
#define GMX_SIMD_DOUBLE_WIDTH                      8
#define GMX_SIMD_FINT32_WIDTH                     16
#define GMX_SIMD_DINT32_WIDTH                      8
#define GMX_SIMD4_WIDTH                            4
#define GMX_SIMD_RSQRT_BITS                       23
#define GMX_SIMD_RCP_BITS                         23

#endif /* GMX_SIMD_IMPL_INTEL_MIC_COMMON_H */
