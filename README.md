PSFPoint
========
[![Travis Build Status](https://travis-ci.org/loveemu/psfpoint.svg?branch=master)](https://travis-ci.org/loveemu/psfpoint) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/5y8wb6fc7sdytmi6/branch/master?svg=true)](https://ci.appveyor.com/project/loveemu/psfpoint/branch/master)

PSFPoint is a simple command-line PSF tagger written by Neill Corlett.

Introduction
------------

Long ago, Neill Corlett wrote a little command-line ID3 tag setting program called
"Pointless ID3 Tagger" - ID3Point for short.  This is the same thing for PSF
files.

It's easy!

Usage
-----

`psfpoint [-tf] [-variable=value ...] psf-file(s)`

`-tf`
  : Sets the title tag according to the filename.
    Obvious track numbers, "%20", and other garbage is processed.

`-variable=value`
  : Sets the given variable name to the given value.   
    Note that if this has spaces in it, you have to enclose the option in quotation marks, i.e. `"-variable=value with spaces"`   
    If you want to delete a variable, just say: `-variable=`

`psf-file(s)`
  : One or more PSF filenames.  Wildcards are accepted.

Examples
--------

To set the game name of every PSF file in the current directory:

```
psfpoint "-game=Final Fantasy 9" *.psf
```

Or to set the volumes to 1.0:

```
psfpoint -volume=1 *.psf
```

Or to delete all comments:

```
psfpoint -comment= *.psf
```

The possibilities are endless!

Thanks to
---------

- ProtoCat, again for no reason at all.

Where to find me
----------------

- Neill Corlett
    - email: <neill@neillcorlett.com>
    - web:   <http://www.neillcorlett.com/>
- Github version by loveemu <https://github.com/loveemu/psfpoint>
