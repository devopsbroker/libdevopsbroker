/*
 * async.h - DevOpsBroker C header file for Linux AIO functionality
 *
 * Copyright (C) 2020 Edward Smith <edwardsmith@devopsbroker.org>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-40
 *
 * echo ORG_DEVOPSBROKER_IO_ASYNC | md5sum | cut -c 25-32
 *
 * There are two things to keep in mind when using the Linux AIO functionality:
 *
 *    1. A file must be opened with the O_DIRECT file status flag
 *       If you use f1207515_open() this will be done for you automatically
 *
 *    2. Linux AIO currently works best on a filesystem formatted with XFS
 *       The XFS filesystem natively supports multithreading which maximizes
 *       Linux AIO performance
 *
 *    3. Unless you are working at a very low-level with respect to writes
 *       (think a database), Linux AIO write functionality is useless. In
 *       order for the writes to succeed they *MUST* be in sizes of 512-byte
 *       increments. Any other size will fail
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_IO_ASYNC_H
#define ORG_DEVOPSBROKER_IO_ASYNC_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include <linux/aio_abi.h>

#include "file.h"

#include "../adt/queuebounded.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define ASYNC_AIOTICKET_MAXSIZE  32768

// ═════════════════════════════════ Typedefs ═════════════════════════════════

/*
 * AIO Commands
 *   - IOCB_CMD_PREAD     positioned read; corresponds to pread() system call
 *   - IOCB_CMD_PWRITE    positioned write; corresponds to pwrite() system call
 *   - IOCB_CMD_FSYNC     sync file data and metadata with disk; corresponds to
 *                        fsync() system call
 *   - IOCB_CMD_FDSYNC    sync file data and metadata with disk, but only
 *                        metadata needed to access modified file data is
 *                        written; corresponds to fdatasync() system call
 *   - IOCB_CMD_POLL
 *   - IOCB_CMD_PREADV    vectored positioned read, sometimes called "scattered
 *                        input"; corresponds to pread() system call
 *   - IOCB_CMD_PWRITEV   vectored positioned write, sometimes called "gathered
 *                        output"; corresponds to pwrite() system call
 */
typedef enum AIOCommand {                                       // io_submit(2)
	AIO_READ = IOCB_CMD_PREAD,
	AIO_WRITE = IOCB_CMD_PWRITE,
	AIO_FSYNC = IOCB_CMD_FSYNC,
	AIO_FDSYNC = IOCB_CMD_FDSYNC,
//	AIO_POLL = IOCB_CMD_POLL,                                     Added in 4.18
	AIO_NOOP = IOCB_CMD_NOOP,
	AIO_VECT_READ = IOCB_CMD_PREADV,
	AIO_VECT_WRITE = IOCB_CMD_PWRITEV
} AIOCommand;

/*
 * AIO Read/Write Flags
 *   - RWF_APPEND     Append data to the end of the file; aio_offset is ignored
 *   - RWF_DSYNC      Write operations on the file will complete according to
 *                    the requirements of synchronized I/O data integrity
 *                    completion. Same as though each write(2) was followed by
 *                    a call to fdatasync(2).
 *   - RWF_HIPRI      High priority request, poll if possible
 *   - RWF_NOWAIT     Don't wait if the I/O will block for operations such as
 *                    file block allocations, dirty page flush, mutex locks,
 *                    or a congested block device inside the kernel.
 *   - RWF_SYNC       Write operations on the file will complete according to
 *                    the requirements of synchronized I/O file integrity
 *                    completion. Same as though each write(2) was followed by
 *                    a call to fsync(2).
 */
typedef enum AIOReadWriteFlag {                                 // io_submit(2)
//	AIORW_APPEND = RWF_APPEND,                                    Added in 4.16
	AIORW_DSYNC = RWF_DSYNC,
	AIORW_HIPRI = RWF_HIPRI,
	AIORW_NOWAIT = RWF_NOWAIT,
	AIORW_SYNC = RWF_SYNC
} AIOReadWriteFlag;

/*
 * AIO Request Flags
 *   - IOCB_FLAG_RESFD    Asynchronous I/O control must signal the file
 *                        descriptor mentioned in aio_resfd upon completion
 *   - IOCB_FLAG_IOPRIO   Interpret the aio_reqprio field as an IOPRIO_VALUE as
 *                        defined by linux/ioprio.h
 */
typedef enum AIORequestFlag {
	AIOREQ_RESFD = IOCB_FLAG_RESFD
//	AIOREQ_IOPRIO = IOCB_FLAG_IOPRIO                              Added in 4.18
} AIORequestFlag;

/*
 * I/O Control Block Structure
 *   uint64_t   aio_data;
 *     - copied into the data field of the io_event structure upon I/O completion
 *   uint32_t   PADDED(aio_key, aio_rw_flags);
 *     - aio_key is an internal field used by the kernel; do not modify
 *     - aio_rw_flags defines the R/W flags passed with structure
 *   uint16_t   aio_lio_opcode;
 *     - defines the type of I/O to be performed
 *   int16_t    aio_reqprio;
 *     - defines the requests priority
 *   uint32_t   aio_fildes;
 *     - file descriptor on which the I/O operation is to be performed
 *   uint64_t   aio_buf;
 *     - the buffer used to transfer data for a read or write operation
 *   uint64_t   aio_nbytes;
 *     - the size of the buffer pointed to by aio_buf
 *   int64_t    aio_offset;
 *     - the file offset at which the I/O operation is to be performed
 *   uint64_t   aio_reserved2;
 *   uint32_t   aio_flags;
 *     - the set of flags associated with the iocb structure
 *   uint32_t   aio_resfd;
 *     - The file descriptor to signal in the event of asynchronous I/O
 *       completion
 */
typedef struct iocb AIORequest;

static_assert(sizeof(AIORequest) == 64, "Check your assumptions");

/*
 * Asynchronous I/O Event Structure
 *   uint64_t   data;
 *     - the aio_data field from the AIORequest
 *   uint64_t   obj;
 *     - what AIORequest this event came from
 *   int64_t    res;
 *     - result code for this event
 *   int64_t    res2;
 *     - secondary result
 */
typedef struct io_event AIOEvent;

static_assert(sizeof(AIOEvent) == 32, "Check your assumptions");

/*
 * Amount of Time to Wait for Events
 *   time_t     tv_sec;
 *     - number of seconds to wait
 *   long       tv_nsec;
 *     - number of nanoseconds to wait [0 .. 999999999]
 */
typedef struct timespec WaitTime;

static_assert(sizeof(WaitTime) == 16, "Check your assumptions");

typedef struct AIOContext {
	aio_context_t id;
	QueueBounded *requestQueue;
	WaitTime      timeout;
	int64_t       numBytesRead;
	int64_t       numBytesWrite;
	uint32_t      maxOperations;
	uint32_t      numRequests;
	uint32_t      numReadRequests;
	uint32_t      numWriteRequests;
} AIOContext;

static_assert(sizeof(AIOContext) == 64, "Check your assumptions");

typedef struct AIOTicket {
	AIORequest *requestList[8];
	AIOEvent    eventList[8];
	uint32_t    numRequests;
	uint32_t    numEvents;
	int64_t     numBytesRead;
	int64_t     numBytesWrite;
} AIOTicket;

static_assert(sizeof(AIOTicket) == 344, "Check your assumptions");

typedef struct AIOFile {
	AIOTicket   aioTicket;
	AIOContext *aioContext;
	char*       fileName;
	int64_t     fileSize;
	int64_t     offset;
	int         fd;
} AIOFile;

static_assert(sizeof(AIOFile) == 384, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Acquire/Release Functions ~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_acquireAIORequest
 * Description: Acquires a AIORequest instance from the internal AIORequestPool
 *
 * Returns:     The AIORequest instance if available, NULL otherwise
 * ----------------------------------------------------------------------------
 */
AIORequest *f1207515_acquireAIORequest();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_releaseAIORequest
 * Description: Releases a AIORequest instance back into the internal AIORequestPool
 *
 * Parameters:
 *   aioRequest     The AIORequest instance to return to the AIORequestPool
 * ----------------------------------------------------------------------------
 */
void f1207515_releaseAIORequest(AIORequest *aioRequest);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_createAIOContext
 * Description: Creates an AIOContext struct instance
 *
 * Parameters:
 *   maxOperations  The maximum number of concurrent I/O operations to support
 * Returns:     An AIOContext struct instance, or NULL if error occurred
 * ----------------------------------------------------------------------------
 */
AIOContext *f1207515_createAIOContext(uint32_t maxOperations);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_destroyAIOContext
 * Description: Frees the memory allocated to the AIOContext struct pointer
 *
 * Parameters:
 *   aioContext     A pointer to the AIOContext instance to destroy
 * Returns:     Zero if the operation succeeded, SYSTEM_ERROR_CODE otherwise
 * ----------------------------------------------------------------------------
 */
int f1207515_destroyAIOContext(AIOContext *aioContext);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_cleanUpAIOContext
 * Description: Frees dynamically allocated memory within the AIOContext instance
 *
 * Parameters:
 *   aioContext     A pointer to the AIOContext instance to clean up
 * Returns:     Zero if the operation succeeded, SYSTEM_ERROR_CODE otherwise
 * ----------------------------------------------------------------------------
 */
int f1207515_cleanUpAIOContext(AIOContext *aioContext);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_initAIOContext
 * Description: Initializes an AIOContext struct
 *
 * Parameters:
 *   aioContext     A pointer to the AIOContext instance to initalize
 *   maxOperations  The maximum number of concurrent I/O operations to support
 * Returns:     Zero if the operation succeeded, SYSTEM_ERROR_CODE otherwise
 * ----------------------------------------------------------------------------
 */
int f1207515_initAIOContext(AIOContext *aioContext, uint32_t maxOperations);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_cleanUpAIOFile
 * Description: Closes the file descriptor within the AIOFile instance
 *
 * Parameters:
 *   aioFile    A pointer to the AIOFile instance to clean up
 * ----------------------------------------------------------------------------
 */
void f1207515_cleanUpAIOFile(AIOFile *aioFile);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_initAIOFile
 * Description: Initializes an AIOFile struct
 *
 * Parameters:
 *   aioContext     A pointer to the AIOContext instance
 *   aioFile        A pointer to the AIOFile instance to initalize
 *   fileName       The name of the file
 * ----------------------------------------------------------------------------
 */
void f1207515_initAIOFile(AIOContext *aioContext, AIOFile *aioFile, char *fileName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_cleanUpAIOTicket
 * Description: Cleans up an AIOTicket struct
 *
 * Parameters:
 *   aioTicket  A pointer to the AIOTicket instance to clean up
 * ----------------------------------------------------------------------------
 */
void f1207515_cleanUpAIOTicket(AIOTicket *aioTicket);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_initAIOTicket
 * Description: Initializes an AIOTicket struct
 *
 * Parameters:
 *   aioTicket  A pointer to the AIOTicket instance to initalize
 * ----------------------------------------------------------------------------
 */
void f1207515_initAIOTicket(AIOTicket *aioTicket);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_create
 * Description: Creates the file specified by pathname; file always created
 *              with O_DIRECT to enable Direct I/O
 *
 * Parameters:
 *   aioFile    The AIOFile instance to reference for the file name
 *   aMode      Open the file for write-only or read/write access
 *   flags      Specifies the creation mode and file status
 *   mode       The file permissions for the new file
 * Returns:     A file descriptor for the created file, or SYSTEM_ERROR_CODE
 * ----------------------------------------------------------------------------
 */
int f1207515_create(AIOFile *aioFile, FileAccessMode aMode, int flags, uint32_t mode);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_open
 * Description: Opens the file specified by pathname; file always opened with
 *              O_DIRECT to enable Direct I/O
 *
 * Parameters:
 *   aioFile    The AIOFile instance to reference for the file name
 *   aMode      Open the file for read-only, write-only, or read/write access
 *   flags      Zero or more file creation flags and file status flags
 * Returns:     The file descriptor for the opened file, or SYSTEM_ERROR_CODE
 * ----------------------------------------------------------------------------
 */
int f1207515_open(AIOFile *aioFile, FileAccessMode aMode, int flags);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_read
 * Description: Queues up an AIORequest instance to perform an AIO positioned
 *              read operation
 *
 * Parameters:
 *   aioFile    The AIOFile instance to read from
 *   buf        The data buffer to read into
 *   bufSize    The size of the data buffer
 * Returns:     The created AIORequest for the read operation, or NULL if
 *              AIOContext request queue is full
 * ----------------------------------------------------------------------------
 */
AIORequest *f1207515_read(AIOFile *aioFile, void *buf, size_t bufSize);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_write
 * Description: Queues up an AIORequest instance to perform an AIO positioned
 *              write operation
 *
 * Parameters:
 *   aioFile    The AIOFile instance to write to
 *   buf        The data buffer to write from
 *   count      The number of bytes to write from the buffer
 * Returns:     The created AIORequest for the write operation, or NULL if
 *              AIOContext request queue is full
 * ----------------------------------------------------------------------------
 */
AIORequest *f1207515_write(AIOFile *aioFile, void *buf, size_t count);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_submit
 * Description: Submits all queued AIORequest blocks for processing
 *
 * Parameters:
 *   aioFile    The AIOFile instance to submit I/O operations for
 * Returns:     True if the AIO submit operation succeeded, false otherwise
 * ----------------------------------------------------------------------------
 */
bool f1207515_submit(AIOFile *aioFile);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_getEvents
 * Description: Read asynchronous I/O events from the Linux completion queue
 *
 * Parameters:
 *   aioFile    The AIOFile instance to retrieve events for
 * Returns:     The number of received events
 * ----------------------------------------------------------------------------
 */
int32_t f1207515_getEvents(AIOFile *aioFile);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_printContext
 * Description: Prints the AIOContext information for debugging purposes
 *
 * Parameters:
 *   aioContext     The AIOContext instance to print
 * ----------------------------------------------------------------------------
 */
void f1207515_printContext(AIOContext *aioContext);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1207515_printTicket
 * Description: Prints the AIOTicket information for debugging purposes
 *
 * Parameters:
 *   aioTicket  The AIOTicket instance to print
 * ----------------------------------------------------------------------------
 */
void f1207515_printTicket(AIOTicket *aioTicket);

#endif /* ORG_DEVOPSBROKER_IO_ASYNC_H */
