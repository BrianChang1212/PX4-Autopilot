/****************************************************************************
 *
 * Helper for the logger on littlefs-on-NAND (CONFIG_LOGGER_FSYNC_AT_FS_BLOCK_END).
 *
 * littlefs stores a file as a CTZ skip-list.  Block 0 holds block_size data
 * bytes; block n >= 1 starts with ctz(n) + 1 four-byte pointers, so it holds
 * block_size - 4 * (ctz(n) + 1) data bytes (see lfs_ctz_extend() in lfs.c).
 *
 * After an fsync, the next write calls lfs_ctz_extend().  If the file size is
 * not exactly the end of a block, littlefs copies the whole incomplete last
 * block into a new one, byte by byte, with read-back verification -- on this
 * NAND that costs 1-2 s.  If the size is exactly the end of a block, it only
 * appends a new block.  So: fsync only where a block ends.
 *
 ****************************************************************************/

#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Number of bytes that can still be appended to a littlefs file of "size"
 * bytes before its current block is exactly full (always >= 1).
 */
static inline size_t lfs_bytes_to_block_end(size_t size, size_t block_size)
{
	size_t end = block_size; // block 0: no skip pointers
	uint32_t n = 0;

	while (end <= size) {
		++n;
		end += block_size - 4 * ((size_t)__builtin_ctz(n) + 1);
	}

	return end - size;
}
