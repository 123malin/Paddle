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
#include "paddle/fluid/framework/kv_maps.h"
#include <string>
#include "paddle/fluid/pybind/kv_maps_py.h"

namespace py = pybind11;

namespace paddle {
namespace pybind {
void BindKvMaps(py::module* m) {
  py::class_<framework::KV_MAPS>(*m, "KV_MAPS")
      .def(py::init([](std::string filename) {
        VLOG(0) << "using KvMaps";
        KV_MAPS::InitInstance(filename);
        return KV_MAPS::GetInstance();
      }))
}
}  // namespace pybind
}  // namespace paddle
