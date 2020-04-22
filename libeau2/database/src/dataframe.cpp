/**
 * @file dataframe.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "dataframe.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "column.hpp"
#include "kvstore.hpp"
#include "row.hpp"
#include "rower.hpp"
#include "schema.hpp"
#include "sorer/column.h"  // from 4500ne

// Default constructor is a local DataFrame
DataFrame::DataFrame() : DataFrame(Schema(), true) {}

/** Create a data frame with the same columns as the given df but with no
 * rows or rownmaes */
DataFrame::DataFrame(const DataFrame& df) : DataFrame(df._schema, df._local) {}

/** Create a data frame from a schema and columns. All columns are created
 * empty. */
DataFrame::DataFrame(const Schema& schema, bool local)
    : _schema(schema), _local(local) {
    if (_local) {
        _data.reserve(schema.width());
        char type;
        for (size_t ii = 0; ii < schema.width(); ii++) {
            type = schema.colType(ii);
            switch (type) {
                case 'I':
                    _data.push_back(std::make_shared<Column<int>>());
                    break;
                case 'B':
                    _data.push_back(std::make_shared<Column<bool>>());
                    break;
                case 'D':
                    _data.push_back(std::make_shared<Column<double>>());
                    break;
                case 'S':
                    _data.push_back(std::make_shared<Column<ExtString>>());
                    break;
                default:
                    throw std::invalid_argument("Unsupported type");
            }
        }
    } else {
        _data.reserve(2);
        if (schema.isLocal()) {
            throw std::invalid_argument(
                "Remote DataFrame must have remote Schema");
        }
    }
}

DataFrame::~DataFrame() {}

/** Returns the dataframe's schema. Modifying the schema after a dataframe
 * has been created in undefined. */
Schema& DataFrame::getSchema() { return _schema; }

int DataFrame::getInt(size_t col, size_t row) { return getVal<int>(col, row); }

bool DataFrame::getBool(size_t col, size_t row) {
    return getVal<bool>(col, row);
}

double DataFrame::getDouble(size_t col, size_t row) {
    return getVal<double>(col, row);
}

ExtString DataFrame::getString(size_t col, size_t row) {
    return getVal<ExtString>(col, row);
}

/** Set the fields of the given row object with values from the columns at
 * the given offset.  If the row is not form the same schema as the
 * dataframe, results are undefined.
 */
void DataFrame::fillRow(size_t idx, Row& row) {
    if (idx >= _schema.length()) {
        printf("Out of range row index: %zu\n", idx);
        return;
    }

    row.setIdx(idx);
    for (size_t ii = 0; ii < _schema.width(); ii++) {
        switch (_schema.colType(ii)) {
            case 'I':
                row.set(ii, getInt(ii, idx));
                break;
            case 'B':
                row.set(ii, getBool(ii, idx));
                break;
            case 'D':
                row.set(ii, getDouble(ii, idx));
                break;
            case 'S':
                row.set(ii, getString(ii, idx));
                break;
        }
    }
}

/** Add a row at the end of this dataframe. The row is expected to have
 *  the right schema and be filled with values, otherwise undedined.  */
void DataFrame::add_row(Row& row) {
    for (size_t ii = 0; ii < _schema.width(); ii++) {
        switch (_schema.colType(ii)) {
            case 'I':
                dynamic_cast<Column<int>&>(*_data[ii])
                    .push_back(row.getInt(ii));
                break;
            case 'B':
                dynamic_cast<Column<bool>&>(*_data[ii])
                    .push_back(row.getBool(ii));
                break;
            case 'D':
                dynamic_cast<Column<double>&>(*_data[ii])
                    .push_back(row.getDouble(ii));
                break;
            case 'S':
                dynamic_cast<Column<ExtString>&>(*_data[ii])
                    .push_back(row.getString(ii));
                break;
        }
    }

    _schema.addRow(nullptr);
}

/** The number of rows in the dataframe. */
size_t DataFrame::nrows() { return _schema.length(); }

/** The number of columns in the dataframe.*/
size_t DataFrame::ncols() { return _schema.width(); }

/** Visit rows in order */
void DataFrame::map(Rower& r) {
    for (size_t ii = 0; ii < _schema.length(); ii++) {
        Row row{_schema};
        fillRow(ii, row);
        r.accept(row);
    }
}

/** Create a new dataframe, constructed from rows for which the given Rower
 * returned true from its accept method. */
DataFrame& DataFrame::filter(Rower& r) {
    DataFrame* ret = new DataFrame(_schema);

    for (size_t ii = 0; ii < _schema.length(); ii++) {
        Row row = {_schema};
        fillRow(ii, row);
        if (r.accept(row)) ret->add_row(row);
    }

    return *ret;
}

/** This method clones the Rower and executes the map in parallel. Join is
 * used at the end to merge the results. */
void DataFrame::pmap(Rower& r) {
    // try to get the max number of threads this configuration can run
    unsigned int numThreads = std::thread::hardware_concurrency();
    numThreads = numThreads ? numThreads : 4;  // default to 4
    size_t length = _schema.length();
    size_t partitionSize = length / numThreads;  // partition between threads

    while (partitionSize < 1 && numThreads > 1) {
        partitionSize = length / --numThreads;
    }

    // Create Rowers per thread
    Rower* rowers[numThreads];
    std::thread threads[numThreads];

    // use a lambda to map one thread's deligated partition
    auto partialMap = [this](Rower* rower, size_t start, size_t end) {
        for (size_t ii = start; ii < end; ii++) {
            Row row{_schema};
            fillRow(ii, row);
            rower->accept(row);
        }
    };

    size_t start, end;
    for (size_t ii = 0; ii < numThreads; ii++) {
        if (ii == numThreads - 1)
            rowers[ii] = &r;
        else
            rowers[ii] = r.clone();

        start = (numThreads - ii - 1) * partitionSize;
        end = ii == 0 ? length : start + partitionSize;

        // split off thread
        threads[ii] = std::thread(partialMap, rowers[ii], start, end);
    }

    // Join threads and fold results
    for (size_t ii = 0; ii < numThreads; ii++) {
        threads[ii].join();
        if (ii != 0) {
            rowers[ii]->join_delete(rowers[ii - 1]);
            rowers[ii - 1] = nullptr;
        }
    }
}

/**
 * @brief Creates a database from 4500ne's parsers. There's some data overhead
 * that would be solved by refactoring our code or theirs to share the same
 * column types.
 *
 * @param key   the key in the store
 * @param kv    the store
 * @param set   the set of columns
 */
void DataFrame::fromColumnSet(Key* key, KVStore* kv, ne::ColumnSet* set) {
    auto df = std::make_shared<DataFrame>();

    for (size_t i = 0; i < set->getLength(); i++) {
        ne::BaseColumn* basecol = set->getColumn(i);

        // TODO(mike) this loop is porrly written
        if (basecol->getType() == ne::ColumnType::STRING) {
            auto newCol = std::make_shared<Column<ExtString>>();

            ne::StringColumn* col = dynamic_cast<ne::StringColumn*>(basecol);

            for (size_t i = 0; i < col->getLength(); i++) {
                if (col->isEntryPresent(i))
                    newCol->push_back(
                        std::make_shared<std::string>(col->getEntry(i)));
                else {
                    // TODO(mike) how to handle missing entries?
                    // newCol->push_back()
                }
            }

            df->addCol(newCol);

        } else if (basecol->getType() == ne::ColumnType::INTEGER) {
            auto newCol = std::make_shared<Column<int>>();

            ne::IntegerColumn* col = dynamic_cast<ne::IntegerColumn*>(basecol);

            for (size_t i = 0; i < col->getLength(); i++) {
                if (col->isEntryPresent(i))
                    newCol->push_back(col->getEntry(i));
                else {
                    // newCol->push_back()
                }
            }

            df->addCol(newCol);

        } else if (basecol->getType() == ne::ColumnType::FLOAT) {
            auto newCol = std::make_shared<Column<float>>();

            ne::FloatColumn* col = dynamic_cast<ne::FloatColumn*>(basecol);

            for (size_t i = 0; i < col->getLength(); i++) {
                if (col->isEntryPresent(i))
                    newCol->push_back(col->getEntry(i));
                else {
                    // newCol->push_back()
                }
            }

            df->addCol(newCol);

        } else if (basecol->getType() == ne::ColumnType::BOOL) {
            auto newCol = std::make_shared<Column<bool>>();

            ne::BoolColumn* col = dynamic_cast<ne::BoolColumn*>(basecol);

            for (size_t i = 0; i < col->getLength(); i++) {
                if (col->isEntryPresent(i))
                    newCol->push_back(col->getEntry(i));
                else {
                    // newCol->push_back()
                }
            }

            df->addCol(newCol);

        } else {  // unknown type TODO(mike) handle this
            assert(false);
        }
    }

    kv->push(*key, df);
}

// To be implemented, will use the 4500NE parser
DFPtr fromFile(const char* filename, const Key& key, KVStore* kv) {
    return nullptr;
}

void DataFrame::print() {
    for (size_t i = 0; i < ncols(); i++) {
        std::cout << "Column " << i << ": " << _data.at(i)->str() << std::endl;
    }
}

template <>
void DataFrame::_addRemoteCol(ColPtr<int64_t> col) {
    if (_data.size() != 1 || col->size() != _data[0]->size()) {
        std::cerr << "Invalid home Column for remote DataFrame\n";
    } else {
        _data.push_back(col);
    }
}

template <>
void DataFrame::_addRemoteCol(ColPtr<ExtString> col) {
    if (!_data.empty() || col->size() * _blkSize < _schema.length()) {
        std::cerr << "Invalid name Column for remote DataFrame\n";
    } else {
        _data.push_back(col);
    }
}