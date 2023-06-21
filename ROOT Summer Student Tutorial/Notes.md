# ROOT Tutorial
## Macros
- Name the file `example.C` and the void function `example(){...}`
- The Macro is executed using `root example.C`
- Or by `.x example.C` or `.L example.C` and `example();` 

## ROOT Files
- `Tfile` instances that are binary and can be compressed
- Open a new file `TFile f("myfile.root", "RECREATE");`
- Write histograms to a file using `histogram->Write();`
  
```
TFile *file = new TFile(fileName, "RECREATE); // Open a new file in write mode 
TH1F *hist = file->Get<TH1F>(histogramName); // Obtain the histogram from a file
hist->Draw();
```

- Columns are called Branches
- Rows are called Events
### TTree Object
- Columns are also called Branches
- Supports any type of object

## RDataFrame 
```
ROOT::RDataFrame d("t", "f.root");
auto h = d.Filter("theta > 0").Histo1D("pt");
h->Draw();

auto h1 = d.Filter("theta > 0").Histo1D("pt");
auto h2 = d.Filter("theta < 0").Histo1D("pt");
h1->Draw(); // event loop is run lazily once here
h2->Draw("SAME"); // no need to run loop again here

// We can specify the name, title and axis ranges
auto h = d.Histo1D({"myName","Title;x",10,0.,1.},"x");

double m = d.Filter("x > y")
        .Define("z", "sqrt(x*x + y*y)")
        .Mean("z");

d.Filter("x > 0", "xcut")
        .Filter("y < 2", "ycut");
d.Report()->Print();
```

```
// output
xcut : pass=49 all=100 -- 49.000 %
ycut : pass=22 all=49 -- 44.898 %
```

### Saving data to a file
We filter the data, add a new column, and then save everything to file. No boilerplate code at all.
```
auto new_df = df.Filter("x > 0")
        .Define("z", "sqrt(x*x + y*y)")
        .Snapshot("tree", "newfile.root");
```