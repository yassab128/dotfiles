/* Compile with "gcc -g -lalpm listpkgs.c" */
#include <alpm.h>
#include <assert.h>
#include <stdio.h>

#define DATE_BUFFER_SIZE 17

static void
convert_epoch_to_string(const time_t installdate, char *const buf)
{
	const struct tm *timeptr;

	timeptr = localtime(&installdate);
	strftime(buf, DATE_BUFFER_SIZE, "%F %R", timeptr);
}

static unsigned short
get_nrevdeps(alpm_pkg_t *const pkg)
{
	alpm_list_t *revdeps;
	unsigned short nrevdeps;

	revdeps = alpm_pkg_compute_requiredby(pkg);
	nrevdeps = (unsigned short)alpm_list_count(revdeps);
	FREELIST(revdeps);

	return nrevdeps;
}

static void
print_pkg(alpm_pkg_t *const pkg)
{
	char buf[DATE_BUFFER_SIZE];
	unsigned short noptdepends;
	unsigned short nrevdeps;

	convert_epoch_to_string(alpm_pkg_get_installdate(pkg), buf);

	noptdepends =
	    (unsigned short)alpm_list_count(alpm_pkg_get_optdepends(pkg));

	nrevdeps = get_nrevdeps(pkg);

	printf("%c|%s: %ld: %s, %hur, %lud, %huo\n",
	    alpm_pkg_get_reason(pkg) == 0 ? 'E' : ' ', alpm_pkg_get_name(pkg),
	    alpm_pkg_get_isize(pkg), buf, nrevdeps,
	    alpm_list_count(alpm_pkg_get_depends(pkg)), noptdepends);
}

static void
iterate_over_pkg_list(alpm_list_t *pkg_list)
{
	while (pkg_list != NULL) {
		print_pkg((alpm_pkg_t *)pkg_list->data);
		pkg_list = alpm_list_next(pkg_list);
	}
}

#pragma GCC diagnostic ignored "-Wcast-qual"
static int
comp_isize(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_isize((alpm_pkg_t *)pkg1) >
	       alpm_pkg_get_isize((alpm_pkg_t *)pkg2);
}
static int
comp_isize_reverse(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_isize((alpm_pkg_t *)pkg1) <
	       alpm_pkg_get_isize((alpm_pkg_t *)pkg2);
}
static int
comp_installdate(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_installdate((alpm_pkg_t *)pkg1) >
	       alpm_pkg_get_installdate((alpm_pkg_t *)pkg2);
}
static int
comp_installdate_reverse(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_installdate((alpm_pkg_t *)pkg1) <
	       alpm_pkg_get_installdate((alpm_pkg_t *)pkg2);
}
static int
comp_depends(const void *pkg1, const void *pkg2)
{
	return alpm_list_count(alpm_pkg_get_depends((alpm_pkg_t *)pkg1)) >
	       alpm_list_count(alpm_pkg_get_depends((alpm_pkg_t *)pkg2));
}
static int
comp_depends_reverse(const void *pkg1, const void *pkg2)
{
	return alpm_list_count(alpm_pkg_get_depends((alpm_pkg_t *)pkg1)) <
	       alpm_list_count(alpm_pkg_get_depends((alpm_pkg_t *)pkg2));
}
static int
comp_reason(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_reason((alpm_pkg_t *)pkg1) >
	       alpm_pkg_get_reason((alpm_pkg_t *)pkg2);
}
static int
comp_reason_reverse(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_reason((alpm_pkg_t *)pkg1) <
	       alpm_pkg_get_reason((alpm_pkg_t *)pkg2);
}
static int
comp_requiredby(const void *pkg1, const void *pkg2)
{
	return get_nrevdeps((alpm_pkg_t *)pkg1) >
	       get_nrevdeps((alpm_pkg_t *)pkg2);
}
static int
comp_requiredby_reverse(const void *pkg1, const void *pkg2)
{
	return get_nrevdeps((alpm_pkg_t *)pkg1) <
	       get_nrevdeps((alpm_pkg_t *)pkg2);
}
#pragma GCC diagnostic warning "-Wcast-qual"

int
main()
{
	int retval;
	alpm_handle_t *handle;
	alpm_db_t *db;
	alpm_list_t *pkg_list;

	handle = alpm_initialize("/", "/var/lib/pacman/", NULL);
	assert(handle != NULL);

	db = alpm_get_localdb(handle);

	pkg_list = alpm_list_copy(alpm_db_get_pkgcache(db));
	assert(pkg_list != NULL);

	pkg_list = alpm_list_msort(
	    pkg_list, alpm_list_count(pkg_list), comp_requiredby);

	iterate_over_pkg_list(pkg_list);

	alpm_list_free(pkg_list);

	retval = alpm_release(handle);
	assert(retval == 0);
}
