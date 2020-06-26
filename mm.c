#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

typedef int bool;

#define false 0
#define true 1

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define handle_error_en(en, msg) \
    do                           \
    {                            \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

/* Matrices */

int **matA;
int **matB;
int **matC;

struct mm
{
    int i_begin, i_end;
    int j_begin, j_end;
    int k_begin, k_end;
};

struct thread_info
{                        /* Used as argument to thread routines */
    pthread_t thread_id; /* ID returned by pthread_create() */
    int thread_num;      /* Application-defined thread # */
    struct mm m_info;    /* Matrix proncessing information */
};

/* Multiplication routine that runs in a thread */

void *multi(void *arg)
{
    struct thread_info *tinfo = (struct thread_info *)arg;

    for (int i = tinfo->m_info.i_begin; i < tinfo->m_info.i_end; i++)
        for (int j = tinfo->m_info.j_begin; j < tinfo->m_info.j_end; j++)
            for (int k = tinfo->m_info.k_begin; k < tinfo->m_info.k_end; k++)
                matC[i][j] += matA[i][k] * matB[k][j];
}

/* 
Defines an interval of multiplication for each thread
i.e. how data will be distributed
*/

void mm_start(struct mm *m_info, int num_threads, int m_size, int d, int tnum)
{
    int porportion = m_size / num_threads;

    if (d == 0)
    {
        m_info->i_begin = tnum * porportion;
        m_info->i_end = m_info->i_begin + porportion;
    }
    else
    {
        m_info->i_begin = 0;
        m_info->i_end = m_size;
    }

    if (d == 1)
    {
        m_info->j_begin = tnum * porportion;
        m_info->j_end = m_info->j_begin + porportion;
    }
    else
    {
        m_info->j_begin = 0;
        m_info->j_end = m_size;
    }

    if (d == 2)
    {
        m_info->k_begin = tnum * porportion;
        m_info->k_end = m_info->k_begin + porportion;
    }
    else
    {
        m_info->k_begin = 0;
        m_info->k_end = m_size;
    }
}

void get_help()
{
    fprintf(stdout, "Options are:\n"
                    "    -h: display what you are reading now\n"
                    "    -m: matrix size\n"
                    "    -t: number of threads\n"
                    "    -d: data distribution strategy (0  = rows, 1 = columns, 2 = blocks, default = 0)\n"
                    "    -v: display output matrix\n");
    fprintf(stdout, "\nRequirements:\n"
                    "    1) -m and -t are required arguments\n"
                    "    2) Number of threads must be at least 2\n"
                    "    3) Matrix size must be multiple of number of threads\n");
}

int main(int argc, char **argv)
{
    /*
    ####################### READ COMMAND LINE INPUTS #######################
    */
    int opt, num_threads = -1, m_size = -1, d = 0;
    bool display = false;

    while ((opt = getopt(argc, argv, "t:m:d:vh")) != -1)
    {
        switch (opt)
        {
        case 't':
            num_threads = strtoul(optarg, NULL, 0);
            break;
        case 'm':
            m_size = strtoul(optarg, NULL, 0);

            break;
        case 'd':
            d = strtoul(optarg, NULL, 0);
            break;
        case 'v':
            display = true;
            break;
        case 'h':
            get_help();
            break;
        case '?':
            if (optopt == 't' || optopt == 'm' || optopt == 'd')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
        default:
            abort();
        }
    }

    if (num_threads == -1 || m_size == -1)
    {
        fprintf(stderr, "All argumments -t -m are required.\n");
        exit(EXIT_FAILURE);
    }

    if (num_threads < 2)
    {
        fprintf(stderr, "Number of threads must be at least 2.\n");
        exit(EXIT_FAILURE);
    }

    /*
    ############################# MATRIX ALLOCATION ##############################
    */

    if (m_size % num_threads != 0 || m_size < num_threads)
    {
        fprintf(stderr, "Matrix size must be multiple of number of threads.\n");
        exit(EXIT_FAILURE);
    }

    /* Allocate memory for A, B and C */

    matA = (int **)calloc(m_size, (sizeof(int *)));
    matB = (int **)calloc(m_size, (sizeof(int *)));
    matC = (int **)calloc(m_size, (sizeof(int *)));

    for (int i = 0; i < m_size; i++)
    {
        matA[i] = (int *)calloc(m_size, sizeof(int));
        matB[i] = (int *)calloc(m_size, sizeof(int));
        matC[i] = (int *)calloc(m_size, sizeof(int));
    }

    /* Generating random values in matA and matB */

    for (int i = 0; i < m_size; i++)
    {
        for (int j = 0; j < m_size; j++)
        {
            matA[i][j] = rand() % 100;
            matB[i][j] = rand() % 100;
        }
    }

    /*
    ############################# MATRIX PRINT ##############################
    */

    if (display)
    {
        /* Displaying matA */

        printf("\nMatrix A\n");
        for (int i = 0; i < m_size; i++)
        {
            for (int j = 0; j < m_size; j++)
                printf("%d ", matA[i][j]);
            printf("\n");
        }

        /* Displaying matB */

        printf("\nMatrix B\n");
        for (int i = 0; i < m_size; i++)
        {
            for (int j = 0; j < m_size; j++)
                printf("%d ", matB[i][j]);
            printf("\n");
        }
    }

    /*
    ################################ THREADS #################################
    */

    /* Initialize thread creation attributes */

    pthread_attr_t attr;
    int s = pthread_attr_init(&attr);

    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    /* Allocate memory for pthread_create() arguments */

    struct thread_info *tinfo;

    tinfo = (struct thread_info *)calloc(num_threads, sizeof(struct thread_info));

    /* Create one thread for each matrix block */

    time_t start, end;
    time(&start);

    for (int tnum = 0; tnum < num_threads; tnum++)
    {
        tinfo[tnum].thread_num = tnum + 1;
        mm_start(&tinfo[tnum].m_info, num_threads, m_size, d, tnum);

        /* The pthread_create() call stores the thread ID into
                  corresponding element of tinfo[] */

        int s = pthread_create(&tinfo[tnum].thread_id, &attr,
                               &multi, &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

    /* Now join with each thread */

    for (int tnum = 0; tnum < num_threads; tnum++)
    {
        int s = pthread_join(tinfo[tnum].thread_id, NULL);
        if (s != 0)
            handle_error_en(s, "pthread_join");
    }

    time(&end);
    double time_spent = end - start;

    /* Displaying matC */

    if (display)
    {
        printf("\nMatrix C\n");
        for (int i = 0; i < m_size; i++)
        {
            for (int j = 0; j < m_size; j++)
                printf("%d ", matC[i][j]);
            printf("\n");
        }
    }

    printf("\nTime spent: %f\n", time_spent);

    free(tinfo);
    return 0;
}