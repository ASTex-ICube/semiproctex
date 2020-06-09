# Semi-Procedural Textures Using Point Process Texture Basis Functions

![semiproctex](Images/semiproctex.png)

This is the page of our EGSR 2020 CGF track paper:

P. Guehl, R. Allègre, J.-M. Dischler, B. Benes, E. Galin.
Semi-Procedural Textures Using Point Process Texture Basis Functions.
To appear in Computer Graphics Forum (EGSR 2020 special issue), 39(4),
2020.

## Paper

Available soon.

## Supplemental materials

 * **Supplemental #1** : Available soon.
 * **Supplemental #2** : Available soon.
 * **Supplemental #3** : Available soon. This supplemental material contains the database of binary structures used for the paper in full quality. Note that for reasons of terms of use, we do not provide all corresponding texture exemplars. The folder Structures contains manually segmented binary structures. The
 folder Structures_with_PPTBF contains manually segmented binary structures with estimated PPTBF
 parameters, that can be used for procedural structure synthesis.

## Code

 * The [PPTBF_Structures_Src](PPTBF_Structures_Src) directory contains code required to produce
 procedural binary structures from files of PPTBF parameters (works under Windows, Linux
 port soon).
 
 * Windows binaries for procedural binary structures synthesis from files of PPTBF parameters
 are available [here](http://igg.unistra.fr/people/semiproctex/PPTBF_Structures_Bin.zip).

 * Check [this file](PPTBF_file_format.txt) to get a description of the PPTBF file format.

 * The PPTBF parameters estimated from our database of manually segmented structures are available
 following [this link](http://igg.unistra.fr/people/semiproctex/PPTBF_Parameters_v2.zip). Note
 that this is an updated version that improves the parameter values provided with the Supplemental
 materials.

 * The [Semiproc_synthesis_Bin](http://igg.unistra.fr/people/semiproctex/Semiproctex_synthesis_Bin.zip) archive
 contains Windows binaries for our semi-procedural texture synthesis method. The program is able
 to reproduce our texture synthesis results (i.e. color images) of Supplemental material #1.
 We provide the binaries of our GPU implementation. Note that a software version will be
 made available soon (for reasons of intellectual property, the GPU code cannot be
 distributed).

<!--
### Prerequisites

### Getting started

 * Clone this repository.
 * ...
-->

## Citation

If you use this code for your research, please cite our paper.

```
@article{Guehl:EGSR2020,
    author = "P. Guehl and R. All\`egre and J.-M. Dischler and B. Benes and E. Galin",
    title = "Semi-Procedural Textures Using Point Process Texture Basis Functions",
    journal = "Computer Graphics Forum (EGSR 2020 special issue)",
    volume = "39",
    number = "4",
    year = "2020",
    note = "To appear"
}
```
