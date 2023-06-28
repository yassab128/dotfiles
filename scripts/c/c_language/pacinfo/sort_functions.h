static int sort_files(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->files - B->files;
}

static int sort_manpages(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->manpages - B->manpages;
}

static int sort_binaries(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->binaries - B->binaries;
}

static int sort_optdepends(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->optdepends - B->optdepends;
}

static int sort_depends(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->depends - B->depends;
}

static int sort_installdate(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->installdate > B->installdate ? 1 : 0;
}

static int sort_size(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->size > B->size ? 1 : 0;
}

static int sort_explicit(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->explicit - B->explicit;
}

static int sort_rev_depends(const void *a, const void *b)
{
	const struct packages *A = a;
	const struct packages *B = b;
	return A->rev_deps - B->rev_deps;
}
