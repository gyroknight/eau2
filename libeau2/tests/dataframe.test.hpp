
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include "dataframe.hpp"
#include "rower.hpp"

// A basic Rower to sum up the elements in a row
class SumInt : public Rower {
 public:
  long sum;

  SumInt() : sum(0) {}

  virtual ~SumInt() {};

  bool accept(Row& r) {
    // compute sum
    for (size_t ii = 0; ii < r.width(); ii++) {
      if (r.col_type(ii) == 'I') sum += (long)r.getInt(ii);
    }

    return true;
  }

  void join_delete(Rower* other) {
    SumInt* otherSumInt = dynamic_cast<SumInt*>(other);

    if (!otherSumInt) {
      delete other;
      return;
    }

    sum += otherSumInt->sum;
    delete otherSumInt;
  }

  Rower* clone() {
      return new SumInt;
  }
};


/**
 * Open the given csv file and load it into a dataframe.
 *
 * @param lines the number of lines to read
 * @param filepath  the path to the CSV file
 * @param schema    the schema of the file. schemas which don't match the file
 *                  layout are unspecified.
 * @return  a DataFrame based on the file data (owned by the caller)
 */
static DataFrame* getTestData(long lines, const char* filepath,
                              Schema& schema) {
    DataFrame* df = new DataFrame(schema);

    std::ifstream file{filepath};
    assert(file.good());

    while (lines > 0 || lines == -1) {
        std::string line;
        std::getline(file, line);
        if (file.eof()) break;

        std::stringstream tokens{line};
        Row row{schema};
        for (size_t fieldIdx = 0; fieldIdx < schema.width(); fieldIdx++) {
            std::string token;
            std::getline(tokens, token, ',');
            switch (schema.col_type(fieldIdx)) {
                case 'S':
                    row.set(fieldIdx, std::make_shared<std::string>(token));
                    break;
                case 'B':
                    row.set(fieldIdx, static_cast<bool>(std::stoi(token)));
                    break;
                case 'I':
                    row.set(fieldIdx, std::stoi(token));
                    break;
                case 'D':
                    row.set(fieldIdx, std::stod(token));
                    break;
            }
        }

        df->add_row(row);

        if (lines > 0) lines--;
    }

    file.close();

    return df;
}

// Helper for SumInt test
static DataFrame* getSumIntTestData(size_t lines, const char* filepath) {
    Schema s{"IIIIISIIIIIIDII"};

    return getTestData(lines, filepath, s);
}

TEST(DataframeTest, dataframe) {
    DataFrame* sumIntDf = getSumIntTestData(10000, "data_ints.txt");

    SumInt sumInt;
    sumIntDf->map(sumInt);
    std::cout << "Normal result: " << sumInt.sum << std::endl;
}