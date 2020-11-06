# Replicability

### Technical paper

Semi-Procedural Textures Using Point Process Texture Basis Functions

P. Guehl , R. AllEgre , J.-M. Dischler, B. Benes , and E. Galin

Computer Graphics Forum (EGSR 2020 special issue)

### Overview

This is a guide to reproduce the "visual structures" results from our procedural stochastic PPTBF model (i.e. binary images).

NOTE : It works on Windows systems for the moment. (Linux port soon)
We have tested the softwares with the following graphics cards:
- NVidia GeForce 1060 GTX (6Go)
- NVidia RTX 2070 (8 Go)

# How To

The code and binaries are able to at least reproduce the Figure 11 (see below) of our article, named "Evaluation of the capability of PPTBF to produce natural structures".

![figure 11](figure_11_from_paper.png)

This figure is a short extract of our supplemental material #2 (available here), especially page "Supplemental2/Page1.html" providing additional results on PPTBF parameters estimation (147 results). So, this supplemental material #2 is the reference data to compare.

![supplemental 2](supplemental_2_snapshot.png)

For this, we provide code required to produce procedural binary structures from files of PPTBF parameters. For instance, in the supplemental material #2, the directory : "Supplemental2\images\Matching_structures\cells\111357_seg_scrop" contains the original image "111357_seg_scrop.jpg", and we provide a parameter file "111357_seg_scrop_pptbf_params.txt" from which our PPTBF procedural model have generated images "111357_seg_scrop_pptbf.jpg" and its thresholded version "111357_seg_scrop_pptbf_binary.jpg" (that looks like the original image). The goal of our code and binaries is to replicate all files "xxx_pptbf.jpg" and "xxx_pptbf_binary.jpg" from parameter files "xxx_pptbf_params.txt" to looks like original images "xxx.jpg".

In the figure 11, you can find the following replicated images:

LEFT COLUMN (top to bottom):

20181215_153153_seg_scrop_pptbf_binary.png
bumpy_hard_concrete_texture_9261475_seg_scrop_pptbf_binary.png [NOTE: for this image, you may find a difference due to a translation in the original article]
foam_texture_seg_scrop_pptbf_binary
mud_flakes_closeup_150326_seg_scrop_pptbf_binary.png
MIDDLE COLUMN (top to bottom):

phasor_seg_scrop_pptbf_binary.png
deephexa_seg_scrop_pptbf_binary.png
mosaicstones1_seg_scrop_pptbf_binary.png
straw_seg_scrop_pptbf_binary.png
RIGHT COLUMN (top to bottom):

Marble_seg_scrop_pptbf_binary.png
14596343807_24e447963c_o_seg_scrop_pptbf_binary.png
whiteash_seg_scrop_pptbf_binary.png
TexturesCom_Crackles0011_S_seg_scrop_pptbf_binary.png
We provide two ways to replicate data: either from a precompiled binary executable or from recompilation of source code from scratch (see below for details).

[1] REPLICATE FROM BINARIES: Windows binaries are available here. We provide a unique script that replicates all binary images from supplemental #2 (see "Supplemental2/Page1.html" webpage), from which our figure 11 in our aticle is an extract. Launch the unique script "launch_pptbf_results.bat" that will generate all images into the "Data\Matching_structures" directory.

[2] REPLICATE FROM SOURCE CODE: To replicate all from scratch, compile code in the "PPTBF_Structures_Src" directory following provided "readme.txt" in the same directory. Then launch the unique script "replicate_pptbfResults_matchingStructures.bat" that will generate all images into the "Install\PPTBFProject\bin\Data\Matching_structures" directory.

NOTE: sometimes, the generation of 1 of the 147 images may seem to crash depending on systems (hardware?).
