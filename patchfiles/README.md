## AppEKG Patches for Some Applications

This folder contains patch files that will add AppEKG instrumentation
to applications that we have worked with. The patch files will contain
Makefile variable settings that point to our own AppEKG installation
and will need to be changed.

A good choice when applying a patch file is to do a dry run first. When
in the top-level directory of the application source tree, do:
~~~
patch --dry-run --verbose -p1 < patch-file.txt
~~~
If the output looks like it can successfully apply the patch, then 
go ahead and patch it.

Patch file names contain the short commit hash that the patch file
was created on.

Creating a patch file can be done using:
~~~
diff -Naru orig-app-dir ekginst-app-dir
~~~
If there are files to ignore, use "-x pattern" to ignore files.

### Notes

Need to check miniAMR patch to ensure the ref version has the correct
heartbeats and rate factors.

### MiniAMR

MiniAMR at https://github.com/Mantevo/miniAMR 

The latest version (as of Sep 2025) works. In root MiniAMR directory, do:

patch --dry-run --verbose -p1 < /home/.../AppEKG/patchfiles/miniAMR-commit-5bf66c0-AppEKG-patch.txt

If all ok then remove "--dry-run"

You will need to edit the Makefiles in each of the openmp and ref directories to point to your installation of AppEKG. Make sure that AppEKG is compiled with OpenMP support. If AppEKG is compiled with SQLite3 support, you will need to add "-lsqlite3" to the APPEKG library flags.


