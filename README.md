# scorec-refs

A collection of IEEE formatted references.

The IEEE bib style files are here:

http://www.ctan.org/tex-archive/macros/latex/contrib/IEEEtran/bibtex/

and the style guide is here:

https://www.ieee.org/documents/style_manual.pdf

RPI Thesis formatting DropBox:

https://goo.gl/LqqbyO

## Usage

Assuming this repository is a subdirectory
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

And finally we provide a BST file `IEEEtran_rpi.bst` which is a
customized version of `IEEEtran.bst` which will turn the `urldate`
field of an `electronic` entry into a ` (Date Last Accessed: )` note,
and will include the `day` field of an
`inproceedings` entry after the month,
both things asked for by the RPI OGE.
Use this file as follows:

```
\bibliographystyle{scorec-refs/IEEEtran_rpi.bst}
```

## Contributing

* Follow the style of other references (e.g., use brackets instead of quotes)
* After adding a new reference run `fixrefs` to clean up formatting and identify
  any missing fields.
* Place conference `@proceedings` entries at the end of the `bib` file
* When you add or update a reference also add a url to the digital library (see
  below) where you found it.  This will greatly help future users in case some
  information is missing or if they just want to find the article.

## Citations with URLs

* Generic websites the 'website' entry type.  It follows
  the formatting of the seventh example from Section C under the
  'Reports and Handbooks' heading on page 40 of the 2016 IEEE Editorial Style Manual.
``` [7] Apple Inc., Palo Alto, CA, USA, “Apple iPhone,” Available: http://apple.com/iphone/. Accessed: Feb. 26, 2013```

* Software packages should use the 'electronic' entry type. It follows
  the formatting of the second example from Section C under the
  'Manuals/Software' heading on page 40 of the 2016 IEEE Editorial Style Manual
```[2] M. Kuhn. The Caret Package. (2012) [Online]. Available: http://cranr-project.org/web/packages /caret /caret.pdf ```

* Blog entries should use the 'blog' entry type. It follows
  the formatting of the first example from Section C under the
  'Reports and Handbooks' heading on page 39 of the 2016 IEEE Editorial Style Manual
```[1] R. J. Hijmans and J. van Etten, “Raster: Geographic analysis and modeling with raster data,” R Package Version 2.0-12, Jan. 12, 2012. [Online]. Available: http://CRAN.R-project.org/package=raster ```

* Online Manuals should use the 'electronicmanual' entry type. It follows
  the formatting of the first example from Section C under the
  'Manuals/Software' heading on page 40 of the 2016 IEEE Editorial Style Manual
```[1] L. Breimann. (2003). Manual on Setting Up, Using, and Understanding Random Forests v4.0. [Online].  Available: http://oz.berkeley.edu/users/breiman/Using_random_forests_v4.0.pdf, Accessed on: Apr. 16, 2014. ```

## Documentation on BibTeX

http://tug.ctan.org/info/bibtex/tamethebeast/ttb_en.pdf

http://maverick.inria.fr/~Xavier.Decoret/resources/xdkbibtex/bibtex_summary.html

## Digital Libraries - great places to find papers and their citations

http://dl.acm.org/

http://www.sciencedirect.com/

http://onlinelibrary.wiley.com/

https://www.engineeringvillage.com
