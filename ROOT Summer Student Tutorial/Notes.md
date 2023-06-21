# ROOT Tutorial
## Macros
- Name the file `example.C` and the void function `example(){...}`
- The Macro is executed using `root example.C`
- Or by `.x example.C` or `.L example.C` and `example();` 

## ROOT Files
- `Tfile` instances that are binary and can be compressed
- Open a new file `TFile f("myfile.root", "RECREATE");`
- Write histograms to a file using `histogram->Write();`