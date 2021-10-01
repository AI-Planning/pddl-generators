#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/times.h>

#ifndef TRUE /* we assume that FALSE is also not defined */
#define TRUE 1
#define FALSE 0
#endif /* TRUE */

#define MY_RANDOM random()
#define MAX_NUM_PACKAGE 100
#define MAX_STRING_LENGTH 50
#define MAX_PREFERENCES   1000
#define MAX_LOCATIONS 100
#define MAX_NUM_TRUCK 50
#define MAX_LENGHT 256
#define PREF_START 'A'
#define PREF_BASE ('Z' - 'A' + 1)


/*
   define boolean types if not allready defined
 */
#ifndef Bool
typedef unsigned char Bool;
#endif /* Bool */

int num_truck, num_loc, num_time = -1, num_package, num_truckarea, seed;
int num_problem = -1, i, j, k, w, num, completely_connected = TRUE;
int completely_rnd_pack_distrib = FALSE;
int final_package_loc[MAX_NUM_PACKAGE], package_loc[MAX_NUM_PACKAGE], safe_generation = FALSE, timeidx;
int simple_preference = FALSE, simple_preference2 = FALSE;
int qualitative_preference = FALSE, truck_location[MAX_NUM_TRUCK];
char domain_name[MAX_STRING_LENGTH];
int used_pref[MAX_PREFERENCES], first, til_version = FALSE, constraint = FALSE;
int x[MAX_LOCATIONS], y[MAX_LOCATIONS], time_version = FALSE, prev;
float drive_time[MAX_LOCATIONS][MAX_LOCATIONS], max_drive_time[MAX_NUM_TRUCK];
char *symb;
char String[MAX_LENGHT][MAX_NUM_PACKAGE];
int package_deadline[MAX_NUM_PACKAGE], complex_preference = FALSE;
float within[MAX_NUM_PACKAGE];



char *pref_symb(int p, int num) {
    char *res;
    int r, q, l;

    res = (char *)calloc(10, sizeof(char));
    sprintf(res, "p%d\0", p);

    l = strlen(res);

    q = num - 1;
    do {
        r = q % PREF_BASE;
        q = q / PREF_BASE;
        res[l++] = (char)(r + PREF_START);
    } while (q > 0);

    res[l] = '\0';

    return res;
}





Bool process_command_line(int argc, char *argv[]) {
    int i, lineok;

    lineok = 0;
    strcpy(&domain_name[0], "Trucks");

    time_version = FALSE;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-')
            continue;

        else if (strcmp(argv[i], "-t") == 0) {
            sscanf(argv[++i], "%d", &num_truck);
            lineok++;
        } else if (strcmp(argv[i], "-l") == 0) {
            sscanf(argv[++i], "%d", &num_loc);
            lineok++;
        }
        /*
        else if (strcmp (argv[i], "-T") == 0 )
      {
        sscanf (argv[++i], "%d", &num_time);
        num_time++;
      }
        */
        else if (strcmp(argv[i], "-p") == 0) {
            sscanf(argv[++i], "%d", &num_package);
            if (num_package > MAX_NUM_PACKAGE) {
                printf("\nError: The number of packages must be lower than %d", MAX_NUM_PACKAGE);
                exit(0);
            }
            lineok++;
        } else if (strcmp(argv[i], "-a") == 0) {
            sscanf(argv[++i], "%d", &num_truckarea);
            lineok++;
        } else if (strcmp(argv[i], "-n") == 0)
            sscanf(argv[++i], "%d", &num_problem);

        else if (strcmp(argv[i], "-seed") == 0)
            sscanf(argv[++i], "%d", &seed);

        /*
        else if (strcmp (argv[i], "-safe") == 0 )
      safe_generation = TRUE;

        else if (strcmp (argv[i], "-qualitativepreference") == 0 )
      {
        qualitative_preference = TRUE;
        strcpy (&domain_name[0], "Trucks-QualitativePreferences");
      }
        else if (strcmp (argv[i], "-simplepreference") == 0 )
      {
        simple_preference = TRUE;
        strcpy (&domain_name[0], "Trucks-SimplePreferences");
      }
        else if (strcmp (argv[i], "-simplepreference2") == 0 )
      {
        simple_preference2 = TRUE;
        strcpy (&domain_name[0], "Trucks-SimplePreferences");
      }
        else if (strcmp (argv[i], "-constraint") == 0 )
      {
        constraint = TRUE; time_version = TRUE;
        strcpy (&domain_name[0], "Trucks-Constraints");
      }
        else if (strcmp (argv[i], "-time") == 0 )
      {
        time_version = TRUE;
        strcpy (&domain_name[0], "Trucks-Time");
      }
        else if (strcmp (argv[i], "-timetil") == 0 )
      {
        til_version = TRUE;
        strcpy (&domain_name[0], "Trucks-TimeTIL");
      }
        else if (strcmp (argv[i], "-complexpreference") == 0 )
      {
        complex_preference = TRUE; time_version = TRUE;
        strcpy (&domain_name[0], "Trucks-ComplexPreferences");
      }
        */
        else {
            printf("\nUnknown option: %s entered\n\n", argv[i]);
            return FALSE;
        }
    }

    if (lineok < 4) {
        printf("\nPlease specify the necessary settings\n\n");
        exit(1);
    }
    return TRUE;
}

void help(void) {
    printf("\nTrucks-Propositional Problem file generator");
    printf("\nUsage: gen-Trucks [options] file");
    printf("\n\nCommand line options:\n");
    printf
        (" -t <num>\tSet the number of trucks \n");
    printf
        (" -l <num>\tSet the number of locations \n");
    printf
        (" -p <num>\tSet the number of packages \n");
    printf
        (" -a <num>\tSet the number of truck areas \n");
    printf
        (" -n <num>\tSet the number of problem \n");
    /*
    printf
      ("-safe                    specifies a problem generation with gentle deadlines\n");
    printf
      ("-T <string>             specifies the number of time steps \n");
    */
    printf
        (" -seed <num>\tSet the seed for random number generator\n\n");
}


int main(int argc, char *argv[]) {
    struct timeval tv;
    struct timezone tz;


    gettimeofday(&tv, &tz);
    seed = ((tv.tv_sec & 0177) * 1000000) + tv.tv_usec;

    /* command line treatment
     */
    if (argc == 1 || (argc == 2 && *++argv[0] == '?')) {
        help();
        exit(1);
    }
    if (!process_command_line(argc, argv)) {
        help();
        exit(1);
    }

    //  printf("\n\nSeed: %d\n", seed);

    srandom(seed);

    memset(used_pref, 0, MAX_PREFERENCES * sizeof(Bool));

    if (constraint || complex_preference)
        for (i = 0; i < MAX_NUM_PACKAGE; i++)
            within[i] = -1;

    if (num_time < 0) {
        if (safe_generation || simple_preference || simple_preference2 || qualitative_preference || complex_preference)
            num_time = num_package * 2 + 1;
        else
            num_time = num_loc * (((num_package - 1) / num_truckarea) + 1) + 1;
    }

    if (safe_generation && num_package * 2 > num_time) {
        printf("The number of available time step could be too low.\n\n");
        exit(0);
    } else if (num_loc * (((num_package - 1) / num_truckarea) + 1) > num_time) {
        printf("The number of available time step could be too low.\n\n");
        exit(0);
    }


    /* Problem declaration */

    printf("; Domain designed by Yannis Dimopoulos, Alfonso Gerevini and Alessandro Saetti\n; This file has been automatically generated by the generator available from\n; http://zeus.ing.unibs.it/ipc-5/generators/index.html\n\n");

    if (num_problem > 0)
        printf("(define (problem truck-%d)", num_problem);
    else
        printf("\n(define (problem truck-problem)");

    printf("\n(:domain %s)", domain_name);

    /* Problem objects */

    printf("\n(:objects");

    for (i = 0; i < num_truck; i++)
        printf("\n\ttruck%d - truck", i + 1);

    for (i = 0; i < num_package; i++)
        printf("\n\tpackage%d - package", i + 1);

    for (i = 0; i < num_loc; i++)
        printf("\n\tl%d - location", i + 1);

    if (!time_version && !til_version)
        for (i = 0; i < num_time; i++)
            printf("\n\tt%d - time", i);

    for (i = 0; i < num_truckarea; i++)
        printf("\n\ta%d - truckarea", i + 1);

    printf(")\n");

    printf("\n(:init");

    /* Initial truck locations */

    for (i = 0; i < num_truck; i++) {
        num = MY_RANDOM % num_loc;
        printf("\n\t(at truck%d l%d)", i + 1, num + 1);
        truck_location[i] = num;
        max_drive_time[i] = 0.0;
    }

    /* truck areas */

    for (i = 0; i < num_truck; i++)
        for (j = 0; j < num_truckarea; j++)
            printf("\n\t(free a%d truck%d)", j + 1, i + 1);

    for (i = 0; i < num_truckarea; i++)
        for (j = i + 1; j < num_truckarea; j++)
            printf("\n\t(closer a%d a%d)", i + 1, j + 1);

    /* Initial package locations */

    if (completely_rnd_pack_distrib) {
        for (i = 0; i < num_package; i++) {
            num = MY_RANDOM % num_loc;
            printf("\n\t(at package%d l%d)", i + 1, num + 1);
            package_loc[i] = num + 1;
        }
    } else {
        i = 0;
        while (i < num_package) {
            num = MY_RANDOM % num_loc;

            for (j = 0; j < num_truckarea; j++) {
                printf("\n\t(at package%d l%d)", i + 1, num + 1);
                package_loc[i] = num + 1;

                i++;
                if (i == num_package)
                    break;
            }
        }
    }

    if (completely_connected == TRUE) {
        for (i = 0; i < num_loc; i++)
            for (j = 0; j < num_loc; j++)
                if (i != j)
                    printf("\n\t(connected l%d l%d)", i + 1, j + 1);
    } else {
        printf("TO BE IMPLEMETED YET...\n\n");
        exit(0);
    }


    /* final locations for goal generation */

    if (!time_version && !til_version) {
        /* time clock */

        printf("\n\t(time-now t0)");

#ifndef __NOTLE__
        for (i = 0; i < num_time - 1; i++)
            for (j = i; j < num_time - 1; j++)
                printf("\n\t(le t%d t%d)", i + 1, j + 1);
#endif

        for (i = 0; i < num_package; i++) {
            do {
                num = MY_RANDOM % num_loc;
            } while (package_loc[i] == (num + 1));

            final_package_loc[i] = num;
        }

        for (i = 0; i < num_package; i++) {
            if (MY_RANDOM % num_truckarea)
                package_deadline[i] = TRUE;
            else
                package_deadline[i] = FALSE;
        }


        for (i = 0; i < num_time - 1; i++)
            printf("\n\t(next t%d t%d)", i, i + 1);

        printf(")\n");
    } else {
        for (i = 0; i < num_loc; i++)
            x[i] = MY_RANDOM % 1000;
        for (i = 0; i < num_loc; i++)
            y[i] = MY_RANDOM % 1000;

        for (i = 0; i < num_package; i++) {
            do {
                num = MY_RANDOM % num_loc;
            } while (package_loc[i] == (num + 1));

            final_package_loc[i] = num;
        }

        for (i = 0; i < num_package; i++) {
            if (MY_RANDOM % num_truckarea)
                package_deadline[i] = TRUE;
            else
                package_deadline[i] = FALSE;
        }



        for (i = 0; i < num_loc; i++)
            for (j = 0; j < num_loc; j++)
                if (i != j)
                    drive_time[i][j] = sqrtf((powf((x[i] - x[j]), 2) + powf(y[i] - y[j], 2)));

        if (til_version || constraint || complex_preference) {
            if (til_version)
                for (i = 0; i < num_package; i++)
                    if (package_deadline[i])
                        printf("\n\t(deliverable package%d l%d)", i + 1, final_package_loc[i] + 1);

            k = 0;
            i = 0;
            j = 0;
            w = 0;

            while (i < num_package) {
                if (w < num_truck)
                    max_drive_time[w % num_truck] = drive_time[truck_location[w]][package_loc[i] - 1] * 1.10;
                else
                    max_drive_time[w % num_truck] += drive_time[final_package_loc[num_truckarea * ((w - num_truck) + 1) - 1]][package_loc[i] - 1] * 1.10;

                while (i < num_truckarea * (w + 1)) {
                    /*
                      if (i > 0)
                      max_drive_time +=  drive_time[final_package_loc[i - 1]][package_loc[i] - 1] * 1.10;
                    */

                    max_drive_time[w % num_truck] += drive_time[package_loc[i] - 1][final_package_loc[i]] * 1.10;

                    //          if (i < num_package - 1)
                    for (; (i < (j + 1) * num_truckarea - 1) && i < num_package - 1; i++)
                        max_drive_time[w % num_truck] += drive_time[final_package_loc[i]][final_package_loc[i + 1]] * 1.10;
                    //          else
                    i++;

                    j++;

                    for (; k < num_truckarea * j && k < num_package; k++)
                        if (package_deadline[k]) {
                            if (til_version)
                                printf("\n\t(at %.1f (not (deliverable package%d l%d)))", max_drive_time[w % num_truck], k + 1, final_package_loc[k] + 1);
                            else
                                within[k] = max_drive_time[w % num_truck];
                        }
                }

                w++;
            }
        }

        for (i = 0; i < num_loc; i++)
            for (j = 0; j < num_loc; j++)
                if (i != j)
                    printf("\n\t(= (drive-time l%d l%d) %.1f)", i + 1, j + 1,
                           sqrtf((powf((x[i] - x[j]), 2) + powf(y[i] - y[j], 2))));

        printf(")\n");
    }


    /* goals */

    printf("\n(:goal (and ");

    if (safe_generation == TRUE) {
        for (i = 0; i < num_package; i++) {
            num = final_package_loc[i];

            if (package_deadline[i])
                printf("\n\t(delivered package%d l%d t%d)", i + 1, num + 1, num_package * 2);
            else
                printf("\n\t(at-destination package%d l%d)", i + 1, num + 1);
        }
        printf(")\n");
    } else {
        //      printf("\nA interesting generation of the goals has not been implemented yet\n\n");
        //      exit(0);

        /*
       The first num_truckarea packages can be delivered within a
       random number between num_locations and num_package * 2 - 1
       The second num_truckarea packages can be delivered within a
       random number between num_locations * 2 and num_package * 2 - 1
       The third num_truckarea packages can be delivered within a
       random number between num_locations * 3 and num_package * 2 - 1
       ...
        */

        for (i = 0; i < num_package; i++) {
            num = final_package_loc[i];

            //      timeidx = num_loc * ((i / num_truckarea) + 1) + (MY_RANDOM % ((num_package * 2 ) - num_loc * ((i / num_truckarea) + 1)));

            if (num_loc <= num_truckarea)
                timeidx = num_loc * ((i / num_truckarea) + 1);
            else
                timeidx = (num_truckarea + 1) * ((i / num_truckarea) + 1);

            if (simple_preference) {
                timeidx = (int)floor((float)timeidx * 0.75);
            }


            //      printf("i = %d -- timeidx = %d", i, timeidx);

            if (package_deadline[i]) {
                if (simple_preference) {
                    printf("\n\t(at-destination package%d l%d)", i + 1, num + 1);
                    for (j = timeidx; j <= num_package * 2; j++) {
                        used_pref[j - timeidx + 1]++;
                        symb = pref_symb(j - timeidx + 1, used_pref[j - timeidx + 1]);
                        printf("\n\t(preference %s (exists (?t - time)\n\t\t (and (delivered package%d l%d ?t) (le ?t t%d))))", symb, i + 1, num + 1, j);
                    }
                } else {
                    if (simple_preference2 || qualitative_preference) {
                        int tmp;

                        printf("\n\t(at-destination package%d l%d)", i + 1, num + 1);

                        if (simple_preference2)
                            tmp = 1;
                        else {
                            if (qualitative_preference)
                                tmp = num_truckarea + 2;
                            else
                                tmp = num_truckarea;
                        }

                        used_pref[tmp]++;
                        symb = pref_symb(tmp, used_pref[tmp]);

                        sprintf(String[i], "(delivered package%d l%d t%d)", i + 1, num + 1, timeidx);

                        if (simple_preference2 || qualitative_preference)
                            printf("\n\t(preference %s (delivered package%d l%d t%d))", symb, i + 1, num + 1, timeidx);
                    } else {
                        if (time_version || til_version) {
                            if (constraint)
                                sprintf(String[i], "(delivered package%d l%d)", i + 1, num + 1);
                            else if (complex_preference || time_version) {
                                printf("\n\t(delivered package%d l%d)", i + 1, num + 1);
                                sprintf(String[i], "(delivered package%d l%d)", i + 1, num + 1);
                            }
                        } else
                            printf("\n\t(delivered package%d l%d t%d)", i + 1, num + 1, timeidx);
                    }
                }
            } else {
                if (simple_preference)
                    printf("\n");

                // MODIFICA QUI

                if (complex_preference || time_version || constraint)
                    printf("\n\t(delivered package%d l%d)", i + 1, num + 1);
                else
                    printf("\n\t(at-destination package%d l%d)", i + 1, num + 1);

                if (simple_preference)
                    printf("\n");

                if (complex_preference || time_version || constraint)
                    sprintf(String[i], "(delivered package%d l%d)", i + 1, num + 1);
                else
                    sprintf(String[i], "(at-destination package%d l%d)", i + 1, num + 1);
            }
        }

        printf("))\n\n");
    }


    if (constraint) {
        printf("(:constraints (and");

        for (i = 0; i < MAX_NUM_PACKAGE; i++)
            if (within[i] > 0)
                printf("\n\t(within %.1f (delivered package%d l%d))", within[i], i + 1, final_package_loc[i] + 1);

        printf("))\n\n");
    }


    if (qualitative_preference || complex_preference) {
        printf("(:constraints (and\n");

        for (i = 0; i < num_truckarea - 1; i++) {
            used_pref[1]++;
            symb = pref_symb(1, used_pref[1]);

            printf("\n\t(forall (?p - package ?t - truck)");
            printf("\n\t   (preference %s (always (forall (?a - truckarea) (imply (in ?p ?t ?a) (closer ?a a%d))))))", symb, i + 2);
        }

        printf("\n");

        prev = 0;
        for (i = 0; i < num_package; prev = i / num_truckarea, i++) {
            //      printf("%d -- %d\n", prev, (i / num_truckarea));
            if ((i / num_truckarea) == prev && i > 0 && !(MY_RANDOM % num_truckarea)) {
                used_pref[num_truckarea - 1]++;
                symb = pref_symb(num_truckarea - 1, used_pref[num_truckarea - 1]);

                printf("\n\t(preference %s (sometime-before %s\n\t\t\t\t\t %s))", symb, String[i], String[i - 1]);
            }
        }

        if (complex_preference) {
            printf("\n");

            for (i = 0; i < MAX_NUM_PACKAGE; i++)
                if (within[i] > 0) {
                    used_pref[num_truckarea + 2]++;
                    symb = pref_symb(num_truckarea + 2, used_pref[num_truckarea + 2]);

                    printf("\n\t(preference %s (within %.1f (delivered package%d l%d)))", symb, within[i], i + 1, final_package_loc[i] + 1);
                }
        }

        used_pref[num_truckarea]++;
        symb = pref_symb(num_truckarea, used_pref[num_truckarea]);

        printf("\n\n\t(forall (?p - package)");
        printf("\n\t   (preference %s (at-most-once (exists (?t - truck ?a - truckarea)", symb);
        printf("\n\t\t(in ?p ?t ?a)))))))\n\n");
    }

    if (simple_preference || simple_preference2 || qualitative_preference || complex_preference) {
        printf("(:metric minimize (+ ");
        first = TRUE;
        for (i = 0; i < MAX_PREFERENCES; i++) {
            if (used_pref[i] <= 0)
                continue;

            for (j = 0; j < used_pref[i]; j++) {
                if (!first)
                    printf("\n\t\t     ");
                printf("(* %d (is-violated %s))", i, pref_symb(i, j + 1));
                first = FALSE;
            }
        }

        printf("))");
    } else if (time_version || til_version)
        printf("(:metric minimize (total-time))");

    printf("\n\n)\n");
}
