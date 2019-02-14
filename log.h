/**
 *@file: log.h
 *@desc: log head file
 */

#ifndef __LOG_H__
#define __LOG_H__


/* buffer size for every line's log */
#define LOG_LINE_BUF_SIZE                   1024
#define LOG_BUF_OUTPUT_BUF_SIZE             (LOG_LINE_BUF_SIZE*4*32)

struct bl_log {
	void *buff;
	ssize_t buf_write_size;
};

#endif /*__LOG_H__*/
