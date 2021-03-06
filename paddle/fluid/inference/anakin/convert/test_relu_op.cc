/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <gtest/gtest.h>
#include "paddle/fluid/inference/anakin/convert/op_converter.h"
#include "paddle/fluid/inference/anakin/convert/relu.h"
#include "paddle/fluid/inference/anakin/convert/ut_helper.h"

namespace paddle {
namespace inference {
namespace anakin {

template <typename TargetT>
static void test_activation_op(const std::string& op_type,
                               const platform::DeviceContext& context,
                               bool use_gpu) {
  std::unordered_set<std::string> parameters;
  framework::Scope scope;
  AnakinConvertValidation<TargetT, ::anakin::Precision::FP32> validator(
      parameters, &scope, context, use_gpu);
  validator.DeclInputVar("act-X", {10, 6, 1, 1});
  validator.DeclOutputVar("act-Out", {10, 6, 1, 1});
  framework::OpDesc desc;
  desc.SetType(op_type);
  desc.SetInput("X", {"act-X"});
  desc.SetOutput("Out", {"act-Out"});
  if (op_type == "leaky_relu") {
    desc.SetAttr("alpha", 0.1f);
  }

  LOG(INFO) << "set OP";
  validator.SetOp(*desc.Proto());
  LOG(INFO) << "execute";

  validator.Execute(5);
}

#ifdef PADDLE_WITH_CUDA
TEST(relu_op, gpu) {
  platform::CUDAPlace gpu_place(0);
  platform::CUDADeviceContext ctx(gpu_place);
  test_activation_op<::anakin::saber::NV>("relu", ctx, true);
}

TEST(leaky_relu_op, gpu) {
  platform::CUDAPlace gpu_place(0);
  platform::CUDADeviceContext ctx(gpu_place);
  test_activation_op<::anakin::saber::NV>("leaky_relu", ctx, true);
}
#endif

}  // namespace anakin
}  // namespace inference
}  // namespace paddle

USE_OP(relu);
USE_OP(leaky_relu);
USE_ANAKIN_CONVERTER(relu);
USE_ANAKIN_CONVERTER(leaky_relu);
