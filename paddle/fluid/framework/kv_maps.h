/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace paddle {
namespace framework {

using UUMAP = std::unordered_map<uint64_t, std::vector<uint64_t>>;

class KV_MAPS {
 public:
  KV_MAPS(){};

  static std::shared_ptr<KV_MAPS> GetInstance() {
    if (NULL == s_instance_) {
      s_instance_.reset(new paddle::framework::KV_MAPS());
    }
    return s_instance_;
  }

  static void InitInstance(std::string filename) {
    if (s_instance_.get() == nullptr) {
      s_instance_.reset(new KV_MAPS());
      s_instance_->InitImpl(filename);
    }
  }

  void InitImpl(std::string filename) {
    if (is_initialized_ == false) return;
    data_.clear();
    std::ifstream fin(filename);
    uint64_t size, dimensions, feasign;
    fin >> size >> dimensions;
    std::vector<uint64_t> feasign_values;
    feasign_values.resize(dimensions);
    for (uint64_t i = 0; i < size; i++) {
      fin >> feasign;
      for (uint64_t j = 0; j < dimensions; j++) fin >> feasign_values[j];
    }
    data->insert(
        std::pair<uint64_t, std::vector<uint64_t>>(feasign, feasign_values));
    is_initialized_ = true;
  }

  UUMAP* get_data() { return data_; }

 protected:
  static bool is_initialized_;
  static std::shared_ptr<KV_MAPS> s_instance_;
  std::shared_ptr<UUMAP> data_;
}

}  // namespace framework
}  // namespace paddle
