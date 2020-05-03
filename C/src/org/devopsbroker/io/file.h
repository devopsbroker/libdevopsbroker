/*
 * file.h - DevOpsBroker C header file for providing file-handling functionality
 *
 * Copyright (C) 2018-2020 Edward Smith <edwardsmith@devopsbroker.org>
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
 * Developed on Ubuntu 16.04.5 LTS running kernel.osrelease = 4.15.0-34
 *
 * echo ORG_DEVOPSBROKER_IO_FILE | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_IO_FILE_H
#define ORG_DEVOPSBROKER_IO_FILE_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdbool.h>
#include <stdint.h>

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define END_OF_FILE 0
#define START_OF_HEADING 1

#define LOGICAL_BLOCK_SIZE 512
#define PHYSICAL_BLOCK_SIZE 4096

// ═════════════════════════════════ Typedefs ═════════════════════════════════

/*
 * File Stat Structure                                            // sys/stat.h
 *   dev_t      st_dev;
 *     - ID of device containing file
 *   ino_t      st_ino;
 *     - inode number
 *   mode_t     st_mode;
 *     - file type and mode
 *   nlink_t    st_nlink;
 *     - number of hard links
 *   uid_t      st_uid;
 *     - user ID of owner
 *   gid_t      st_gid;
 *     - group ID of owner
 *   dev_t      st_rdev;
 *     - device ID (if special file)
 *   off_t      st_size;
 *     - total size in bytes
 *   blksize_t  st_blksize;
 *     - block size for filesystem I/O
 *   blkcnt_t   st_blocks;
 *     - number of 512B blocks allocated
 *   struct timespec st_atim;
 *     - time of last access, nanosecond precision
 *   struct timespec st_mtim;
 *     - time of last modification, nanosecond precision
 *   struct timespec st_ctim;
 *     - time of last status change, nanosecond precision
 */
typedef struct stat FileStatus;

static_assert(sizeof(FileStatus) == 144, "Check your assumptions");

/*
 * File Access Modes
 *   - O_RDONLY       Open file as read-only
 *   - O_RDWR         Open file as read-write
 *   - O_WRONLY       Open file as write-only
 */
typedef enum FileAccessMode {                                       // open(2)
	FOPEN_READONLY = O_RDONLY,
	FOPEN_READWRITE = O_RDWR,
	FOPEN_WRITEONLY = O_WRONLY
} FileAccessMode;

/*
 * File Creation Flags
 *   - O_CLOEXEC      Enable the close-on-exec flag for the new file descriptor
 *   - O_CREAT        If pathname does not exist, create it as a regular file
 *   - O_DIRECTORY    If pathname is not a directory, cause the open to fail
 *   - O_EXCL         Ensure that this call creates the file; used with O_CREAT
 *   - O_NOCTTY       If pathname refers to a terminal device, don't become
 *                    process's controlling terminal
 *   - O_NOFOLLOW     If pathname is a symbolic link, then open fails with the
 *                    error ELOOP
 *   - O_TMPFILE      Create an unnamed temporary regular file
 *   - O_TRUNC        If the file already exists and is a regular file and the
 *                    access mode allows writing (i.e., is O_RDWR or O_WRONLY)
 *                    it will be truncated to length 0
 */
typedef enum FileCreationFlag {                                     // open(2)
	FCREAT_CLOEXEC = O_CLOEXEC,
	FCREAT_CREATE = O_CREAT,
	FCREAT_DIRECTORY = O_DIRECTORY,
	FCREAT_FAIL_ON_EXIST = O_EXCL,
	FCREAT_NOCTTY = O_NOCTTY,
	FCREAT_NOFOLLOW = O_NOFOLLOW,
	FCREAT_TMPFILE = O_TMPFILE,
	FCREAT_TRUNC = O_TRUNC
} FileCreationFlag;

/*
 * File Status Flags
 *   - O_APPEND       The file is opened in append mode
 *   - O_ASYNC        Enable signal-driven I/O; only available for terminals,
 *                    pseudoterminals, sockets, pipes and FIFOs
 *   - O_DIRECT       Minimize cache effects of the I/O to and from this file.
 *                    File I/O is done directly to/from user-space buffers. The
 *                    O_DIRECT flag on its own makes an effort to transfer data
 *                    synchronously, but does not give the guarantees of the
 *                    O_SYNC flag that data and necessary metadata are
 *                    transferred. To guarantee synchronous I/O, O_SYNC must be
 *                    used in addition to O_DIRECT.
 *   - O_DSYNC        Write operations on the file will complete according to
 *                    the requirements of synchronized I/O data integrity
 *                    completion. Same as though each write(2) was followed by
 *                    a call to fdatasync(2).
 *   - O_LARGEFILE    Allow files whose sizes cannot be represented in an off_t
 *                    (but can be represented in an off64_t) to be opened. The
 *                    _LARGEFILE64_SOURCE macro must be defined (before
 *                    including any header files) in order to obtain this
 *                    definition. Setting the _FILE_OFFSET_BITS feature test
 *                    macro to 64 (rather than using O_LARGEFILE) is the
 *                    preferred method of accessing large files on 32-bit
 *                    systems.
 *   - O_NOATIME      Do not update the file last access time (st_atime in the
 *                    inode) when the file is read(2).
 *   - O_NONBLOCK     The file is opened in nonblocking mode. This flag has no
 *                    effect on the operation of poll(2), select(2), epoll(7),
 *                    and similar.
 *   - O_PATH         Obtain a file descriptor that can be used for two
 *                    purposes: to indicate a location in the filesystem tree
 *                    and to perform operations that act purely at the file
 *                    descriptor level. The file itself is not opened, and the
 *                    file operations read(2), write(2), fchmod(2), fchown(2),
 *                    fgetxattr(2), ioctl(2), mmap(2) fail with the error
 *                    EBADF. The operations close(2), fchdir(2), fstat(2),
 *                    fstatfs(2), dup(2), fcntl(2) F_DUPFD, fcntl(2) F_GETFD
 *                    and F_SETFD, fcntl(2) F_GETFL can be performed on the
 *                    resulting file descriptor
 *   - O_SYNC         Write operations on the file will complete according to
 *                    the requirements of synchronized I/O file integrity
 *                    completion. Same as though each write(2) was followed by
 *                    a call to fsync(2).
 */
typedef enum FileStatusFlag {                                     // open(2)
	FSTATUS_APPEND = O_APPEND,
	FSTATUS_ASYNC = O_ASYNC,
	FSTATUS_DIRECT = O_DIRECT,
	FSTATUS_DSYNC = O_DSYNC,
	FSTATUS_LARGEFILE = O_LARGEFILE,
	FSTATUS_NOATIME = O_NOATIME,
	FSTATUS_NONBLOCK = O_NONBLOCK,
	FSTATUS_PATH = O_PATH,
	FSTATUS_SYNC = O_SYNC
} FileStatusFlag;

// ════════════════════════════ Function Prototypes ═══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_openFile
 * Description: Opens the file using the specified pathName and flags
 *
 * Parameters:
 *   pathName       The name of the file to open
 *   flags          Specifies the file access mode, creation mode, and file status
 * Returns:         The opened file descriptor
 * ----------------------------------------------------------------------------
 */
int e2f74138_openFile(const char *pathName, const int flags);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_closeFile
 * Description: Closes the file referenced by the file descriptor
 *
 * Parameters:
 *   fd             The file descriptor
 *   pathName       The name of the file to close (used for error handling)
 * ----------------------------------------------------------------------------
 */
void e2f74138_closeFile(const int fd, const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_fileExists
 * Description: Returns true if file exists, false otherwise
 *
 * Parameters:
 *   pathName   The name of the file to check for existence
 * Returns:     true if file exists, false otherwise
 * ----------------------------------------------------------------------------
 */
bool e2f74138_fileExists(const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_isReadable
 * Description: Returns true if file exists and is readable, false otherwise
 *
 * Parameters:
 *   pathName   The name of the file to check for readability
 * Returns:     true if file exists and is readable, false otherwise
 * ----------------------------------------------------------------------------
 */
bool e2f74138_isReadable(const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_isWritable
 * Description: Returns true if file exists and is writable, false otherwise
 *
 * Parameters:
 *   pathName   The name of the file to check for writability
 * Returns:     true if file exists and is writable, false otherwise
 * ----------------------------------------------------------------------------
 */
bool e2f74138_isWritable(const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_isExecutable
 * Description: Returns true if file exists and is executable, false otherwise
 *
 * Parameters:
 *   pathName   The name of the file to check for executability
 * Returns:     true if file exists and is executable, false otherwise
 * ----------------------------------------------------------------------------
 */
bool e2f74138_isExecutable(const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_isAccessible
 * Description: Returns true if file exists and all requested permissions granted, false otherwise
 *
 * Parameters:
 *   pathName   The name of the file to check for accessibility
 *   mode       The bitwise OR of one or more of R_OK, W_OK, and X_OK
 * Returns:     true if file exists and all requested permissions granted, false otherwise
 * ----------------------------------------------------------------------------
 */
bool e2f74138_isAccessible(const char *pathName, int mode);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_getDescriptorStatus
 * Description: Populates the FileStatus struct with information about the file
 *              descriptor
 *
 * Parameters:
 *   fd             The file descriptor to retrieve file information for
 *   fileStatus     The FileStatus struct to populate
 * Returns:     True if the fstat() operation succeeded, false otherwise
 * ----------------------------------------------------------------------------
 */
bool e2f74138_getDescriptorStatus(int fd, FileStatus* fileStatus);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_getFileStatus
 * Description: Populates the FileStatus struct with information about the file
 *
 * Parameters:
 *   pathName       The name of the file to get the file status on
 *   fileStatus     The FileStatus struct to populate
 * ----------------------------------------------------------------------------
 */
void e2f74138_getFileStatus(const char *pathName, FileStatus* fileStatus);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_getLinkStatus
 * Description: Populates the FileStatus struct with information about either a
 *              file or a symbolic link to a file
 *
 * Parameters:
 *   pathName       The name of the file or symbolic link to get status on
 *   fileStatus     The FileStatus struct to populate
 * ----------------------------------------------------------------------------
 */
void e2f74138_getLinkStatus(const char *pathName, FileStatus* fileStatus);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_readFile
 * Description: Reads up to count bytes from the file descriptor into the buffer
 *
 * Parameters:
 *   fd             The file descriptor of the file to read
 *   buffer         The buffer to read into
 *   count          The maximum number of bytes to read
 *   pathName       The name of the file to read (used for error handling)
 * Returns:         The actual number of bytes read
 * ----------------------------------------------------------------------------
 */
ssize_t e2f74138_readFile(const int fd, void *buffer, size_t count, const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_readlink
 * Description: Reads the value of a symbolic link
 *
 * Parameters:
 *   pathName       The path name of the symbolic link
 *   fileSize       The size of the symbolic link
 * Returns:         The real path name of the file referenced by the symbolic link
 * ----------------------------------------------------------------------------
 */
char *e2f74138_readlink(const char *pathName, const ssize_t fileSize);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_readString
 * Description: Populates the character buffer with up to bufSize characters
 *              from the specified file
 *
 * Parameters:
 *   pathName   The path name of the file
 *   buffer     The string buffer to populate
 *   bufSize    The size of the buffer
 * Returns:     The number of bytes read
 * ----------------------------------------------------------------------------
 */
uint32_t e2f74138_readString(char *pathName, char *buffer, uint32_t bufSize);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_readUint32Hex
 * Description: Returns the unsigned int value from the specified file
 *
 * Parameters:
 *   pathName   The path name of the file
 * Returns:     The unsigned int value from the specified file
 * ----------------------------------------------------------------------------
 */
uint32_t e2f74138_readUint32Hex(char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_read_uint64
 * Description: Returns the unsigned long int value in the specified file
 *
 * Parameters:
 *   pathName       The path name of the file
 * Returns:         The unsigned long int value contained within the file
 * ----------------------------------------------------------------------------
 */
uint64_t e2f74138_read_uint64(const char *pathName);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_realpath
 * Description: Returns the canonicalized absolute pathname
 *
 * Parameters:
 *   pathName       The path name of the file to evaluate
 * Returns:         The canonicalized absolute pathname
 * ----------------------------------------------------------------------------
 */
char *e2f74138_realpath(const char *pathName);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Error Handling Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e2f74138_printOpenError
 * Description: Prints a message related to open() errors
 *
 * Parameters:
 *   pathName   The file path name with the error
 *   errorNum   The error number generated by open()
 * ----------------------------------------------------------------------------
 */
void e2f74138_printOpenError(char *pathName, int errorNum);

#endif /* ORG_DEVOPSBROKER_IO_FILE_H */
