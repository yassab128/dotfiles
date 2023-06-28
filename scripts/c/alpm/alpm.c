#include <alpm.h>
#include <assert.h>
#include <stdio.h>

static int
size_comp(const void *pkg1, const void *pkg2)
{
	return alpm_pkg_get_isize((alpm_pkg_t *)pkg1) >
	       alpm_pkg_get_isize((alpm_pkg_t *)pkg2);
}

#define DATE_BUFFER_SIZE 17

static void
convert_epoch_to_string(const time_t installdate, char *const buf)
{
	const struct tm *timeptr;

	timeptr = localtime(&installdate);
	strftime(buf, DATE_BUFFER_SIZE, "%F %R", timeptr);
}

static void
print_pkg_info(alpm_pkg_t *const pkg)
{
	char buf[DATE_BUFFER_SIZE];
	unsigned char ndepends;
	unsigned char noptdepends;

	convert_epoch_to_string(alpm_pkg_get_installdate(pkg), buf);

	ndepends = (unsigned char)alpm_list_count(alpm_pkg_get_depends(pkg));

	noptdepends =
	    (unsigned char)alpm_list_count(alpm_pkg_get_optdepends(pkg));

	printf("%c|%s: %ld: %s, %hhud, %hhuo\n",
	    alpm_pkg_get_reason(pkg) == 0 ? 'E' : ' ', alpm_pkg_get_name(pkg),
	    alpm_pkg_get_isize(pkg), buf, ndepends, noptdepends);
}

static void
iterate_over_packages_list(alpm_list_t *pkgs)
{
	while (pkgs != (alpm_list_t *)0) {
		print_pkg_info((alpm_pkg_t *)pkgs->data);
		pkgs = alpm_list_next(pkgs);
	}
}

static alpm_list_t *
get_sorted_pkgs_list(alpm_db_t *db)
{
	alpm_list_t *unsorted_pkgs;
	alpm_list_t *sorted_pkgs;

	unsorted_pkgs = alpm_db_get_pkgcache(db);

	sorted_pkgs = alpm_list_msort(
	    unsorted_pkgs, alpm_list_count(unsorted_pkgs), size_comp);

	return sorted_pkgs;
}

static void
list_packages()
{
	int retval;
	alpm_handle_t *handle;
	alpm_db_t *db;
	alpm_list_t *sorted_pkgs;

	handle = alpm_initialize("/", "/var/lib/pacman/", (alpm_errno_t *)0);
	assert(handle != (alpm_handle_t *)0);

	/* This shouldn't fail provided alpm_initialize() didn't. */
	db = alpm_get_localdb(handle);

	sorted_pkgs = get_sorted_pkgs_list(db);

	iterate_over_packages_list(sorted_pkgs);

	retval = alpm_release(handle);
	assert(retval == 0);
}

int
main()
{
	list_packages();
	return EXIT_SUCCESS;
}
