/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <arrow/array.h>
#include <gtest/gtest.h>

#include <memory>

#include "codegen/code_generator.h"
#include "codegen/code_generator_factory.h"
#include "tests/test_utils.h"

namespace sparkcolumnarplugin {
namespace codegen {

TEST(TestArrowCompute, AggregateTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int32());
  auto f1 = field("f1", int32());
  auto f2 = field("f2", float64());
  auto f3 = field("f3", float64());
  auto f4 = field("f4", float64());
  auto f5 = field("f5", utf8());
  auto f6 = field("f6", utf8());
  auto f7 = field("f7", boolean());

  auto arg_0 = TreeExprBuilder::MakeField(f0);
  auto arg_1 = TreeExprBuilder::MakeField(f1);
  auto arg_2 = TreeExprBuilder::MakeField(f2);
  auto arg_3 = TreeExprBuilder::MakeField(f3);
  auto arg_4 = TreeExprBuilder::MakeField(f4);
  auto arg_5 = TreeExprBuilder::MakeField(f5);
  auto arg_6 = TreeExprBuilder::MakeField(f6);
  auto arg_7 = TreeExprBuilder::MakeField(f7);

  auto n_sum = TreeExprBuilder::MakeFunction("action_sum", {arg_0}, int64());
  auto n_count = TreeExprBuilder::MakeFunction("action_count", {arg_0}, int64());
  auto n_sum_count = TreeExprBuilder::MakeFunction("action_sum_count", {arg_0}, int64());
  auto n_avg =
      TreeExprBuilder::MakeFunction("action_avgByCount", {arg_2, arg_1}, float64());
  auto n_min = TreeExprBuilder::MakeFunction("action_min", {arg_0}, int64());
  auto n_max = TreeExprBuilder::MakeFunction("action_max", {arg_0}, int64());
  auto n_stddev = TreeExprBuilder::MakeFunction("action_stddev_samp_final",
                                                {arg_2, arg_3, arg_4}, float64());
  auto n_count_literal =
      TreeExprBuilder::MakeFunction("action_countLiteral_1", {}, int64());
  auto n_min_str = TreeExprBuilder::MakeFunction("action_min", {arg_5}, utf8());
  auto n_max_str = TreeExprBuilder::MakeFunction("action_max", {arg_5}, utf8());

  auto ignore_nulls_node = std::make_shared<::gandiva::LiteralNode>(
      arrow::boolean(), ::gandiva::LiteralHolder(true), false);
  // The specified return type (utf8, here) doesn't take effect. Actually, the return
  // types are utf8, boolean.
  auto n_utf8_first_partial = TreeExprBuilder::MakeFunction(
      "action_first_partial", {arg_6, ignore_nulls_node}, int32());
  auto n_utf8_first_final =
      TreeExprBuilder::MakeFunction("action_first_final", {arg_6, arg_7}, utf8());
  // For int32 input type test.
  auto n_int32_first_partial = TreeExprBuilder::MakeFunction(
      "action_first_partial", {arg_0, ignore_nulls_node}, int32());
  auto n_int32_first_final =
      TreeExprBuilder::MakeFunction("action_first_final", {arg_0, arg_7}, int32());

  auto f_sum = field("sum", int64());
  auto f_count = field("count", int64());
  auto f_sum_count = field("sum_count", int64());
  auto f_avg = field("avg", float64());
  auto f_min = field("min", int64());
  auto f_max = field("max", int64());
  auto f_stddev = field("stddev", float64());
  auto f_count_literal = field("count_all", int64());
  auto f_min_str = field("min_str", utf8());
  auto f_max_str = field("max_str", utf8());
  // first_partial has two output, one for first value and the other for value set.
  auto f_utf8_first_partial_1 = field("utf8_first_partial_1", utf8());
  auto f_first_partial_2 = field("first_partial_2", boolean());
  auto f_utf8_first_final = field("utf8_first_final", utf8());
  // For int32 input to test first agg func.
  auto f_int32_first_partial_1 = field("int32_first_partial_1", int32());
  auto f_int32_first_final = field("int32_first_final", int32());

  auto f_res = field("res", int32());

  auto n_proj = TreeExprBuilder::MakeFunction(
      "aggregateExpressions", {arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7},
      uint32());
  auto n_action = TreeExprBuilder::MakeFunction(
      "aggregateActions",
      {n_sum, n_count, n_sum_count, n_avg, n_min, n_max, n_stddev, n_count_literal,
       n_min_str, n_max_str, n_utf8_first_partial, n_utf8_first_final,
       n_int32_first_partial, n_int32_first_final},
      uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_sum), TreeExprBuilder::MakeField(f_count),
       TreeExprBuilder::MakeField(f_sum_count), TreeExprBuilder::MakeField(f_count),
       TreeExprBuilder::MakeField(f_avg), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max), TreeExprBuilder::MakeField(f_stddev),
       TreeExprBuilder::MakeField(f_count_literal), TreeExprBuilder::MakeField(f_min_str),
       TreeExprBuilder::MakeField(f_max_str),
       TreeExprBuilder::MakeField(f_utf8_first_partial_1),
       TreeExprBuilder::MakeField(f_first_partial_2),
       TreeExprBuilder::MakeField(f_utf8_first_final),
       TreeExprBuilder::MakeField(f_int32_first_partial_1),
       TreeExprBuilder::MakeField(f_first_partial_2),
       TreeExprBuilder::MakeField(f_int32_first_final)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_sum), TreeExprBuilder::MakeField(f_count),
       TreeExprBuilder::MakeField(f_sum_count), TreeExprBuilder::MakeField(f_count),
       TreeExprBuilder::MakeField(f_avg), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max), TreeExprBuilder::MakeField(f_stddev),
       TreeExprBuilder::MakeField(f_count_literal), TreeExprBuilder::MakeField(f_min_str),
       TreeExprBuilder::MakeField(f_max_str),
       TreeExprBuilder::MakeField(f_utf8_first_partial_1),
       TreeExprBuilder::MakeField(f_first_partial_2),
       TreeExprBuilder::MakeField(f_utf8_first_final),
       TreeExprBuilder::MakeField(f_int32_first_partial_1),
       TreeExprBuilder::MakeField(f_first_partial_2),
       TreeExprBuilder::MakeField(f_int32_first_final)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  auto sch = arrow::schema({f0, f1, f2, f3, f4, f5, f6, f7});
  std::vector<std::shared_ptr<Field>> ret_types = {f_sum,
                                                   f_count,
                                                   f_sum,
                                                   f_count,
                                                   f_avg,
                                                   f_min,
                                                   f_max,
                                                   f_stddev,
                                                   f_count_literal,
                                                   f_min_str,
                                                   f_max_str,
                                                   f_utf8_first_partial_1,
                                                   f_first_partial_2,
                                                   f_utf8_first_final,
                                                   f_int32_first_partial_1,
                                                   f_first_partial_2,
                                                   f_int32_first_final};
  ///////////////////// Calculation //////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, {aggr_expr}, ret_types, &expr, true));

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::string> input_data_string = {
      "[1, 2, 3, 4, 5, null, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[1, 2, 3, 4, 5, null, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      R"(["BJK", "SH", "SZ", "NY", "WH", "WH", "AU", "BJ", "SH", "DL", "cD", "CD", 
          "Bj", "LA", "HZ", "LA", "WH", "NY", "WH", "WH"])",
      R"(["ZZ", "SH", "SZ", "NY", "WH", "WH", "AU", "BJ", "SH", "DL", "cD", "CD", 
          "Bj", "LA", "HZ", "LA", "WH", "NY", "WH", "WH"])",
      "[true, false, true, false, false, true, false, true, false, false, true, "
      "false, true, false, false, true, false, true, false, false]"};
  MakeInputBatch(input_data_string, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));
  std::vector<std::string> input_data_2_string = {
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[7, 8, 4, 5, 6, 1, 34, 54, 65, 66, 78, 12, 32, 24, 32, 45, 12, 24, 35, "
      "46]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      R"(["BJ", "SHL", "SZ", "NY", "WH", "WH", "au", "BJ", "SH", "DL", "cD", "CD", 
          "Bj", "LA", "HZ", "LA", "WH", "NY", "WHZ", "wH"])",
      R"(["BJ", "SHL", "SZ", "NY", "WH", "WH", "au", "BJ", "SH", "DL", "cD", "CD", 
          "Bj", "LA", "HZ", "LA", "WH", "NY", "WHZ", "wH"])",
      "[true, false, true, false, false, true, false, true, false, false, true, "
      "false, true, false, false, true, false, true, false, false]"};
  MakeInputBatch(input_data_2_string, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::vector<std::string> expected_result_string = {
      "[221]",  "[39]",      "[221]", "[39]",      "[4.40724]", "[1]",
      "[10]",   "[17.2996]", "[40]",  R"(["AU"])", R"(["wH"])", R"(["ZZ"])",
      "[true]", R"(["ZZ"])", "[1]",   "[true]",    "[1]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    // auto status = arrow::PrettyPrint(*result_batch.get(), 2, &std::cout);
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, MinMaxNaNTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", float64());

  auto arg_0 = TreeExprBuilder::MakeField(f0);

  auto n_min = TreeExprBuilder::MakeFunction("action_min_true", {arg_0}, int64());
  auto n_max = TreeExprBuilder::MakeFunction("action_max_true", {arg_0}, int64());

  auto f_min = field("min", float64());
  auto f_max = field("max", float64());
  auto f_res = field("res", int32());

  auto n_proj = TreeExprBuilder::MakeFunction("aggregateExpressions", {arg_0}, uint32());
  auto n_action =
      TreeExprBuilder::MakeFunction("aggregateActions", {n_min, n_max}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_min), TreeExprBuilder::MakeField(f_max)}, uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_min), TreeExprBuilder::MakeField(f_max)}, uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  auto sch = arrow::schema({f0});
  std::vector<std::shared_ptr<Field>> ret_types = {f_min, f_max};
  ///////////////////// Calculation //////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, {aggr_expr}, ret_types, &expr, true));

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::string> input_data_string = {
      "[1, 2, 3, 4, 5, NaN, 6, 7, 8, 9, 10, 16, 19, 42, 78, 12, 5, NaN, 11, 19]"};
  MakeInputBatch(input_data_string, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));
  std::vector<std::string> input_data_2_string = {
      "[16, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 18, 19, 12, 13]"};
  MakeInputBatch(input_data_2_string, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::vector<std::string> expected_result_string = {"[1]", "[NaN]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByMinMaxNaNTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int64());
  auto f1 = field("f1", float64());

  auto f_unique = field("unique", int64());
  auto f_min = field("min", float64());
  auto f_max = field("max", float64());
  auto f_res = field("res", int32());

  auto arg0 = TreeExprBuilder::MakeField(f0);
  auto arg1 = TreeExprBuilder::MakeField(f1);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_min = TreeExprBuilder::MakeFunction("action_min_true", {arg1}, uint32());
  auto n_max = TreeExprBuilder::MakeFunction("action_max_true", {arg1}, uint32());
  auto n_proj =
      TreeExprBuilder::MakeFunction("aggregateExpressions", {arg0, arg1}, uint32());
  auto n_action = TreeExprBuilder::MakeFunction("aggregateActions",
                                                {n_groupby, n_min, n_max}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0, f1});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_min, f_max};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {
      "[1, 2, 3, 4, 5, null, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[1, 2, 3, 4, 5, NaN, 6, 7, 8, 9, 10, 16, 19, 42, 78, 12, 5, NaN, 11, 19]"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_2 = {
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[16, 7, 8, 9, 10, NaN, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 18, 19, 12, 13]"};
  MakeInputBatch(input_data_2, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {
      "[1, 2, 3, 4, 5, null, 6, 7, 8, 9, 10]", "[1, 2, 3, 4, 5, NaN, 6, 7, 8, 9, 10]",
      "[19, 9, 12, 78, NaN, NaN, 16, 7, 8, 9, NaN]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, AggregateAllNullTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int32());

  auto arg_0 = TreeExprBuilder::MakeField(f0);

  auto n_sum = TreeExprBuilder::MakeFunction("action_sum", {arg_0}, int64());

  auto f_sum = field("sum", int64());
  auto f_res = field("res", int32());

  auto n_proj = TreeExprBuilder::MakeFunction("aggregateExpressions", {arg_0}, uint32());
  auto n_action = TreeExprBuilder::MakeFunction("aggregateActions", {n_sum}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema", {TreeExprBuilder::MakeField(f_sum)}, uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions", {TreeExprBuilder::MakeField(f_sum)}, uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  auto sch = arrow::schema({f0});
  std::vector<std::shared_ptr<Field>> ret_types = {f_sum};
  ///////////////////// Calculation //////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, {aggr_expr}, ret_types, &expr, true));

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::string> input_data_string = {
      "[null, null, null, null, null, null, null, null, null, null, null, null]"};
  MakeInputBatch(input_data_string, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));
  std::vector<std::string> input_data_2_string = {
      "[null, null, null, null, null, null, null, null, null, null, null, null]"};
  MakeInputBatch(input_data_2_string, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::vector<std::string> expected_result_string = {"[null]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByAggregateTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int64());
  auto f1 = field("f1", uint32());
  auto f2 = field("f2", float64());
  auto f3 = field("f3", float64());
  auto f4 = field("f4", float64());

  auto f_unique = field("unique", int64());
  auto f_sum = field("sum", float64());
  auto f_sum_count_multiply = field("sum_count_multiply", float64());
  auto f_count = field("count", int64());
  auto f_min = field("min", uint32());
  auto f_max = field("max", uint32());
  auto f_avg = field("avg", float64());
  auto f_stddev = field("stddev", float64());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);
  auto arg1 = TreeExprBuilder::MakeField(f1);
  auto arg2 = TreeExprBuilder::MakeField(f2);
  auto arg3 = TreeExprBuilder::MakeField(f3);
  auto arg4 = TreeExprBuilder::MakeField(f4);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_sum_count = TreeExprBuilder::MakeFunction("action_sum_count", {arg1}, uint32());
  auto n_min = TreeExprBuilder::MakeFunction("action_min", {arg1}, uint32());
  auto n_max = TreeExprBuilder::MakeFunction("action_max", {arg1}, uint32());
  auto n_avg = TreeExprBuilder::MakeFunction("action_avgByCount", {arg2, arg0}, uint32());
  auto n_stddev = TreeExprBuilder::MakeFunction("action_stddev_samp_final",
                                                {arg2, arg3, arg4}, uint32());
  auto n_proj = TreeExprBuilder::MakeFunction("aggregateExpressions",
                                              {arg0, arg1, arg2, arg3, arg4}, uint32());
  auto n_action = TreeExprBuilder::MakeFunction(
      "aggregateActions", {n_groupby, n_sum_count, n_min, n_max, n_avg, n_stddev},
      uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_sum),
       TreeExprBuilder::MakeField(f_count), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max), TreeExprBuilder::MakeField(f_avg),
       TreeExprBuilder::MakeField(f_stddev)},
      uint32());
  auto n_multiply = TreeExprBuilder::MakeFunction(
      "multiply",
      {
          TreeExprBuilder::MakeField(f_sum),
          TreeExprBuilder::MakeFunction("castFloat8",
                                        {TreeExprBuilder::MakeField(f_count)}, float64()),
      },
      float64());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), n_multiply,
       TreeExprBuilder::MakeField(f_min), TreeExprBuilder::MakeField(f_avg),
       TreeExprBuilder::MakeField(f_stddev)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0, f1, f2, f3, f4});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_sum_count_multiply, f_min,
                                                   f_avg, f_stddev};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {
      "[1, 2, 3, 4, 5, null, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[1, 2, 3, 4, 5, 5, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_2 = {
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[7, 8, 4, 5, 6, 1, 34, 54, 65, 66, 78, 12, 32, 24, 32, 45, 12, 24, 35, "
      "46]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]"};
  MakeInputBatch(input_data_2, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {
      "[1, 2, 3, 4, 5, null, 6, 7, 8, 9, 10]",
      "[25, 18, 12, 64, 125, 5, 150, 63, 32, 144, 360]",
      "[1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10]",
      "[16.4, 6.5, 5, 5.875, 5.48, null, 6.1, 6.61905, 3.0625, 2.63889, "
      "2.06667]",
      "[8.49255, 6.93137, 7.6489, 13.5708, 17.4668, 1.41421, 8.52779, 6.23633, "
      "5.58903, "
      "12.535, 24.3544]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByBoolTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", boolean());

  auto f_unique = field("unique", boolean());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_proj = TreeExprBuilder::MakeFunction("aggregateExpressions", {arg0}, uint32());

  auto n_action =
      TreeExprBuilder::MakeFunction("aggregateActions", {n_groupby}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema", {TreeExprBuilder::MakeField(f_unique)}, uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions", {TreeExprBuilder::MakeField(f_unique)}, uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {"[true, true, true, true, true]"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {"[true]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByMaxForBoolTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int64());
  auto f1 = field("f1", boolean());

  auto f_unique = field("unique", int64());
  auto f_max = field("max", boolean());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);
  auto arg1 = TreeExprBuilder::MakeField(f1);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_max = TreeExprBuilder::MakeFunction("action_max", {arg1}, uint32());
  auto n_proj =
      TreeExprBuilder::MakeFunction("aggregateExpressions", {arg0, arg1}, uint32());
  auto n_action =
      TreeExprBuilder::MakeFunction("aggregateActions", {n_groupby, n_max}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_max)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_max)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0, f1});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_max};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {
      "[1, 1, 2, 3, 3, 4, 4, 5, 5, 5]",
      "[true, false, true, false, null, null, null, null, true, false]"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {"[1, 2, 3, 4, 5]",
                                                     "[true, true, false, null, true]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByMaxMinStringTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int64());
  auto f1 = field("f1", utf8());

  auto f_unique = field("unique", int64());
  auto f_max = field("max", utf8());
  auto f_min = field("min", utf8());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);
  auto arg1 = TreeExprBuilder::MakeField(f1);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_min = TreeExprBuilder::MakeFunction("action_min", {arg1}, uint32());
  auto n_max = TreeExprBuilder::MakeFunction("action_max", {arg1}, uint32());

  auto n_proj =
      TreeExprBuilder::MakeFunction("aggregateExpressions", {arg0, arg1}, uint32());
  auto n_action = TreeExprBuilder::MakeFunction("aggregateActions",
                                                {n_groupby, n_min, n_max}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_min),
       TreeExprBuilder::MakeField(f_max)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0, f1});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_min, f_max};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {
      "[1, 2, 3, 4, 5, null, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      R"(["BJK", "SH", "SZ", "NY", "WH", "WH", "AU", "BJ", "SH", "DL", "cD", "CD", 
          "Bj", "LA", "HZ", "LA", "WH", "NY", "WH", "WH"])"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {
      "[1, 2, 3, 4, 5, null]", R"(["BJ", "DL", "LA", "AU", "NY", "WH"])",
      R"(["cD", "SH", "SZ", "NY", "WH", "WH"])"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByCountAll) {
  auto f0 = field("f0", utf8());
  auto f_unique = field("unique", utf8());
  auto f_count = field("count", int64());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_count = TreeExprBuilder::MakeFunction("action_countLiteral_1", {arg0}, uint32());
  auto n_proj = TreeExprBuilder::MakeFunction("aggregateExpressions", {arg0}, uint32());
  auto n_action =
      TreeExprBuilder::MakeFunction("aggregateActions", {n_groupby, n_count}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_count)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_count)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_count};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true))

  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  ////////////////////// calculation /////////////////////

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  std::vector<std::string> input_data = {
      R"(["BJ", "SH", "SZ", "HZ", "WH", "WH", "HZ", "BJ", "SH", "SH", "BJ", "BJ", "BJ", "HZ", "HZ", "SZ", "WH", "WH", "WH", "WH"])"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_2 = {
      R"(["CD", "DL", "NY", "LA", "AU", "AU", "LA", "CD", "DL", "DL", "CD", "CD", "CD", "LA", "LA", "NY", "AU", "AU", "AU", "AU"])"};
  MakeInputBatch(input_data_2, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_3 = {
      R"(["BJ", "SH", "SZ", "NY", "WH", "WH", "AU", "BJ", "SH", "DL", "CD", "CD", "BJ", "LA", "HZ", "LA", "WH", "NY", "WH", "WH"])"};
  MakeInputBatch(input_data_3, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////

  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::vector<std::string> expected_result_string = {
      R"(["BJ", "SH", "SZ", "HZ", "WH", "CD", "DL", "NY" ,"LA", "AU"])",
      "[8, 5, 3, 5, 11, 7, 4, 4, 6, 7]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByCountOnMutipleCols) {
  auto f0 = field("f0", utf8());
  auto f1 = field("f1", utf8());
  auto f2 = field("f2", utf8());
  auto f_unique = field("unique", utf8());
  auto f_count = field("count", int64());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);
  auto arg1 = TreeExprBuilder::MakeField(f1);
  auto arg2 = TreeExprBuilder::MakeField(f2);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_count = TreeExprBuilder::MakeFunction("action_count", {arg1, arg2}, uint32());
  auto n_proj =
      TreeExprBuilder::MakeFunction("aggregateExpressions", {arg0, arg1, arg2}, uint32());
  auto n_action =
      TreeExprBuilder::MakeFunction("aggregateActions", {n_groupby, n_count}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_count)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_count)},
      uint32());
  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0, f1, f2});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_count};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true))

  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  ////////////////////// calculation /////////////////////

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  std::vector<std::string> input_data = {R"(["a", "a", "a", "x", "x"])",
                                         R"(["b", "b", "b", "y", "q"])",
                                         R"([null, "c", "d", "z", null])"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_2 = {R"(["b", "a", "b", "a", "x"])",
                                           R"(["b", "b", "b", null, "q"])",
                                           R"(["c", null, "d", "z", null])"};
  MakeInputBatch(input_data_2, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////

  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::vector<std::string> expected_result_string = {R"(["a", "x", "b"])", "[2, 1, 2]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByTwoAggregateTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f0 = field("f0", int64());
  auto f1 = field("f1", uint32());
  auto f2 = field("f2", float64());
  auto f3 = field("f3", float64());
  auto f4 = field("f4", float64());
  auto f5 = field("f5", utf8());

  auto f_unique = field("unique_int64", int64());
  auto f_unique_1 = field("unique_str", utf8());
  auto f_sum = field("sum", float64());
  auto f_sum_count_multiply = field("sum_count_multiply", float64());
  auto f_count = field("count", int64());
  auto f_min = field("min", uint32());
  auto f_max = field("max", uint32());
  auto f_avg = field("avg", float64());
  auto f_stddev = field("stddev", float64());
  auto f_res = field("res", uint32());

  auto arg0 = TreeExprBuilder::MakeField(f0);
  auto arg1 = TreeExprBuilder::MakeField(f1);
  auto arg2 = TreeExprBuilder::MakeField(f2);
  auto arg3 = TreeExprBuilder::MakeField(f3);
  auto arg4 = TreeExprBuilder::MakeField(f4);
  auto arg5 = TreeExprBuilder::MakeField(f5);

  auto n_groupby = TreeExprBuilder::MakeFunction("action_groupby", {arg0}, uint32());
  auto n_groupby_5 = TreeExprBuilder::MakeFunction("action_groupby", {arg5}, uint32());
  auto n_sum_count = TreeExprBuilder::MakeFunction("action_sum_count", {arg1}, uint32());
  auto n_min = TreeExprBuilder::MakeFunction("action_min", {arg1}, uint32());
  auto n_max = TreeExprBuilder::MakeFunction("action_max", {arg1}, uint32());
  auto n_avg = TreeExprBuilder::MakeFunction("action_avgByCount", {arg2, arg0}, uint32());
  auto n_stddev = TreeExprBuilder::MakeFunction("action_stddev_samp_final",
                                                {arg2, arg3, arg4}, uint32());
  auto n_proj = TreeExprBuilder::MakeFunction(
      "aggregateExpressions", {arg0, arg1, arg2, arg3, arg4, arg5}, uint32());
  auto n_action = TreeExprBuilder::MakeFunction(
      "aggregateActions",
      {n_groupby, n_groupby_5, n_sum_count, n_min, n_max, n_avg, n_stddev}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_unique_1),
       TreeExprBuilder::MakeField(f_sum), TreeExprBuilder::MakeField(f_count),
       TreeExprBuilder::MakeField(f_min), TreeExprBuilder::MakeField(f_max),
       TreeExprBuilder::MakeField(f_avg), TreeExprBuilder::MakeField(f_stddev)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_unique_1),
       TreeExprBuilder::MakeField(f_sum), TreeExprBuilder::MakeField(f_count),
       TreeExprBuilder::MakeField(f_min), TreeExprBuilder::MakeField(f_max),
       TreeExprBuilder::MakeField(f_avg), TreeExprBuilder::MakeField(f_stddev)},
      uint32());

  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f0, f1, f2, f3, f4, f5});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_unique_1, f_sum, f_count,
                                                   f_min,    f_max,      f_avg, f_stddev};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {
      "[1, 2, 3, 4, 5, null, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[1, 2, 3, 4, 5, 5, 4, 1, 2, 2, 1, 1, 1, 4, 4, 3, 5, 5, 5, 5]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      R"(["BJ", "SH", "HZ", "BH", "NY", "SH", "BH", "BJ", "SH", "SH", "BJ", "BJ", "BJ", "BH", "BH", "HZ", "NY", "NY", "NY", "NY"])"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_2 = {
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[6, 7, 8, 9, 10, 10, 9, 6, 7, 7, 6, 6, 6, 9, 9, 8, 10, 10, 10, 10]",
      "[7, 8, 4, 5, 6, 1, 34, 54, 65, 66, 78, 12, 32, 24, 32, 45, 12, 24, 35, "
      "46]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      "[2, 4, 5, 7, 8, 2, 45, 32, 23, 12, 14, 16, 18, 19, 23, 25, 57, 59, 12, "
      "1]",
      R"(["BJ", "SH", "TK", "SH", "PH", "PH", "SH", "BJ", "SH", "SH", "BJ", "BJ", "BJ", "SH", "SH", "TK", "PH", "PH", "PH", "PH"])"};
  MakeInputBatch(input_data_2, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {
      "[1, 2, 3, 4, 5, null, 6, 7, 8, 9, 10]",
      R"(["BJ", "SH", "HZ", "BH", "NY", "SH", "BJ", "SH", "TK", "SH", "PH"])",
      "[5, 6, 6, 16, 25, 5, 30, 21, 16, 36, 60]",
      "[5, 3, 2, 4, 5, 1, 5, 3, 2, 4, 6]",
      "[1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10]",
      "[1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10]",
      "[16.4, 6.5, 5, 5.875, 5.48, null, 6.1, 6.61905, 3.0625, 2.63889, "
      "2.06667]",
      "[8.49255, 6.93137, 7.6489, 13.5708, 17.4668, 1.41421, 8.52779, 6.23633, "
      "5.58903, "
      "12.535, 24.3544]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

TEST(TestArrowCompute, GroupByHashAggregateWithCaseWhenTest) {
  ////////////////////// prepare expr_vector ///////////////////////
  auto f_0 = field("f0", utf8());
  auto f_1 = field("f1", float64());
  auto f_unique = field("unique", utf8());
  auto f_sum = field("sum", float64());
  auto f_res = field("dummy_res", uint32());

  auto arg_unique = TreeExprBuilder::MakeField(f_unique);
  auto arg_sum = TreeExprBuilder::MakeField(f_sum);
  auto n_groupby = TreeExprBuilder::MakeFunction(
      "action_groupby", {TreeExprBuilder::MakeField(f_0)}, uint32());

  auto n_when = TreeExprBuilder::MakeFunction(
      "equal",
      {TreeExprBuilder::MakeField(f_0), TreeExprBuilder::MakeStringLiteral("BJ")},
      arrow::boolean());
  auto n_then = TreeExprBuilder::MakeFunction(
      "multiply",
      {TreeExprBuilder::MakeField(f_1), TreeExprBuilder::MakeLiteral((double)0.3)},
      float64());
  auto n_else = TreeExprBuilder::MakeFunction(
      "multiply",
      {TreeExprBuilder::MakeField(f_1), TreeExprBuilder::MakeLiteral((double)1.3)},
      float64());
  auto n_projection = TreeExprBuilder::MakeIf(n_when, n_then, n_else, float64());

  auto n_sum = TreeExprBuilder::MakeFunction("action_sum", {n_projection}, uint32());

  auto n_proj = TreeExprBuilder::MakeFunction(
      "aggregateExpressions",
      {TreeExprBuilder::MakeField(f_0), TreeExprBuilder::MakeField(f_1)}, uint32());
  auto n_action =
      TreeExprBuilder::MakeFunction("aggregateActions", {n_groupby, n_sum}, uint32());
  auto n_result = TreeExprBuilder::MakeFunction(
      "resultSchema",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_sum)},
      uint32());
  auto n_result_expr = TreeExprBuilder::MakeFunction(
      "resultExpressions",
      {TreeExprBuilder::MakeField(f_unique), TreeExprBuilder::MakeField(f_sum)},
      uint32());

  auto n_aggr = TreeExprBuilder::MakeFunction(
      "hashAggregateArrays", {n_proj, n_action, n_result, n_result_expr}, uint32());
  auto n_child = TreeExprBuilder::MakeFunction("standalone", {n_aggr}, uint32());
  auto aggr_expr = TreeExprBuilder::MakeExpression(n_child, f_res);

  std::vector<std::shared_ptr<::gandiva::Expression>> expr_vector = {aggr_expr};

  auto sch = arrow::schema({f_0, f_1});
  std::vector<std::shared_ptr<Field>> ret_types = {f_unique, f_sum};

  /////////////////////// Create Expression Evaluator ////////////////////
  std::shared_ptr<CodeGenerator> expr;
  arrow::compute::ExecContext ctx;
  ASSERT_NOT_OK(
      CreateCodeGenerator(ctx.memory_pool(), sch, expr_vector, ret_types, &expr, true));
  std::shared_ptr<arrow::RecordBatch> input_batch;
  std::vector<std::shared_ptr<arrow::RecordBatch>> output_batch_list;

  std::shared_ptr<ResultIterator<arrow::RecordBatch>> aggr_result_iterator;
  std::shared_ptr<ResultIteratorBase> aggr_result_iterator_base;
  ASSERT_NOT_OK(expr->finish(&aggr_result_iterator_base));
  aggr_result_iterator = std::dynamic_pointer_cast<ResultIterator<arrow::RecordBatch>>(
      aggr_result_iterator_base);

  ////////////////////// calculation /////////////////////
  std::vector<std::string> input_data = {
      R"(["BJ", "SH", "SZ", "HZ", "WH", "WH", "HZ", "BJ", "SH", "SH", "BJ", "BJ", "BJ",
"HZ", "HZ", "SZ", "WH", "WH", "WH", "WH"])",
      "[1, 4, 9, 16, 25, 25, 16, 1, 3, 5, 1, 1, 1, 16, 16, 9, 25, 25, 25, 25]"};
  MakeInputBatch(input_data, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_2 = {
      R"(["CD", "DL", "NY", "LA", "AU", "AU", "LA", "CD", "DL", "DL", "CD", "CD", "CD",
"LA", "LA", "NY", "AU", "AU", "AU", "AU"])",
      "[36, 49, 64, 81, 100, 100, 81, 36, 49, 49, 36, 36, 36, 81, 81, 64, 100, "
      "100, 100, "
      "100]"};
  MakeInputBatch(input_data_2, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  std::vector<std::string> input_data_3 = {
      R"(["BJ", "SH", "SZ", "NY", "WH", "WH", "AU", "BJ", "SH", "DL", "CD", "CD", "BJ",
"LA", "HZ", "LA", "WH", "NY", "WH", "WH"])",
      "[1, 4, 9, 64, 25, 25, 100, 1, 4, 49, 36, 36, 1, 81, 16, 81, 25, 64, 25, "
      "25]"};
  MakeInputBatch(input_data_3, sch, &input_batch);
  ASSERT_NOT_OK(aggr_result_iterator->ProcessAndCacheOne(input_batch->columns()));

  ////////////////////// Finish //////////////////////////
  std::shared_ptr<arrow::RecordBatch> result_batch;
  std::shared_ptr<arrow::RecordBatch> expected_result;
  std::vector<std::string> expected_result_string = {
      R"(["BJ", "SH", "SZ", "HZ", "WH", "CD", "DL", "NY" ,"LA", "AU"])",
      "[2.4, 26, 35.1, 104, 357.5,  327.6, 254.8, 332.8, 631.8, 910]"};
  auto res_sch = arrow::schema(ret_types);
  MakeInputBatch(expected_result_string, res_sch, &expected_result);
  if (aggr_result_iterator->HasNext()) {
    ASSERT_NOT_OK(aggr_result_iterator->Next(&result_batch));
    ASSERT_NOT_OK(Equals(*expected_result.get(), *result_batch.get()));
  }
}

}  // namespace codegen
}  // namespace sparkcolumnarplugin
