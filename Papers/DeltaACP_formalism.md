# $\Delta A_{CP}$ formalism for $D^\star\to D^0(h^-h^+)\pi_s$ decays

## Integrated asymmetries

- The number of produced $D^{\star\pm}$ mesons can be written as a quantity proportional to an integral of a weight function
- $A^+_{hh} (A^-_{hh})$ is the $D^0(\bar{D}^0)\to h^-h^+$ decay amplitude 
- $\Gamma$ is the mean lifetime of $D^0$ and $t$ is the time decay
- $t(\vec{r}_{PV}, \vec{r}_{SV}, \vec{p}_{h^+}\vec{p}_{h^-})$ is the function of the decay time ad a function of the indicated quantities
- $\vec{p}_X$ is the three-momentum, $\vec{r}_{PV}$ is the position of the primary vertex, $\vec{r}_{SV}$ is the position of the secondary vertex
- We denote with $f$ the probability density function (PDF) for each observable quantity and pass as arguments all quantities that affect the PDF
- $\varepsilon_{hh}(\vec{r}_{PV}, \vec{r}_{SV}, \vec{p}_{h^+}\vec{p}_{h^-})$ is the overall efficiency for $D^0$ and $\varepsilon_{\pi_s^\pm}(\vec{r}_{PV}, \vec{p}_{\pi_s})$ is the overall efficiency for soft pions (soft = low momentum)

By defining an operator $\mathcal{G}_{hh}[g]$ acting on a generic function $\mathcal{G}_{hh}[g] = \mathcal{G}^\prime_{hh}[g]/\mathcal{G}^\prime_{hh}[1]$ we obtain the raw asymmetry for $hh$ final state up to the desired order
$$A_\text{raw}^{hh} = \frac{\mathcal{G}_{hh}[A^{hh}_{CP} + A_{P}(\vec{P}_{D^\star}) + A_{D}(\vec{r}_{PV}, \vec{p}_{\pi_s})] + \Gamma R_{hh}\mathcal{G}_{hh}[t(A^{hh}_{CP} + A_{R_{hh}} + A_{P}(\vec{p}_{D^\star}) + A_{D}(\vec{r}_{PV}, \vec{p}_{\pi_s}))]}{1 + \Gamma R_{hh}\mathcal{G}_{hh}[t]}$$
This is the \textit{integrated raw asymmetry} which is valid if the integrated values of the various asymmetries are small.
Further expansion allows us to express the raw asymmetry as
$$A^{hh}_{\text{raw}} = A^{hh}_{CP} - A^{hh}_{\Gamma}\Gamma\langle t\rangle_{hh} + A^{hh}_{P} + A^{hh}_{D}$$

## Equalization of $K^-K^+$ and $\pi^-\pi^+$ distributions
### $D^\star$ and $\pi_s$ kinematics

- We want to simplify the results by neglecting the differences between the two distributions
- We define the weighting function
$$W(\vec{p}_{D^{\star}}, \vec{p}_{\pi_s})\equiv \frac{\Gamma^{\pi\pi}_{D^{\star+}} + \Gamma^{\pi\pi}_{D^{\star-}}}{\Gamma^{KK_{D^{\star+}} + \Gamma^{KK}_{D^{\star-}}}}$$
where we skip the arguments for the decay rate $\Gamma$

By writing the decay rate in a more compact way we arrive at 
$$\Delta A^{\prime}_{\text{raw}} = K\Delta A_{CP}$$

- If the detection and production asymmetries are small over the whole phase space the factor $K$ is approximately equal to 1 and the two asymmetries match
- If the two asymmetries are $\mathcal{O}(10^{-2})$ over phase space then K is equal to one up to a few parts in $10^{-4}$
- If the detection asymmetry is $20\%$ over the full phase space the $K$ factor would differ from one by around $4\%$
- If we integrate over the phase space regions where there is $\pm 100\%$ detection asymmetries this will not contribute to the measurement and the value of $\Delta A^{\prime}_{\text{raw}}$ would be zero, thus, such regions are to be excluded by imposing additional requirements 

### $D^0$ kinematics

We introduce a similar weighting function for $D^0$
$$H(\vec{p}_{D^0}) \equiv \frac{\Gamma^{\pi\pi}_{D^{\star +}} + \Gamma^{\pi\pi}_{D^{\star -}}}{\Gamma^{KK}_{D^{\star +}} + \Gamma^{KK}_{D^{\star -}}}$$

The weighted raw asymmetry difference up to a desired order can be written as $\Delta A_{CP}$ plus some extra terms
- In the limit of small asymmetries the weighted raw asymmetry difference $\Delta A^{\prime}_{\text{raw}} = \Delta A_{CP}$
- It is more convenient to perform the event weighting in $\vec{p}_{D^0}$

### Improvements

We can define an even better weighting function $Q(\vec{p}_{D^\star}, \vec{p}_{\pi_s})$.
- In this case $\Delta A^{\prime}_{\text{raw}}$ would measure $\Delta A_{CP}$ if the average of the detection asymmetry is small

We can also approximate the $Q$ function with excellent accuracy 
$$Q(\vec{p}_{D^\star}, \vec{p}_{\pi_s}) \approx \frac{\Gamma^{\pi\pi}_{D^0}(\vec{p}_{D^\star} - \vec{p}_{\pi_s}) + \Gamma^{\pi\pi}_{\bar{D}^0}(\vec{p}_{D^\star} - \vec{p}_{\pi_s})}{\Gamma^{KK}_{D^0}(\vec{p}_{D^\star} - \vec{p}_{\pi_s}) + \Gamma^{KK}_{\bar{D}^0}(\vec{p}_{D^\star} - \vec{p}_{\pi_s})}$$
- This $Q$ function can be determined from data by counting the candidates $D^0\to\pi^-\pi^+$ and $D^0\to K^-K^+$ in bins of $D^0$ momentum before the $D^\star$ reconstruction takes place
- We can weight $KK$ events without any fiducial requirements if the average detection-production asymmetries are small
- If the average detection(production) asymmetry of $\pi_s(D^{0}, D^{\star})$ are order of percent then $\Delta A^{\prime}_{\text{raw}}$ measures the $\Delta A_{CP}$ up to a systematic error of $\mathcal{O}(10^{-4})$