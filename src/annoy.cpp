// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
//  RcppAnnoy -- Rcpp bindings to Annoy library for Approximate Nearest Neighbours
//
//  Copyright (C) 2014 - 2015  Dirk Eddelbuettel
//
//  This file is part of RcppAnnoy
//
//  RcppAnnoy is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 2 of the License, or
//  (at your option) any later version.
//
//  RcppAnnoy is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with RcppAnnoy.  If not, see <http://www.gnu.org/licenses/>.

// simple C++ modules to wrap to two templated classes from Annoy
//
// uses annoylib.h (from Annoy) and provides R access via Rcpp
//
// Dirk Eddelbuettel, Nov 2014

#include <Rcpp.h>

#if defined(__MINGW32__)
#undef Realloc
#undef Free
#endif

// define R's REprintf as the 'local' error print method for Annoy
#define __ERROR_PRINTER_OVERRIDE__  REprintf

#include "annoylib.h"
#include "kissrandom.h"

template <typename S, typename T>
class AnnoyBase {
protected:
    AnnoyIndexInterface<S, T> *ptr;
    int vector_size;
public:
    AnnoyBase(int n, int type) : vector_size(n) {
        switch (type) {
        case 1:
            ptr = new AnnoyIndex<S, T, Euclidean, Kiss64Random>(n);
            break;
        default:
            ptr = new AnnoyIndex<S, T, Angular, Kiss64Random>(n);
        }
    }
    void addItem(S item, Rcpp::NumericVector dv) {
        std::vector<T> fv(dv.size());
        std::copy(dv.begin(), dv.end(), fv.begin());
        ptr->add_item(item, &fv[0]);
    }
    void   callBuild(int n)               { ptr->build(n);                  }
    void   callSave(std::string filename) { ptr->save(filename.c_str());    }
    void   callLoad(std::string filename) { ptr->load(filename.c_str());    }
    void   callUnload()                   { ptr->unload();                  }
    int    getNItems()                    { return ptr->get_n_items();      }
    T      getDistance(int i, int j)      { return ptr->get_distance(i, j); }
    void   verbose(bool v)                { ptr->verbose(v);                }

    Rcpp::List getNNsByItem(S item, size_t n, size_t search_k, bool include_distances) {
        if (include_distances) {
            std::vector<S> result;
            std::vector<T> distances;
            ptr->get_nns_by_item(item, n, search_k, &result, &distances);

            return Rcpp::List::create(
                Rcpp::Named("item") = result,
                Rcpp::Named("distance") = distances);
        } else {
            std::vector<S> result;
            ptr->get_nns_by_item(item, n, search_k, &result, NULL);
            return Rcpp::List::create(Rcpp::Named("item") = result);
        }
    }

    Rcpp::List getNNsByVector(std::vector<T> fv, size_t n, size_t search_k, bool include_distances) {

        if (include_distances) {
            std::vector<S> result;
            std::vector<T> distances;
            ptr->get_nns_by_vector(&fv[0], n, search_k, &result, &distances);

            return Rcpp::List::create(
                Rcpp::Named("item") = result,
                Rcpp::Named("distance") = distances);
        } else {
            std::vector<S> result;
            ptr->get_nns_by_vector(&fv[0], n, search_k, &result, NULL);

            return Rcpp::List::create(Rcpp::Named("item") = result);
        }
    }

    vector<T> getItemsVector(S item) {
        vector<T> fv(vector_size);
        ptr->get_item(item, &fv[0]);
        return fv;
    }

};

typedef AnnoyBase<int32_t, float> AnnoyIF;
RCPP_EXPOSED_CLASS_NODECL(AnnoyIF)
RCPP_MODULE(AnnoyIF) {
    Rcpp::class_<AnnoyIF>("AnnoyIF")
        .constructor<int32_t, int32_t>("constructor with integer count")

        .method("addItem",        &AnnoyIF::addItem,         "add item")
        .method("build",          &AnnoyIF::callBuild,       "build an index")
        .method("save",           &AnnoyIF::callSave,        "save index to file")
        .method("load",           &AnnoyIF::callLoad,        "load index from file")
        .method("unload",         &AnnoyIF::callUnload,      "unload index")
        .method("getDistance",    &AnnoyIF::getDistance,     "get distance between i and j")
        .method("_getNNsByItem",  &AnnoyIF::getNNsByItem,    "retrieve Nearest Neigbours given item")
        .method("_getNNsByVector", &AnnoyIF::getNNsByVector, "retrieve Nearest Neigbours given vector")
        .method("getItemsVector", &AnnoyIF::getItemsVector,  "retrieve item vector")
        .method("getNItems",      &AnnoyIF::getNItems,       "get N items")
        .method("setVerbose",     &AnnoyIF::verbose,         "set verbose")
        ;
}
