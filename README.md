TTF - TrueType/OpenType Font Library
====================================

![Version](https://img.shields.io/github/v/release/michaelrsweet/ttf?include_prereleases)
![Apache 2.0](https://img.shields.io/github/license/michaelrsweet/ttf)
[![Build Status](https://img.shields.io/github/actions/workflow/status/michaelrsweet/ttf/build.yml)](https://github.com/michaelrsweet/ttf/actions/workflows/build.yml)
[![Coverity Scan Status](https://img.shields.io/coverity/scan/32614.svg)](https://scan.coverity.com/projects/michaelrsweet-ttf)

TTF is a simple C library for using TrueType and OpenType font files.


Requirements
------------

You'll need a C compiler.


How to Incorporate in Your Project
----------------------------------

Add the `ttf.c` and `ttf.h` files to your project.  Include the `ttf.h`
header in any file that needs to read font files.


"Kicking the Tires"
-------------------

The supplied makefile allows you to build the unit tests on Linux and macOS (at
least), which verify that all of the functions work as expected to read some
sample font files in the `testfiles` directory:

    make test


Resources
---------

The "ttf.html" file contains some simple HTML documentation for the library.


Legal Stuff
-----------

Copyright © 2018-2026 by Michael R Sweet.

TTF is licensed under the Apache License Version 2.0 with an exception to
allow linking against GPL2/LGPL2 software (like older versions of CUPS).  See
the files "LICENSE" and "NOTICE" for more information.
