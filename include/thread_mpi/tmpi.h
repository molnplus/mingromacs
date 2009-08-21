/*
This source code file is part of thread_mpi.  
Written by Sander Pronk, Erik Lindahl, and possibly others. 

Copyright (c) 2009, Sander Pronk, Erik Lindahl.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1) Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2) Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3) Neither the name of the copyright holders nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY US ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WE BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

If you want to redistribute modifications, please consider that
scientific software is very special. Version control is crucial -
bugs must be traceable. We will be happy to consider code for
inclusion in the official distribution, but derived work should not
be called official thread_mpi. Details are found in the README & COPYING
files.

To help us fund development, we humbly ask that you cite
any papers on the package - you can find them in the top README file.
*/

#ifndef _TMPI_H_
#define _TMPI_H_

/** \file 
 *
 * \brief Partial implementation of MPI using only threads. 
 * 
 * See the MPI specification at
 * http://www.mpi-forum.org/docs/docs.html
 * for an explanation of what these functions do.
 *
 * Because this is a thread-based library, be very careful with global
 * variables and static variables in functions: they will be shared across
 * all threads and lead to conflicts if not properly mutex-ed or barrier-ed
 * out.
 *
 * \sa http://www.mpi-forum.org/docs/docs.html for MPI documentation.
*/


#ifdef __cplusplus
extern "C" 
{  
#endif
#if 0
} /* Avoids screwing up auto-indentation */
#endif

/*#include "thread_mpi/threads.h"
#include "thread_mpi/atomic.h"*/


/** \brief tMPI Communicator
  
  Holds the group of processes to communicate
   with, and defines the scope for global operations such as broadcast. */
typedef struct tmpi_comm_ *tMPI_Comm;

/** \brief tMPI Group
  
  The group structure. Contains a list of threads. */
typedef struct tmpi_group_ *tMPI_Group;

/** \brief tMPI Request
  
  Request structure for holding data about non-blocking transfers. */
typedef struct tmpi_req_ *tMPI_Request;


/** \brief tMPI datatype

  tMPI data type structure. Holds info about datatypes. */
typedef struct tmpi_datatype_ *tMPI_Datatype;


/*! \name tMPI Data types 
    These are MPI data types as specified by the MPI standard. 
    Note that not all are available.  */
/*! \{ */
extern const tMPI_Datatype TMPI_CHAR;               /**< char */
extern const tMPI_Datatype TMPI_SHORT;              /**< short */
extern const tMPI_Datatype TMPI_INT;                /**< int */
extern const tMPI_Datatype TMPI_LONG;               /**< long */
#ifdef SIZEOF_LONG_LONG_INT
extern const tMPI_Datatype TMPI_LONG_LONG;          /**< long long */
extern const tMPI_Datatype TMPI_LONG_LONG_INT;      /**< long long int */
#endif
extern const tMPI_Datatype TMPI_SIGNED_CHAR;        /**< signed char */
extern const tMPI_Datatype TMPI_UNSIGNED_CHAR;      /**< unsigned char */
extern const tMPI_Datatype TMPI_UNSIGNED_SHORT;     /**< unsigned short */
extern const tMPI_Datatype TMPI_UNSIGNED;           /**< unsigned int */
extern const tMPI_Datatype TMPI_UNSIGNED_LONG;      /**< unsigned long */
#ifdef SIZEOF_LONG_LONG_INT
extern const tMPI_Datatype TMPI_UNSIGNED_LONG_LONG; /**< unsigned long long */
#endif
extern const tMPI_Datatype TMPI_FLOAT;              /**< float */
extern const tMPI_Datatype TMPI_DOUBLE;             /**< double */
extern const tMPI_Datatype TMPI_LONG_DOUBLE;        /**< long double */
/*extern tMPI_Datatype tMPI_UNSIGNED_WCHAR */
extern const tMPI_Datatype TMPI_BYTE;       /**< byte (for binary xmissions) */
/*! \} */


/** \brief Error codes */
enum
{
    TMPI_SUCCESS=0,                 /*!< No error */
    TMPI_ERR_MALLOC,                /*!< Out of memory */
    TMPI_ERR_INIT,                  /*!< Initialization error */ 
    TMPI_ERR_FINALIZE,
    TMPI_ERR_GROUP,
    TMPI_ERR_COMM,
    TMPI_ERR_STATUS,
    TMPI_ERR_GROUP_RANK,
    TMPI_ERR_DIMS,
    TMPI_ERR_COORDS,
    TMPI_ERR_CART_CREATE_NPROCS,
    TMPI_ERR_XFER_COUNTERPART,
    TMPI_ERR_XFER_BUFSIZE,
    TMPI_ERR_XFER_BUF_OVERLAP,
    TMPI_ERR_SEND_DEST,
    TMPI_ERR_RECV_SRC,
    TMPI_ERR_BUF,
    TMPI_ERR_MULTI_MISMATCH,
    TMPI_ERR_OP_FN,
    TMPI_ERR_ENVELOPES,
    TMPI_ERR_REQUESTS,
    TMPI_FAILURE,
    TMPI_ERR_UNKNOWN,
    N_TMPI_ERR  /* this must be the last one */
};

/*! \brief Maximum length of error string for tMPI_Error_string() */
#define TMPI_MAX_ERROR_STRING            256

/*! \brief default code for undefined value, 
  
    For example for undefined color in tMPI_Split(). */
#define TMPI_UNDEFINED -1

/*! \brief error handler function */
typedef void (*tMPI_Errhandler_fn)(tMPI_Comm*, int*);
/*! \brief error handler object */
typedef struct tmpi_errhandler_ *tMPI_Errhandler;

/*! \brief pre-defined error handler that abort()s on every error */
extern tMPI_Errhandler TMPI_ERRORS_ARE_FATAL;
/*! \brief pre-defined error handler that tries to continue on every error */
extern tMPI_Errhandler TMPI_ERRORS_RETURN;


/** \brief Source number wildcard so tMPI_Recv(), etc. can receive from 
           any source. */
#define TMPI_ANY_SOURCE -1
/** \brief Tag number wildcard so tMPI_Recv(), etc. can receive messages with 
           any tag. */
#define TMPI_ANY_TAG -1

/** \brief Return code for Cartesian topology with tMPI_Topo_test().  */
#define TMPI_CART 1
/** \brief Return code for graph topology with tMPI_Topo_test().  */
#define TMPI_GRAPH 2


/** \brief Pre-initialized communicator with all available threads. */
extern tMPI_Comm TMPI_COMM_WORLD;


/** \brief A pre-defined NULL communicator to compare against, to check comm 
           validity */
#define TMPI_COMM_NULL 0
/** \brief A pre-defined NULL group to compare against, to check group 
           validity */
#define TMPI_GROUP_NULL 0

/** \brief the empty group */
extern tMPI_Group tMPI_GROUP_EMPTY;


/** \brief The maximum processor name returned using tMPI_Get_processor_name(). */
#define TMPI_MAX_PROCESSOR_NAME 128


/** \brief Used as NULL status for tMPI_Recv(), etc. */
#define TMPI_STATUS_IGNORE 0
/** \brief Used as NULL status list for tMPI_Waitall(), etc. */
#define TMPI_STATUSES_IGNORE 0

/** \brief tMPI Status.
  
  Holds status info (tag, sender, amount of data transmitted) for receives. 
  The status object is user-maintained. */
typedef struct tmpi_status_
{
    int TMPI_SOURCE;        /**< Message source rank. */
    int TMPI_TAG;           /**< Message source tag. */
    int TMPI_ERROR;         /**< Message error. */
    size_t transferred;     /**< Number of transferred bytes */
} tMPI_Status;
/*typedef struct tmpi_status_ tMPI_Status;*/

/** \brief NULL request */
#define TMPI_REQUEST_NULL 0

/** \brief collective communication special to signify that the send 
           buffer is to function as receive buffer. 
           
           Used, for example in tMPI_Reduce. */
#define TMPI_IN_PLACE 0


/** \brief tMPI_Reduce operators.

    These all work (except obviously bad combinations like bitwise
    and/or/xor on floats, etc): */
typedef enum
{
    TMPI_MAX,       /**< calculate maximum value */
    TMPI_MIN,       /**< calculate minimum value */
    TMPI_SUM,       /**< calculate sum */
    TMPI_PROD,      /**< calculate product */
    TMPI_LAND,      /**< calculate logical and */
    TMPI_BAND,      /**< calculate binary and */
    TMPI_LOR,       /**< calculate logical or */
    TMPI_BOR,       /**< calculate binary or */
    TMPI_LXOR,      /**< calculate logical xor */
    TMPI_BXOR       /**< calculate binary xor */
} tMPI_Op;

#ifndef DOXYGEN
/* function to obtain tMPI_COMM_SELF */
tMPI_Comm tMPI_Get_comm_self(void);
#endif
/** \brief The thread-specific comm containing only the thread itself.

    \hideinitializer
    \return the self comm object associated with the thread. */
#define TMPI_COMM_SELF (tMPI_Get_comm_self())







/* functions: */

/*! \name Initialization and exit functions 
    \{ */
/** \brief Traditional MPI initializer; spawns threads that start at main(). 
  
    Seeks the argument '-np n', where n is the number of 
    threads that will be created. These new threads then run main() again,
    with the original argc and argv. 
    
    \param[in] argc     argc of original main() invocation, or NULL
    \param[in] argv     argv of original main() invocation, or NULL. 

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Init(int *argc, char ***argv);


/** \brief Alternate thread MPI intializer and thread spawner.
  
    Creates N threads (including main thread) 
    that run the function start_function, which takes a void* argument, 
    given by arg. The function start_function also gets called by the main
    thread. When the function start_function returns it, will behave 
    as if tMPI_Finalize is called, and if it's a sub-thread it will
    stop running. 
    
    \param[in] N                The number of threads to start.
    \param[in] start_function   The function to start threads at (including 
                                main thread). 
    \param[in] arg              An optional argument for start_function().

    \return  TMPI_FAILURE on failure, TMPI_SUCCESS on succes, after all
             threads have finished.
    */
int tMPI_Init_fn(int N, void (*start_function)(void*), void *arg);

/** \brief get the number of threads from the command line
  
    can be called before tMPI_Init() 

    \param[in]  argc            argc from main()
    \param[in]  argv            argv from main()
    
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Get_N(int *argc, char ***argv);



/** \brief Waits for all other threads to finish and cleans up 

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Finalize(void);


/** \brief Just kills all threads. 
  
    Not really neccesary because exit() would do that for us anyway.

    \param[in] comm         Comm to kill threads for
    \param[in] errorcode    Error code to exit with
    
    \return Never returns. */
int tMPI_Abort(tMPI_Comm comm, int errorcode);

/** \brief whether tMPI_Init, but not yet tMPI_Finalize, has been run

    \param[out] flag     Set to TRUE if tMPI_Init() has been called, 
                         FALSE if not.
    
    \return     always returns TMPI_SUCCESS. */
int tMPI_Initialized(int *flag);

/** \brief Determine whether tMPI_Finalize has been run.

    \param[out] flag        Set to TRUE if tMPI_Finalize() has been 
                            called, FALSE if not.
     
    \return     always returns TMPI_SUCCESS.  */
int tMPI_Finalized(int *flag);
/** \} */









/*! \name Error handling functions
    \{ */
/** \brief Create an error handler object from a function.

    \param[in]  function        The function to make an error handler of.
    \param[out] errhandler      The error handler.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Create_errhandler(tMPI_Errhandler_fn *function,
                           tMPI_Errhandler *errhandler);


/** \brief Free the error handler object.
    
    \param[in] errhandler       The error handler.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Errhandler_free(tMPI_Errhandler *errhandler);

/** \brief Set the error handler.

    \param[in] comm         the communicator to set the error handler for.
    \param[in] errhandler   the error handler.
    
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */

int tMPI_Comm_set_errhandler(tMPI_Comm comm, tMPI_Errhandler errhandler);
/** \brief get the error handler.

    Gets the error handler associated with a comm

    \param[in]  comm         the communicator to get the error handler for.
    \param[out] errhandler   the error handler.
    
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */

int tMPI_Comm_get_errhandler(tMPI_Comm comm, tMPI_Errhandler *errhandler);

/*! \brief get the error string associated with an error code. 
  
    The length of the error string will never exceed TMPI_MAX_ERROR_STRING.

    \param[in]  errorcode   The error code.
    \param[out] string      The pre-allocated char pointer to output to.
    \param[out] resultlen   The length of the error string. Will 
                            never be longer than TMPI_MAX_ERROR_STRING.  

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Error_string(int errorcode, char *string, size_t *resultlen);
/** \} */








/*! \name Environment query functions
    \{ */
/** \brief returns string with thread number.

    \param[out] name        Pre-allocated string to output name to (will not 
                            be longer than TMPI_MAX_PROCESSOR_NAME). 
    \param[out] resultlen   The length of the output.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Get_processor_name(char *name, size_t *resultlen);

/** \brief get a time value as a double, in seconds.

    \return time value.
    */
double tMPI_Wtime(void);
#if 0
/** get the resolution of tMPI_Wtime as a double, in seconds */
double tMPI_Wtick(void);
#endif

#ifndef DOXYGEN
#define tMPI_This_threadnr() (int)(tMPI_Get_current() - threads)
#else
/** \brief Get the thread number of this thread. 
    Mostly for debugging.

    \return the global thread number. */
int tMPI_This_Threadnr(void);
#endif

/** \} */









/*! \name tMPI_Group functions
    \{ */
/** \brief Get the size (number of members) of a group.  

    \param[in]  group       The group.
    \param[out] size        Size.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Group_size(tMPI_Group group, int *size);

/** \brief Get the rank of a thread in a group 

    \param[in]  group       The group.
    \param[out] rank        Variable for the rank, or TMPI_UNDEFINED 
                            if not in group.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Group_rank(tMPI_Group group, int *rank);

/** \brief Create a new group as a the collection of threads with given ranks. 

    \param[in] group        The group from which the ranks are taken.
    \param[in] n            The number of new group members.
    \param[in] ranks        The ranks of the threads to add to the new group.
    \param[out] newgroup    The new group.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Group_incl(tMPI_Group group, int n, int *ranks, tMPI_Group *newgroup);

/** \brief Get a pointer to the group in the comm. 
    
    \param[in] comm         The comm from which to take the group.
    \param[out] group       The comm's group. 

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_group(tMPI_Comm comm, tMPI_Group *group);

/** \brief De-allocate a group 

    \param[in] group        The group to de-allocate.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Group_free(tMPI_Group *group);
/*! \} */







/*! \name tMPI_Comm functions
    \{ */
/** \brief Get the comm size (nr. of threads).
    
    \param[in] comm         The comm to query.
    \param[out] size        The comm size.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_size(tMPI_Comm comm, int *size);

/** \brief get the rank in comm of the current process 

    \param[in]  comm        The comm to query.
    \param[out] rank        Thread rank in comm.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_rank(tMPI_Comm comm, int *rank);

/** \brief De-allocate a comm 

    Collective function.

    \param[in] comm         The comm to free.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_free(tMPI_Comm *comm);

/** \brief Create a comm based on group membership.

    Collective function that creates a new comm containing only proceses 
    that are members of the given group.

    \param[in]  comm        The originating comm.
    \param[in]  group       The group of threads to create a comm from.
    \param[out] newcomm     The new comm.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_create(tMPI_Comm comm, tMPI_Group group, tMPI_Comm *newcomm);

/** \brief Split up a group into same-colored sub-groups ordered by key.

    This is the main comm creation function: it's a collective call that takes
    a color and a key from each process, and arranges all threads that
    call tMPI_Split() withe the same color together into a comm. 

    The rank in the new group will be based on the value given in key. 

    Passing TMPI_UNDEFINED as a color will result in the thread not being
    part of any group, and getting TMPI_COMM_NULL back in newcomm.

    \param[in]  comm        The originating comm.
    \param[in]  color       This thread's color (determines which comm it will 
                            be in). Giving TMPI_UNDEFINED will result in 
                            this thread not being in any group.
    \param[in]  key         This thread's key (determines rank).
    \param[out] newcomm     The new comm.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_split(tMPI_Comm comm, int color, int key, tMPI_Comm *newcomm);

/** \brief Make a duplicate of a comm.

    Collective function.

    \param[in] comm         The originating comm.
    \param[in] newcomm      The new comm.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Comm_dup(tMPI_Comm comm, tMPI_Comm *newcomm);
/*! \} */








/*! \name Topology functions
    \{ */
/* topology functions */
/** \brief Check what type of topology the comm has. 

    \param[in] comm         The comm to query
    \param[out] status      The type of topology. 
    
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Topo_test(tMPI_Comm comm, int *status);

/** \brief Get the dimensionality of a comm with a topology.

    \param[in] comm         The comm to query.
    \param[out] ndims       The number of dimensions.
    
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */

int tMPI_Cartdim_get(tMPI_Comm comm, int *ndims);
/** \brief Get the size and pbc a of a comm with a Cartesian topology has.

    \param[in]  comm        The comm to query.
    \param[in]  maxdims     The maximum number of dimensions in the periods 
                            and coords parameter.
    \param[out] dims        The number of dimensions.
    \param[out] periods     The periodicity in each dimension.
    \param[out] coords      The number of coordinates in each dimension.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */

int tMPI_Cart_get(tMPI_Comm comm, int maxdims, int *dims, int *periods, 
                 int *coords);


/** \brief Get rank that a specific set of process coordinates has in 
    a Cartesian topology.

    \param[in]  comm        The comm to query.
    \param[in]  coords      The coordinates in each dimension.
    \param[out] rank        The rank associated with the coordinates.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Cart_rank(tMPI_Comm comm, int *coords, int *rank);

/** \brief Get coordinates of a process rank in a Cartesian topology.

    \param[in]  comm        The comm to query.
    \param[in]  rank        The rank associated with the coordinates.
    \param[in]  maxdims     The maximum number of dimensions in the coords     
                            parameter.
    \param[out] coords      The coordinates in each dimension.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Cart_coords(tMPI_Comm comm, int rank, int maxdims, int *coords);

/** \brief Get optimal rank this process would have in a Cartesian topology. 

    \param[in]  comm        The comm to query.
    \param[in]  ndims       The number of dimensions.
    \param[in]  dims        The size in each dimension.
    \param[in]  periods     The periodicity in each dimension.

    \param[out] newrank     The rank the thread would have given the topology.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Cart_map(tMPI_Comm comm, int ndims, int *dims, int *periods, 
                         int *newrank);

/** \brief Create a comm with a Cartesian topology.

    \param[in]  comm_old    The originating comm.
    \param[in]  ndims       The number of dimensions.
    \param[in]  dims        The size in each dimension.
    \param[in]  periods     The periodicity in each dimension.
    \param[in]  reorder     Whether to allow reordering of the threads 
                            according to tMPI_Cart_map().
    \param[out] comm_cart   The new comm with Cartesian topology.
   
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Cart_create(tMPI_Comm comm_old, int ndims, int *dims, int *periods, 
                    int reorder, tMPI_Comm *comm_cart);
/*! \} */








/*! \name Data type manipulation functions
    \{ */
/** \brief Create a contiguous data type (the only type possible right now).

    Creates a datatype that is a vector of oldtype.

    \param[in]  count       The number of oldtype types in the new type.
    \param[in]  oldtype     The old data type.
    \param[out] newtype     The new data type (still needs to be committed).
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Type_contiguous(int count, tMPI_Datatype oldtype, 
                        tMPI_Datatype *newtype);


/** \brief Make a data type ready for use.

    \param[in,out] datatype  The new datatype.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Type_commit(tMPI_Datatype *datatype);
/*! \} */








/*! \name Point-to-point communication functions
    \{ */

/* blocking transfers. The actual transfer (copy) is done on the receiving end 
    (so that the receiver's cache already contains the data that it presumably
     will use soon).  */
/** \brief Send message; blocks until buf is reusable. 

    \param[in]  buf         The buffer with data to send.
    \param[in]  count       The number of items to send.
    \param[in]  datatype    The data type of the items in buf.
    \param[in]  dest        The rank of the destination thread. 
    \param[in]  tag         The message tag. 
    \param[in]  comm        The shared communicator.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Send(void* buf, int count, tMPI_Datatype datatype, int dest, 
             int tag, tMPI_Comm comm);

/** \brief Receive message; blocks until buf is filled.

    \param[out] buf         The buffer for data to receive.
    \param[in]  count       The maximum number of items to receive.
    \param[in]  datatype    The data type of the items in buf.
    \param[in]  source      The rank of the source thread (or TMPI_ANY_SOURCE). 
    \param[in]  tag         The message tag (or TMPI_ANY_TAG). 
    \param[in]  comm        The shared communicator.
    \param[out] status      The message status. 
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Recv(void* buf, int count, tMPI_Datatype datatype, int source, 
             int tag, tMPI_Comm comm, tMPI_Status *status);

/** \brief Send & receive message at the same time. 
    
    Blocks until recvbuf is filled, and sendbuf is ready for reuse.

    \param[in]  sendbuf     The buffer with data to send.
    \param[in]  sendcount   The number of items to send.
    \param[in]  sendtype    The data type of the items in send buf.
    \param[in]  dest        The rank of the destination thread. 
    \param[in]  sendtag     The send message tag. 
    \param[out] recvbuf     The buffer for data to receive.
    \param[in]  recvcount   The maximum number of items to receive.
    \param[in]  recvtype    The data type of the items in recvbuf.
    \param[in]  source      The rank of the source thread (or TMPI_ANY_SOURCE). 
    \param[in]  recvtag     The recveive message tag (or TMPI_ANY_TAG). 
    \param[in]  comm        The shared communicator.
    \param[out] status      The received message status. 
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Sendrecv(void *sendbuf, int sendcount, tMPI_Datatype sendtype, 
                 int dest, int sendtag, void *recvbuf, int recvcount, 
                 tMPI_Datatype recvtype, int source, int recvtag, 
                 tMPI_Comm comm, tMPI_Status *status);

/* async send/recv. The actual transfer is done on the receiving 
    end, during tMPI_Wait, tMPI_Waitall or tMPI_Test. For tMPI_Waitall, 
    the incoming messages are processed in the order they come in.  */

/** \brief Initiate sending a message, non-blocking.

    This makes the buffer available to be received. The contents of buf
    should not be touched before the transmission is finished with 
    tMPI_Wait(), tMPI_Test() or tMPI_Waitall(). 


    \param[in]  buf         The buffer with data to send.
    \param[in]  count       The number of items to send.
    \param[in]  datatype    The data type of the items in buf.
    \param[in]  dest        The rank of the destination thread. 
    \param[in]  tag         The message tag. 
    \param[in]  comm        The shared communicator.
    \param[out] request     The request object that can be used in tMPI_Wait(),
                            tMPI_Test, etc.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Isend(void* buf, int count, tMPI_Datatype datatype, int dest, 
              int tag, tMPI_Comm comm, tMPI_Request *request);

/** \brief Initiate receiving a message.

    This makes the buffer available to be filled with data. The contents of 
    buf should not be relied on before the transmission is finished with 
    tMPI_Wait(), tMPI_Test() or tMPI_Waitall(). 

    \param[out] buf         The buffer for data to receive.
    \param[in]  count       The maximum number of items to receive.
    \param[in]  datatype    The data type of the items in buf.
    \param[in]  source      The rank of the source thread (or TMPI_ANY_SOURCE). 
    \param[in]  tag         The message tag (or TMPI_ANY_TAG). 
    \param[in]  comm        The shared communicator.
    \param[out] request     The request object that can be used in tMPI_Wait(),
                            tMPI_Test, etc.
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Irecv(void* buf, int count, tMPI_Datatype datatype, int source, 
              int tag, tMPI_Comm comm, tMPI_Request *request);

/** \brief Test whether a message is transferred. 
    
    \param[in,out]  request The request obtained wit tMPI_Isend()/tMPI_Irecv().
    \param[out]     flag    A flag set to TRUE(1) if the request is finished,
                            FALSE(0) otherwise.
    \param[out]     status  Message status (can be set to TMPI_STATUS_IGNORE).

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Test(tMPI_Request *request, int *flag, tMPI_Status *status);

/** \brief Wait until a message is transferred.
    
    \param[in,out]  request The request obtained wit tMPI_Isend()/tMPI_Irecv().
    \param[out]     status  Message status.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Wait(tMPI_Request *request, tMPI_Status *status);

/** \brief Wait until several messages are transferred. 
   
    \param[in]      count               The number of requests
    \param[in,out]  array_of_requests   List of count requests obtained with
                                        tMPI_Isend()/tMPI_Irecv().
    \param[out]     array_of_statuses   List of count message statuses (can 
                                        be set to TMPI_STATUSES_IGNORE).

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Waitall(int count, tMPI_Request *array_of_requests, 
                tMPI_Status *array_of_statuses);


/** \brief get the number of actually transferred items from a receive 
    status. 
    
    \param[in]  status      The status. 
    \param[in]  datatype    The data type which was received. 
    \param[out] count       The number of items actually received.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Get_count(tMPI_Status *status, tMPI_Datatype datatype, int *count);
/*! \} */








/*! \name Synchronization functions
    \{ */
/** \brief Block until all threads in the comm call this function. 

    \param[in]  comm    The comm object.
  
    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Barrier(tMPI_Comm comm);
/*! \} */







/*! \name Multicast communication functions
    \{ */
/** \brief Broadcast from one thread to all others in comm.

    Collective function; data is transferred from root's buffer to all others'
    buffer. 

    \param[in,out]  buffer      The buffer to send from (root)/receive from 
                                (other threads).
    \param[in]      count       The number of items to send/receive.
    \param[in]      datatype    The type of the items to send/receive.
    \param[in]      root        The rank of the sending thread.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Bcast(void* buffer, int count, tMPI_Datatype datatype, int root, 
              tMPI_Comm comm);

/** \brief Gather data from all threads in comm to root. 

    Collective function; assumes that all data is received in blocks of 
    recvcount. 

    \param[in]      sendbuf     The send buffer for all threads (root may
                                specify TMPI_IN_PLACE, in which case it 
                                transfers nothing to itself).
    \param[in]      sendcount   The number of items to send for all threads. 
    \param[in]      sendtype    The type of the items to send.
    \param[out]     recvbuf     The receiving buffer (for root thread).
    \param[in]      recvcount   The number of items to receive (for root).
    \param[in]      recvtype    The type of the items to receive (for root).
    \param[in]      root        The rank of root.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Gather(void* sendbuf, int sendcount, tMPI_Datatype sendtype, 
               void* recvbuf, int recvcount, tMPI_Datatype recvtype, int root, 
               tMPI_Comm comm);


/** \brief Gather irregularly laid out data from all processes in comm to root. 

    Collective function.

    \param[in]      sendbuf     The send buffer for all threads (root may
                                specify TMPI_IN_PLACE, in which case it 
                                transfers nothing to itself).
    \param[in]      sendcount   The number of items to send for all threads. 
    \param[in]      sendtype    The type of the items to send.
    \param[out]     recvbuf     The receiving buffer (for root thread).
    \param[in]      recvcounts  The list of number of items to receive (for 
                                root).
    \param[in]      displs      The list of displacements in recvbuf to 
                                receive data in (for root).
    \param[in]      recvtype    The type of the items to receive (for root).
    \param[in]      root        The rank of root.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Gatherv(void* sendbuf, int sendcount, tMPI_Datatype sendtype, 
                void* recvbuf, int *recvcounts, int *displs, 
                tMPI_Datatype recvtype, int root, tMPI_Comm comm);


/** \brief Spread parts of sendbuf to all processes in comm from root.

    Collective function.

    \param[in]      sendbuf     The send buffer for root.
    \param[in]      sendcount   The number of items for root to send to each
                                thread.
    \param[in]      sendtype    The type of the items root sends.
    \param[out]     recvbuf     The receiving buffer for all receiving threads 
                                (root may specify TMPI_IN_PLACE, in which case
                                it transmits nothing to itself).
    \param[in]      recvcount   The number of items recvbuf can receive. 
    \param[in]      recvtype    The type of items to receive.
    \param[in]      root        The rank of root.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Scatter(void* sendbuf, int sendcount, tMPI_Datatype sendtype, 
                void* recvbuf, int recvcount, tMPI_Datatype recvtype, int root, 
                tMPI_Comm comm);

/** \brief Spread irregularly laid out parts of sendbuf to all processes 
            in comm from root.

    Collective function.

    \param[in]      sendbuf     The send buffer for root.
    \param[in]      sendcounts  List of the number of items for root to send 
                                to each thread.
    \param[in]      displs      List of displacements in sendbuf from which 
                                to start transmission to each thread.
    \param[in]      sendtype    The type of the items root sends.
    \param[out]     recvbuf     The receiving buffer for all receiving threads 
                                (root may specify TMPI_IN_PLACE, in which case
                                it transmits nothing to itself).
    \param[in]      recvcount   The number of items recvbuf can receive. 
    \param[in]      recvtype    The type of items to receive.
    \param[in]      root        The rank of root.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, 
                 tMPI_Datatype sendtype, void* recvbuf, int recvcount, 
                 tMPI_Datatype recvtype, int root, tMPI_Comm comm); 


/** \brief Spread out parts of sendbuf to all processes from all processes in 
           comm.

    Collective function.

    \param[in]      sendbuf     The send buffer. 
    \param[in]      sendcount   The number of items for to send to each thread.
    \param[in]      sendtype    The type of the items to send.
    \param[out]     recvbuf     The receive buffer for all threads.
    \param[in]      recvcount   The number of items recvbuf can receive per 
                                thread. 
    \param[in]      recvtype    The type of items to receive.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Alltoall(void* sendbuf, int sendcount, tMPI_Datatype sendtype, 
                  void* recvbuf, int recvcount, tMPI_Datatype recvtype, 
                  tMPI_Comm comm);


/** \brief Spread out irregularly laid out parts of sendbuf to all 
           processes from all processes in comm.

    Collective function.

    \param[in]      sendbuf     The send buffer. 
    \param[in]      sendcounts  List of the number of items for to send to 
                                each thread.
    \param[in]      sdispls     List of the displacements in sendbuf of items 
                                to send to each thread.
    \param[in]      sendtype    The type of the items to send.
    \param[out]     recvbuf     The receive buffer for all threads.
    \param[in]      recvcounts  List of the number of items recvbuf can 
                                receive from each thread. 
    \param[in]      rdispls     List of the displacements in recvbuf of items 
                                to receive from each thread.
    \param[in]      recvtype    The type of items to receive.
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Alltoallv(void* sendbuf, int *sendcounts, int *sdispls, 
                  tMPI_Datatype sendtype, void* recvbuf, int *recvcounts, 
                  int *rdispls, tMPI_Datatype recvtype, tMPI_Comm comm);

/*! \} */









/*! \name Reduce functions
    \{ */
/**\brief Do an operation between all locally held buffers on all items in the 
          buffers, and send the results to root.

    Collective function.
   
    \param[in]  sendbuf     The operand parameters.
    \param[out] recvbuf     The result buffer at root.
    \param[in]  count       The number of items to do operation on.
    \param[in]  datatype    The data type of the items.
    \param[in]  op          The operation to perform.
    \param[in]  root        The root thread (which is to receive the results).
    \param[in]  comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Reduce(void* sendbuf, void* recvbuf, int count, 
               tMPI_Datatype datatype, tMPI_Op op, int root, tMPI_Comm comm);



/** \brief Do an operation between all locally held buffers on all items in the 
    buffers and broadcast the results. 

    Collective function.
   
    \param[in]  sendbuf     The operand parameters.
    \param[out] recvbuf     The result buffer at root.
    \param[in]  count       The number of items to do operation on.
    \param[in]  datatype    The data type of the items.
    \param[in]  op          The operation to perform.
    \param[in]  comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Allreduce(void* sendbuf, void* recvbuf, int count, 
                  tMPI_Datatype datatype, tMPI_Op op, tMPI_Comm comm);

/** \brief Do an tMPI_Reduce, but with the following assumption:
    recvbuf points to a valid buffer in all calling threads, or 
    sendbuf has the value TMPI_IN_PLACE (in which case the values of 
    sendbuf may be changed in that thread).  

    This avoids unnecesary memory allocations associated with the normal
    tMPI_Reduce. 

    Collective call. 
    

    \param[in,out]  sendbuf     The operand parameters.
    \param[out]     recvbuf     The result buffer (or TMPI_IN_PLACE).
    \param[in]      count       The number of items to do operation on.
    \param[in]      datatype    The data type of the items.
    \param[in]      op          The operation to perform.
    \param[in]      root        The root thread (which is to receive 
                                the final results).
    \param[in]      comm        The communicator.

    \return  TMPI_SUCCESS on success, TMPI_FAILURE on failure.  */
int tMPI_Reduce_fast(void* sendbuf, void* recvbuf, int count, 
                     tMPI_Datatype datatype, tMPI_Op op, int root, 
                     tMPI_Comm comm);
/*! \} */



#ifdef __cplusplus
} /* closing extern "C" */
#endif

#endif /* _TMPI_H_ */