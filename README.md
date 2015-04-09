PSFPoint
========

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

Terms of Use
------------

PSFPoint and its source code are free and have been released to the public
domain.  You are free to redistribute, modify, use in your own commercial or
noncommercial work, or even claim you wrote it if you want.  I don't care.

Anyone using PSFPoint does so at their own risk.  No warranty is expressed or
implied.

Thanks to
---------

- ProtoCat, again for no reason at all.

Where to find me
----------------

- Neill Corlett
    - email: <neill@neillcorlett.com>
    - web:   <http://www.neillcorlett.com/>
