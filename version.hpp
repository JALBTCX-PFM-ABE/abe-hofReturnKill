
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/


/*********************************************************************************************

    This program is public domain software that was developed by the U.S. Naval Oceanographic
    Office.

    This is a work of the US Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the US Government.

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.

*********************************************************************************************/

#ifndef VERSION

#define     VERSION     "PFM Software - hofReturnKill V4.00 - 06/06/17"

#endif

/*

    Version 1.00
    Jan C. Depner
    06/15/09

    First version.


    Version 1.01
    Jan C. Depner
    08/20/09

    Added shallow water algorithm and shoreline depth swap kill switch.


    Version 2.00
    Jan C. Depner
    01/11/01

    Replaced the old first return kill with just a return kill based on slope and run required.  This actually makes
    more sense now that we're loading both primary and secondary returns.  Also, lowered the acceptable slope to 
    0.75 (based on empirical evidence).


    Version 2.01
    Jan C. Depner
    01/06/11

    Correct problem with the way I was instantiating the main widget.  This caused an intermittent error on Windows7/XP.


    Version 2.02
    Jan C. Depner
    04/12/11

    Fixed the APD and PMT return filters.  It appears that Optech does not allow a single drop or any number of flat returns to
    end a run.


    Version 3.00
    Jan C. Depner
    05/18/11

    Removed the first return kill option since we weren't using it anymore.  Also, replaced the apd and pmt
    return filters with the simplified versions from hofWaveFilter.  These are faster and better.


    Version 3.01
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 3.02
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).


    Version 3.03
    Jan C. Depner (PFM Software)
    07/29/14

    - Fixed errors discovered by cppcheck.


    Version 3.04
    Jan C. Depner (PFM Software)
    02/16/15

    - To give better feedback to shelling programs in the case of errors I've added the program name to all
      output to stderr.


    Version 3.05
    Jan C. Depner (PFM Software)
    06/03/17

    - Fixed bug caused by not reading the HOF header prior to reading the record.


    Version 4.00
    Jan C. Depner (PFM Software)
    06/06/17

    - No longer does waveform slope filter (hofWaveFilter does that).  Now kills secondary returns.  I know that
      I could have done this in pfmEdit3D but we already had the slot so I just reused it.

*/
