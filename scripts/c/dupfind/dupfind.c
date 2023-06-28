#include <assert.h>
#include <fcntl.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <zlib.h>

struct readable_size {
	float value;
	char unit;
	char gap[3];
};

struct file {
	char *path;
	ssize_t size;
	unsigned long adler32_checksum;
	unsigned char is_a_duplicate;
	const char gap[7];
};

static struct readable_size
convert_size(const long bytes)
{
	struct readable_size size;

	if (bytes > 1073741824) {
		size.value = (float)bytes / 1073741824.0F;
		size.unit = 'G';
	} else if (bytes > 1048576) {
		size.value = (float)bytes / 1048576.0F;
		size.unit = 'M';
	} else if (bytes > 1024) {
		size.value = (float)bytes / 1024.0F;
		size.unit = 'K';
	} else {
		size.value = (float)bytes;
		size.unit = 'B';
	}
	return size;
}

#if 0
#	define MEGABYTE 1048576
static unsigned long
calculate_adler32_checksum(const char *const file_path)
{
	unsigned char buffer[MEGABYTE];
	unsigned long bytes_read;
	unsigned long crc;
	FILE *fp;

	fp = fopen(file_path, "r");
	if (!fp) {
		return 0;
	}
	crc = 0;
	for (bytes_read = fread(buffer, 1, MEGABYTE, fp); bytes_read > 0; bytes_read
	     = fread(buffer, 1, MEGABYTE, fp)) {
		crc = adler32_z(crc, buffer, bytes_read);
	}
	fclose(fp);

	return crc;
}

#endif

#define EMPTY_FILE_adler32_CHECKSUM 0xDEADDA7A

static int
compar(void *const a, void *const b)
{
	struct file *const a_file = (struct file *)a;
	int fd;
	int retval;
	struct file *const b_file = (struct file *)b;
	void *data;

	if (!a_file->size) {
		a_file->is_a_duplicate = 1;
		a_file->adler32_checksum = EMPTY_FILE_adler32_CHECKSUM;
		return 1;
	}
	if (!b_file->size) {
		b_file->is_a_duplicate = 1;
		b_file->adler32_checksum = EMPTY_FILE_adler32_CHECKSUM;
		return -1;
	}
	if (a_file->size > b_file->size) {
		return -1;
	} else if (a_file->size < b_file->size) {
		return 1;
	}
	if (!a_file->adler32_checksum) {
		fd = open(a_file->path, O_RDONLY);
		if (fd == -1) {
			puts(a_file->path);
		}
		assert(fd != -1);
		data = mmap(0, (unsigned long)a_file->size, PROT_READ,
		    MAP_PRIVATE, fd, 0);
		assert(data != MAP_FAILED);
		retval = close(fd);
		assert(retval == 0);
		a_file->adler32_checksum =
		    adler32_z(0, data, (size_t)a_file->size);
		munmap(data, (size_t)a_file->size);
	}
	if (!b_file->adler32_checksum) {
		fd = open(b_file->path, O_RDONLY);
		assert(fd != -1);
		data = mmap(
		    0, (size_t)b_file->size, PROT_READ, MAP_PRIVATE, fd, 0);
		assert(data != MAP_FAILED);
		retval = close(fd);
		assert(retval == 0);
		b_file->adler32_checksum =
		    adler32_z(0, data, (unsigned long)b_file->size);
		munmap(data, (size_t)b_file->size);
	}
	if (a_file->adler32_checksum > b_file->adler32_checksum) {
		return 1;
	} else if (a_file->adler32_checksum < b_file->adler32_checksum) {
		return -1;
	} else {
		a_file->is_a_duplicate = 1;
		b_file->is_a_duplicate = 1;
		return 0;
	}
}

int
main(const int argc, char *const *const argv)
{
	unsigned int i = 0;
	struct file *files;
	{
		const FTSENT *node;
		char *path_argv[2];
		unsigned long nfiles = 1;
		int retval;
		FTS *tree;

		path_argv[0] = argc == 2 ? argv[1] : ".";
		path_argv[1] = NULL;
		tree = fts_open(path_argv, 0, NULL);
		assert(tree != NULL);

		files = malloc(sizeof(struct file));
		assert(files != NULL);
		for (node = fts_read(tree); node; node = fts_read(tree)) {
			if (node->fts_info != FTS_F) {
				continue;
			}
			if (!(node->fts_statp->st_mode & S_IRUSR)) {
				continue;
			}
			if (!(node->fts_statp->st_mode & S_IROTH)) {
				continue;
			}
			if (nfiles == i) {
				nfiles = i << 1;
				files = realloc(
				    files, nfiles * sizeof(struct file));
			}
			assert(files != NULL);

			files[i].path = malloc(node->fts_pathlen + 1);
			assert(files[i].path != NULL);
			memcpy(files[i].path, node->fts_path,
			    node->fts_pathlen + 1);

			files[i].size = node->fts_statp->st_size;

			files[i].is_a_duplicate = 0;
			files[i].adler32_checksum = 0;

			++i;
		}
		retval = fts_close(tree);
		assert(retval == 0);
	}

	qsort(files, i, sizeof(struct file), compar);

	{
		struct readable_size size;
		unsigned long previous_file_adler32_checksum = 0;
		for (; i--; free(files[i].path)) {
			if (!files[i].is_a_duplicate) {
				continue;
			}
			if (files[i].adler32_checksum ==
			    previous_file_adler32_checksum) {
				printf("%s\n", files[i].path);
			} else {
				previous_file_adler32_checksum =
				    files[i].adler32_checksum;
				size = convert_size(files[i].size);
				printf("\n\033[1;31m%08lx, %.4g%c\033[0m\n%s\n",
				    files[i].adler32_checksum,
				    (double)size.value, size.unit,
				    files[i].path);
			}
		}
	}
	free(files);
	return 0;
}
