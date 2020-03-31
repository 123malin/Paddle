/* Copyright (c) 2020 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/fluid/operators/tdm_sampler_op.h"
#include "paddle/fluid/framework/fleet/kv_maps.h"
#include <vector>
#include "paddle/fluid/framework/fleet/kv_maps.h"
#include "paddle/fluid/framework/op_registry.h"
#include "paddle/fluid/operators/math/sampler.h"
#include "paddle/fluid/platform/enforce.h"

namespace paddle {
namespace operators {

class TDMSamplerOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  void Make() {
    AddInput("Input",
             "Input(Tensor), Input variable which"
             "mapping the leaf node idx of tdm tree"
             "must has the same dtype with tree node idx");
    AddInput("Layer",
             "Layer(Tensor), must has the same dtype with tree node idx"
             "Which nodes are included in each layer");
    AddAttr<bool>("output_labels",
                  "output_labels(bool)"
                  "Whether output sampling results's labels")
        .SetDefault(false);
    AddAttr<bool>("output_positive",
                  "output_positive(bool)"
                  "Whether positive samples are included in the output")
        .SetDefault(false);
    AddAttr<std::vector<int>>(
        "neg_samples_num_list",
        "neg_samples_num_list(python:list[int], C:vector<int>)"
        "The num of negative samples of every layer")
        .SetDefault({});
    AddAttr<std::vector<int>>("layer_offset_lod", "layer offset lod info")
        .SetDefault({});
    AddAttr<int>("seed",
                 "(int) The seed used in sampler. If it is 0, "
                 "the sampler will generate a seed randomly.")
        .SetDefault(0);
    AddOutput("Out",
              "Sampling result lodTensor, with shape [batch_size, layer_num, "
              "neg_num_of_layer]");
    AddOutput("Labels",
              "Labels of sampling result, has the same shape with Out."
              "pos samples mapping value 1, neg sample mapping value 0")
        .AsDispensable();
    AddOutput(
        "Mask",
        "Padding flag of Sampling result, if sampling res comes from padding,"
        "it will be 0, else 1, lodTensor, with shape [batch_size, "
        "layer_num, neg_num_of_layer]");
    AddComment(R"DOC("TDM Sampler")DOC");
  }
};

class TDMSamplerOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;
  void InferShape(framework::InferShapeContext* ctx) const override {
    VLOG(1) << "Begin tdm sampler infershape";
    PADDLE_ENFORCE_EQ(ctx->HasInput("Input"), true,
                      "Inputs(Input) of TdmSampler should not be null.");
    // PADDLE_ENFORCE_EQ(ctx->HasInput("Travel"), true);
    PADDLE_ENFORCE_EQ(ctx->HasInput("Layer"), true);
    VLOG(1) << "Begin infershape vec get";
    auto neg_samples_num_vec =
        ctx->Attrs().Get<std::vector<int>>("neg_samples_num_list");
    auto output_positive_flag = ctx->Attrs().Get<bool>("output_positive");

    int64_t sample_res_length = 0;
    for (auto sample_nums : neg_samples_num_vec) {
      sample_res_length += sample_nums + (int64_t)output_positive_flag;
    }
    VLOG(1) << "Infershape sample_res_length: " << sample_res_length;

    auto ddim = framework::make_ddim({-1, sample_res_length});
    if (ctx->IsRuntime()) {
      // out_dim_0 = input_dims[0];
    } else {
      ctx->SetOutputDim("Out", ddim);
      ctx->SetOutputDim("Labels", ddim);
      ctx->SetOutputDim("Mask", ddim);
    }

    // check vec.size() < tree deepth
    // check every layer neg num <= layer nodes num
  }

 protected:
  framework::OpKernelType GetExpectedKernelType(
      const framework::ExecutionContext& ctx) const override {
    auto data_type = OperatorWithKernel::IndicateVarDataType(ctx, "Input");
    return framework::OpKernelType(data_type, ctx.device_context());
  }
};

}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;

REGISTER_OPERATOR(
    tdm_sampler, ops::TDMSamplerOp, ops::TDMSamplerOpMaker,
    paddle::framework::EmptyGradOpMaker<paddle::framework::OpDesc>,
    paddle::framework::EmptyGradOpMaker<paddle::imperative::OpBase>);
REGISTER_OP_CPU_KERNEL(
    tdm_sampler, ops::TDMSamplerKernel<paddle::platform::CPUPlace, float>,
    ops::TDMSamplerKernel<paddle::platform::CPUPlace, double>,
    ops::TDMSamplerKernel<paddle::platform::CPUPlace, int>,
    ops::TDMSamplerKernel<paddle::platform::CPUPlace, int64_t>);
