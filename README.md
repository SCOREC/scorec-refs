# scorec-refs

A collection of IEEE formatted references.

The IEEE bib style files are here:

http://www.ctan.org/tex-archive/macros/latex/contrib/IEEEtran/bibtex/

and the style guide is here:

https://www.ieee.org/documents/style_manual.pdf

RPI Thesis formatting DropBox:

https://goo.gl/LqqbyO

Usage, assuming this repository is a subdirectory
of your thesis:

```latex
\bibliography{scorec-refs/scorec-refs}
```

Although Google Scholar is a good place to find articles
and get a BibTeX entry good enough for a journal,
we recommend going to the publisher's website
(for example [Elsevier](http://www.sciencedirect.com/))
and using their BibTeX export functionality.
It still won't be 100%, but it will be much more complete.

We provide a Python script `toascii.py` which converts non-ASCII
Unicode symbols found in many exported BibTeX entries
into ASCII symbols, using LaTeX formatting for accents.
Run this first after getting your BibTeX entry:

```
python toascii.py from_the_web.bib ascii_output.bib
```

We also provide a C++11 program `fixrefs` that does
a lot of processing specifically to obtain the kind
of bibliography wanted by the RPI CS department for
doctoral dissertations (IEEE style guide).

```
make
./fixrefs old.bib new.bib
./fixrefs -i myrefs.bib
```

Documentation on BibTeX:

http://tug.ctan.org/info/bibtex/tamethebeast/ttb_en.pdf

http://maverick.inria.fr/~Xavier.Decoret/resources/xdkbibtex/bibtex_summary.html
