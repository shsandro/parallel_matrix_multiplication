# A simple parallel algorithm for random matrix multiplication

The algorithm implements a matrix multiplication using threads, from the [pthread](https://computing.llnl.gov/tutorials/pthreads/) library. Square matrices with a specified size are ramdomly generated, and then multiplied in _n_ threads with an specific data distribution.

# Usage

All you need to do is specify the matrix size, the number of threads and the data distribution format. A CPU running time is calculated and then displayed at the end of the execution. A sequencial version of the algorithm is also available, so you can compare the parallel and the sequencial multiplicantion running time for speed-up and things like that.

- Compilation
  - make
- Run
  - `./mm_p -m -t [-d][-v][-h]` (parallel version)
  - `./mm_seq -m [-v][-h]` (sequencial version)
