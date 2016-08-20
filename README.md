# scorec-refs
A collection of IEEE formatted references.

The IEEE bib style files are here:

http://www.ctan.org/tex-archive/macros/latex/contrib/IEEEtran/bibtex/

and the style guide is here:

https://www.ieee.org/documents/style_manual.pdf

Contents:
* cfd.bib - computational fluid dynamics
* cr.bib - checkpoint and restart
* errorest.bib - error estimators
* fem.bib - finite element method
* frameworks.bib - application frameworks
* hardware.bib - processors, systems, etc.
* io.bib - parallel io
* meshadapt.bib - mesh adaptation
* meshdb.bib - mesh databases and data structures
* msgpass.bib - message passing 
* partition.bib - partitioning
* proglang.bib - programming languages
* algorithms.bib - algorithms

RPI Thesis formatting DropBox: 

https://goo.gl/LqqbyO

ProTip. To list multiple references in your latex file do the following:
```
\bibliography{scorec-refs/frameworks,scorec-refs/cr,scorec-refs/errorest}
```
