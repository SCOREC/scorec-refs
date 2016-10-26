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
