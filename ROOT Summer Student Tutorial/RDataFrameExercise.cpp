#include <ROOT/RDataFrame.hxx>

int main(){
        auto fileName = "tdf014_CsvDataSource_MuRun2010B.csv";
        auto rdf = ROOT::RDF::MakeCsvDataFrame(fileName);

        return 0;
}