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

/* Matrices */

int **matA;
int **matB;
int **matC;

void multiply(int m_size)
{
    int i, j, k;

    for (i = 0; i < m_size; i++)
    {
        for (j = 0; j < m_size; j++)
        {
            matC[i][j] = 0;
            for (k = 0; k < m_size; k++)
                matC[i][j] += matA[i][k] * matB[k][j];
        }
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
    int opt, m_size = -1;
    bool display = false;

    while ((opt = getopt(argc, argv, "m:d:vh")) != -1)
    {
        switch (opt)
        {
        case 'm':
            m_size = strtoul(optarg, NULL, 0);

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

    if (m_size == -1)
    {
        fprintf(stderr, "All argumments -m are required.\n");
        exit(EXIT_FAILURE);
    }

    /*
    ############################# MATRIX ALLOCATION ##############################
    */

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

    time_t start, end;
    time(&start);

    multiply(m_size);

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
    return 0;
}