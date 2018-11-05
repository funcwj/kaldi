// featbin/select-feats.cc

// Copyright 2012 Korbinian Riedhammer

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>

#include "base/kaldi-common.h"
#include "matrix/kaldi-matrix.h"
#include "util/common-utils.h"

int main(int argc, char *argv[]) {
  try {
    using namespace kaldi;
    using namespace std;

    const char *usage =
        "Copy features on randomly sampled time index.\n"
        "Usage: sample-feats  <in-rspecifier> <out-wspecifier>\n"
        "See also copy-feats, select-feats, subset-feats, subsample-feats\n";

    ParseOptions po(usage);

    bool vector = false;
    po.Register("vector", &vector,
                "If true, output in vector instead of matrix");

    po.Read(argc, argv);

    if (po.NumArgs() != 2) {
      po.PrintUsage();
      exit(1);
    }

    srand(time(0));

    string rspecifier = po.GetArg(1);
    string wspecifier = po.GetArg(2);

    // set up input (we'll need that to validate the selected indices)
    SequentialBaseFloatMatrixReader kaldi_reader(rspecifier);

    if (kaldi_reader.Done()) {
      KALDI_WARN << "Empty archive provided.";
      return 0;
    }

    // pre-allocated
    Matrix<BaseFloat> sampled_mat(1, kaldi_reader.Value().NumCols());
    Vector<BaseFloat> sampled_vec(kaldi_reader.Value().NumCols());
    // set up output
    BaseFloatMatrixWriter mat_writer;
    BaseFloatVectorWriter vec_writer;

    if (!vector) {
      if (!mat_writer.Open(wspecifier))
        KALDI_ERR << "Error in opening wspecifier: " << wspecifier;
    } else {
      if (!vec_writer.Open(wspecifier))
        KALDI_ERR << "Error in opening wspecifier: " << wspecifier;
    }
    // process all keys
    for (; !kaldi_reader.Done(); kaldi_reader.Next()) {
      const Matrix<BaseFloat> &mat = kaldi_reader.Value();
      // generated time index
      int32 time_index = RandInt(0, mat.NumRows() - 1);
      if (!vector) {
        sampled_mat.Row(0).CopyFromVec(mat.Row(time_index));
        mat_writer.Write(kaldi_reader.Key(), sampled_mat);
      } else {
        sampled_vec.CopyFromVec(mat.Row(time_index));
        vec_writer.Write(kaldi_reader.Key(), sampled_vec);
      }
      KALDI_VLOG(2) << "Random choose time index as " << time_index
                    << " for matrix " << kaldi_reader.Key();
    }

    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what();
    return -1;
  }
}
