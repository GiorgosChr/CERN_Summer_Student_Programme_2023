# ToDo List - CERN Summer Student Programme

## $1^{st}$ Week 12/06 - 16/6
### FirstPlots
- [x] Plot $m(D^0)$
- [x] Plot $m(D^\star)$
- [x] Plot the mass difference of the two
- [x] Plot Transverse momentum of $D^0$ and of the soft $\pi$ and $K$ mesons
- [ ] Histograms with errorbars (sqrt)
- [x] 2D Histogram for $p_x - p_z$

## $2^{nd}$ Week 19/06 - 23/06
### ChargedPi
- [x] Copy TTree to a new file
- [x] Add random charges to soft $\pi$ with uniform distribution\
- [x] Generate data with RapidSim for $D^{\star}\rightarrow \{D^0 \rightarrow \pi^-\pi^+\}\pi$
- [x] Use $0.25$ as production asymmetry, i.e., production of $D^0$ and $\bar{D}^0$, the probability then should be $N_\pm = \frac{1 \pm A}{2}$.
- [x] Save this to a new intermediate TTree in a ROOT file
- [x] Use $0.50$ as $CP$ asymmetry, i.e., asymmetry between $D^0 \to K^- K^+$ and $\bar{D}^0 \to K^- K^+$. This should remove either positively charged or negatively charged $\pi$ events. The probability for the $D^0$ is $\frac{1-A_{CP}}{1 + A_{CP}}$

## $3^{rd}$ Week 26/06 - 30/06
### ChargedPi
- [x] Assign detection asymmetry to the charged pion. Same function as the $CP$ asymmetry.
- [x] Same asymmetries with different $A_{CP}$ but for the $D^0\rightarrow \pi^-\pi^+$ TTree.

### ComparisonPlots
- [x] Generate data with RapidSim for $\eta$ and $\phi$ for all particles.
- [x] Compare $p, p_T, \eta, \phi$ of the $D^\star$ and soft $\pi$ for the two decays $D^0\to K^-K^+$ and $D^0\to \pi^-\pi^+$
- [ ] Add error bars to the comparison plots.

### FirstPlots
- [x] $p_x-p_z$ histogram for soft $\pi$


## $4^{th}$ Week 03/07 - 07/07
### MomentumDependence
- [x] Add 0 production asymmetry, and the same detection asymmetry for both $D^0\to K^-K^+$ and $D^0\to \pi^-\pi^+$ with momentum dependence
- [x] Add different $CP$ asymmetry to the two sets with a large value
- [x] Calculate total asymmetry and uncertainty
- [ ] Try to add negative $CP$ asymmetry (Not necessary)
- [ ] Solve multithreading problem (Not necessary)
- [x] Calculate integrated detection asymmetry
- [x] Calculate total asymmetry with the correct formula and uncertainty due to the integrated detection asymmetry
- [x] Create LaTeX report for this week

## $5^{th}$ Week 10/07 - 14/07
### MomentumDependence
- [x] Add more bins to the detection asymmetry histogram
- [x] Minor stuff on LaTeX report
- [x] Save final TTrees in new files

### WeightingFunction
- [x] Create normalized 3d distributions according to the Decay formalism pdf
- [x] Equalize $KK$ and $\pi\pi$ distributions
- [x] Increase statistics for Weighting function using RapidSim
- [x] Fix NaN weight problem
- [ ] Fix other weight problems
- [x] Plot some kinematics

 
### CDS Report
- [ ] Begin writing the CDS Report.
- [ ] Add introduction and motivation
- [ ] Add $\Delta A_{CP}$ formalism and the weighting function

### FullAnalysis
- [ ] 1 and -1 detection asymmetry for up and down on $p_x-p_z$ plane (optional)
- [ ] 5th week LaTeX report
- [ ] Compare $\Delta A_{\text{total}}$ and $\Delta A_{CP}$ before and after applying the weights
- [ ] Put everything in the CDS Report
- [ ] First introduce CP, calculate weights, then introduce the detection asymmetry and then apply the weights on the detection asymmetry sample.

## $6^{th}$ Week 17/07 - 21/07
### WeightingFunction
- [x] Check weighting function regarding $\phi$
- [x] Write $6^{th}$ week report

### CDS Report
- [ ] Begin writing the CDS Report.
- [ ] Add introduction and motivation
- [ ] Add $\Delta A_{CP}$ formalism and the weighting function
- [ ] Add plots from Federico's thesis for asymmetry.

