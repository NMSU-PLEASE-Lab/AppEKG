# AppEKG Patches for Some Applications

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

JEC note: Current miniAMR patch is only for the openmp version; we need
to change this to include patches for the reference (mpi only) version.
