/******************************************************************
*                            Melissa                              *
*-----------------------------------------------------------------*
*   COPYRIGHT (C) 2017  by INRIA and EDF. ALL RIGHTS RESERVED.    *
*                                                                 *
* This source is covered by the BSD 3-Clause License.             *
* Refer to the  LICENCE file for further information.             *
*                                                                 *
*-----------------------------------------------------------------*
*  Original Contributors:                                         *
*    Theophile Terraz,                                            *
*    Bruno Raffin,                                                *
*    Alejandro Ribes,                                             *
*    Bertrand Iooss,                                              *
******************************************************************/

/**
 *
 * @file compute_stats.h
 * @author Terraz Théophile
 * @date 2017-15-01
 *
 **/

#ifndef COMPUTE_STATS_H
#define COMPUTE_STATS_H
#include "melissa_data.h"

void compute_stats (melissa_data_t *data,
                    const int       time_step,
                    const int       nb_vect,
                    double        **in_vect_tab,
                    const int       group_id);

void finalize_stats (melissa_data_t *data);

#endif // COMPUTE_STATS_H
