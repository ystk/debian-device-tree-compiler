/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase for fdt_get_path()
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <fdt.h>
#include <libfdt.h>

#include "tests.h"
#include "testdata.h"

#define POISON	('\xff')

void check_path_buf(void *fdt, const char *path, int pathlen, int buflen)
{
	int offset;
	char buf[buflen+1];
	int len;

	offset = fdt_path_offset(fdt, path);
	if (offset < 0)
		FAIL("Couldn't find path \"%s\": %s", path, fdt_strerror(offset));

	memset(buf, POISON, sizeof(buf)); /* poison the buffer */

	len = fdt_get_path(fdt, offset, buf, buflen);
	if (buflen <= pathlen) {
		if (len != -FDT_ERR_NOSPACE)
			FAIL("fdt_get_path([%d bytes]) returns %d with "
			     "insufficient buffer space", buflen, len);
	} else {
		if (len < 0)
			FAIL("fdt_get_path([%d bytes]): %s", buflen,
			     fdt_strerror(len));
		if (len != pathlen)
			FAIL("fdt_get_path([%d bytes]) reports length %d "
			     "instead of %d", buflen, len, pathlen);
		if (strcmp(buf, path) != 0)
			FAIL("fdt_get_path([%d bytes]) returns \"%s\" "
			     "instead of \"%s\"", buflen, buf, path);
	}

	if (buf[buflen] != POISON)
		FAIL("fdt_get_path([%d bytes]) overran buffer", buflen);
}

void check_path(void *fdt, const char *path)
{
	int pathlen = strlen(path);

	check_path_buf(fdt, path, pathlen, 1024);
	check_path_buf(fdt, path, pathlen, pathlen+1);
	check_path_buf(fdt, path, pathlen, pathlen);
}

int main(int argc, char *argv[])
{
	void *fdt;

	test_init(argc, argv);
	fdt = load_blob_arg(argc, argv);

	check_path(fdt, "/");
	check_path(fdt, "/subnode@1");
	check_path(fdt, "/subnode@2");
	check_path(fdt, "/subnode@1/subsubnode");
	check_path(fdt, "/subnode@2/subsubnode@0");

	PASS();
}
