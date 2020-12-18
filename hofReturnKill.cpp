
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

#include "hofReturnKill.hpp"
#include "version.hpp"


/***************************************************************************\
*                                                                           *
*   Module Name:        hofReturnKill                                       *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       December 05, 2007                                   *
*                                                                           *
*   Purpose:            Invalidate HOF returns in a portion of a PFM based  *
*                       on the slope and amplitude of the associated        *
*                       waveforms.                                          *
*                                                                           *
*   Caveats:            This program is not meant to be run from the        *
*                       command line.  It should only be run as a QProcess  *
*                       from pfmEdit.                                       *
*                                                                           *
\***************************************************************************/

int32_t main (int32_t argc, char **argv)
{
  new hofReturnKill (argc, argv);
}


void hofReturnKill::usage ()
{
  fprintf (stderr, "\nUsage: hofReturnKill [-s] --shared_file SHARED_FILE\n");
  fprintf (stderr, "\nWhere:\n\n");
  fprintf (stderr, "\t-s = invalidate Shoreline Depth Swapped (abdc = 72) or \n");
  fprintf (stderr, "\t\tShallow Water Algorithm (abdc = 74) processed data.\n");
  fprintf (stderr, "\tSHARED_FILE = shared file name from pfmEdit or pfmEdit3D\n\n");
  fprintf (stderr, "This program is not meant to be run from the command line.  It should only be\n");
  fprintf (stderr, "run as a QProcess from pfmEdit or pfmEdit3D.\n\n");
  fflush (stderr);
}


hofReturnKill::hofReturnKill (int32_t argc, char **argv)
{
  char               shared_file[1024], tmp_file[1024], string[1024];
  FILE               *fp = NULL, *shfp = NULL, *shfp2 = NULL;
  HOF_HEADER_T       hof_header;
  HYDRO_OUTPUT_T     hof_record;
  char               c;
  extern char        *optarg;
  int32_t            rec_num, point, count;
  uint8_t            new_file = NVTrue, kill_shallow = NVFalse;
  SHARED_FILE_STRUCT data;


  if (argc < 2)
    {
      usage ();
      exit (-1);
    }


  strcpy (progname, argv[0]);
  int32_t option_index = 0;
  int32_t key = 0;
  while (NVTrue) 
    {
      static struct option long_options[] = {{"shared_file", required_argument, 0, 0},
					     {"shared_memory_key", required_argument, 0, 0},
                                             {0, no_argument, 0, 0}};

      c = (char) getopt_long (argc, argv, "s", long_options, &option_index);
      if (c == -1) break;

      switch (c) 
        {
        case 0:

          switch (option_index)
            {
            case 0:
              strcpy (shared_file, optarg);
              break;

            case 1:
	      sscanf (optarg, "%d", &key);
              break;
            }

          break;

        case 's':
          kill_shallow = NVTrue;
          break;

        default:
          usage ();
          exit (-1);
          break;
        }
    }


  /******************************************* IMPORTANT NOTE ABOUT SHARED MEMORY **************************************** \

      This is a little note about the use of shared memory within the Area-Based Editor (ABE) programs.  If you read
      the Qt documentation (or anyone else's documentation) about the use of shared memory they will say "Dear [insert
      name of omnipotent being of your choice here], whatever you do, always lock shared memory when you use it!".
      The reason they say this is that access to shared memory is not atomic.  That is, reading shared memory and then
      writing to it is not a single operation.  An example of why this might be important - two programs are running,
      the first checks a value in shared memory, sees that it is a zero.  The second program checks the same location
      and sees that it is a zero.  These two programs have different actions they must perform depending on the value
      of that particular location in shared memory.  Now the first program writes a one to that location which was
      supposed to tell the second program to do something but the second program thinks it's a zero.  The second program
      doesn't do what it's supposed to do and it writes a two to that location.  The two will tell the first program 
      to do something.  Obviously this could be a problem.  In real life, this almost never occurs.  Also, if you write
      your program properly you can make sure this doesn't happen.  In ABE we almost never lock shared memory because
      something much worse than two programs getting out of sync can occur.  If we start a program and it locks shared
      memory and then dies, all the other programs will be locked up.  When you look through the ABE code you'll see
      that we very rarely lock shared memory, and then only for very short periods of time.  This is by design.

  \******************************************* IMPORTANT NOTE ABOUT SHARED MEMORY ****************************************/


  //  Get the shared memory area.  If it doesn't exist, quit.  It should
  //  have already been created by pfmEdit.  The key is the process ID of the bin viewer (pfmView)
  //  plus _abe.

  if (!key)
    {
      fprintf (stderr, "%s %s %s %d - shared_memory_key option not specified on command line.  Terminating!\n", progname, __FILE__, __FUNCTION__, __LINE__);
      exit (-1);
    }

  QString skey;
  skey.sprintf ("%d_abe", key);

  abeShare = new QSharedMemory (skey);

  if (!abeShare->attach (QSharedMemory::ReadWrite))
    {
      fprintf (stderr, "%s %s %s %d - abeShare - %s\n", progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  abe_share = (ABE_SHARE *) abeShare->data ();


  if ((shfp = fopen (shared_file, "r")) == NULL)
    {
      fprintf (stderr, "%s %s %s %d - %s - %s\n", progname, __FILE__, __FUNCTION__, __LINE__, shared_file, strerror (errno));
      return;
    }

  sprintf (tmp_file, "%s_2", shared_file);
  if ((shfp2 = fopen (tmp_file, "w")) == NULL)
    {
      fprintf (stderr, "%s %s %s %d - %s - %s\n", progname, __FILE__, __FUNCTION__, __LINE__, tmp_file, strerror (errno));
      fclose (shfp);
      return;
    }


  new_file = NVTrue;
  fp = NULL;
  uint8_t mod = NVFalse;
  while (ngets (string, sizeof (string), shfp) != NULL)
    {
      if (strstr (string, "EOF")) break;

      if (strstr (string, "EOD"))
        {
          new_file = NVTrue;
        }
      else
        {
          if (new_file)
            {
              if (fp) fclose (fp);

              if ((fp = open_hof_file (string)) == NULL)
                {
                  fprintf (stderr, "%s %s %s %d - %s - %s\n", progname, __FILE__, __FUNCTION__, __LINE__, string, strerror (errno));
                  fclose (shfp);
                  fclose (shfp2);
                  return;
                }

              hof_read_header (fp, &hof_header);

              new_file = NVFalse;
            }
          else
            {
              int32_t tmpi;

              sscanf (string, "%d %d %lf %lf %f %f %f %d %hd %hd %d %d %d %d", &point, &count, &data.x, &data.y, 
                      &data.z, &data.herr, &data.verr, &data.val, &data.pfm, &data.file, &data.line, &data.rec, &data.sub, &tmpi);
              data.exflag = (uint8_t) tmpi;

              rec_num = data.rec;

              hof_read_record (fp, rec_num, &hof_record);


              killed = NVFalse;


              if (kill_shallow)
                {
                  if (hof_record.abdc == 72 || hof_record.abdc == 74)
                    {
                      killed = NVTrue;
                      data.val = PFM_FILTER_INVAL;
                      mod = NVTrue;
                    }
                }
              else
                {
                  if (data.sub == 1)
                    {
                      killed = NVTrue;
                      data.val = PFM_FILTER_INVAL;
                      mod = NVTrue;
                    }
                }

              fprintf (shfp2, "%d %d %0.9f %0.9f %0.3f %0.3f %0.3f %d %d %d %d %d %d %d %d\n", point, count, data.x, data.y,
                       data.z, data.herr, data.verr, data.val, data.pfm, data.file, data.line, data.rec, data.sub, data.exflag, killed);
            }
        }
    }

  fprintf (shfp2, "EOF\n");
  fclose (shfp);
  fclose (shfp2);
  remove (shared_file);
  rename (tmp_file, shared_file);


  //  Lock shared memory while we're modifying things.

  abeShare->lock ();


  if (mod)
    {
      abe_share->modcode = PFM_CHARTS_HOF_DATA;
    }
  else
    {
      abe_share->modcode = NO_ACTION_REQUIRED;
    }


  abeShare->unlock ();
  abeShare->detach ();
}


hofReturnKill::~hofReturnKill ()
{
}
