//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// txn_abort_test.cpp
//
// Identification: test/txn/txn_abort_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <chrono>  // NOLINT
#include <exception>
#include <memory>
#include <mutex>  // NOLINT
#include <random>
#include <string>
#include <thread>  // NOLINT
#include "common/bustub_instance.h"
#include "common/macros.h"
#include "concurrency/transaction.h"
#include "execution/execution_common.h"
#include "fmt/core.h"
#include "txn_common.h"  // NOLINT

namespace bustub {

// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST(TxnAbortTest, DISABLED_SimpleAbortTest) {  // NOLINT
  fmt::println(stderr, "--- SimpleAbortTest: Setup without primary key ---");
  auto bustub = std::make_unique<BusTubInstance>();
  Execute(*bustub, "CREATE TABLE maintable(a int, b int)");
  auto table_info = bustub->catalog_->GetTable("maintable");

  fmt::println(stderr, "A: INSERT then ABORT - ensure nothing persists");
  {
    auto txn1 = BeginTxn(*bustub, "txn1");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1,10), (1,11), (2,20)"));
    WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                  IntResult{{1, 10}, {1, 11}, {2, 20}}));
    WithTxn(txn1, AbortTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_after_abort_insert");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b", IntResult{}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "B: Prepare committed baseline for UPDATE/DELETE abort tests");
  {
    auto txn2 = BeginTxn(*bustub, "txn2");
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1,10), (2,200), (3,300)"));
    WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_baseline");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {2, 200}, {3, 300}}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "C: UPDATE then ABORT - values should revert");
  {
    auto txn3 = BeginTxn(*bustub, "txn3");
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET b = 0 WHERE a >= 2"));
    WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                  IntResult{{1, 10}, {2, 0}, {3, 0}}));
    WithTxn(txn3, AbortTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_after_abort_update");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {2, 200}, {3, 300}}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "D: DELETE then ABORT - rows should reappear");
  {
    auto txn4 = BeginTxn(*bustub, "txn4");
    WithTxn(txn4, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable"));
    WithTxn(txn4, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b", IntResult{}));
    WithTxn(txn4, AbortTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_after_abort_delete");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {2, 200}, {3, 300}}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "E: Commit with duplicates (no primary key) and verify");
  {
    auto txn5 = BeginTxn(*bustub, "txn5");
    WithTxn(txn5, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1,999), (1,1000)"));
    WithTxn(txn5, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET b = b + 1 WHERE a = 3"));
    WithTxn(txn5, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                  IntResult{{1, 10}, {1, 999}, {1, 1000}, {2, 200}, {3, 301}}));
    WithTxn(txn5, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "F: Final verification from fresh transaction");
  {
    auto txn_final = BeginTxn(*bustub, "txn_final");
    WithTxn(txn_final, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {1, 999}, {1, 1000}, {2, 200}, {3, 301}}));
    WithTxn(txn_final, CommitTxn(*bustub, _var, _txn));
  }
}

TEST(TxnAbortTest, DISABLED_AbortIndexTest) {  // NOLINT
  fmt::println(stderr, "--- AbortIndexTest1: Simple Abort ---");
  {
    auto bustub = std::make_unique<BusTubInstance>();
    EnsureIndexScan(*bustub);
    Execute(*bustub, "CREATE TABLE maintable(a int primary key, b int)");
    auto table_info = bustub->catalog_->GetTable("maintable");
    auto txn1 = BeginTxn(*bustub, "txn1");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 233), (2, 2333)"));
    WithTxn(txn1, AbortTxn(*bustub, _var, _txn));
    TxnMgrDbg("after abort", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    auto txn2 = BeginTxn(*bustub, "txn2");
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 2333), (2, 23333), (3, 233)"));
    WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, "SELECT * FROM maintable",
                                  IntResult{
                                      {1, 2333},
                                      {2, 23333},
                                      {3, 233},
                                  }));
    TxnMgrDbg("after insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
    TxnMgrDbg("after commit", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    auto txn3 = BeginTxn(*bustub, "txn3");
    WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, "SELECT * FROM maintable",
                                  IntResult{
                                      {1, 2333},
                                      {2, 23333},
                                      {3, 233},
                                  }));
    TableHeapEntryNoMoreThan(*bustub, table_info.get(), 3);
    // test continues on Gradescope...
  }
}

TEST(TxnAbortTest, DISABLED_IndexConcurrentUpdateAbortTest) {  // NOLINT
  const auto generate_sql = [](int n) -> std::string {
    return fmt::format("UPDATE maintable SET b = b + {} WHERE a = {}", 1, n);
  };
  const int thread_cnt = 8;
  const int number_cnt = 5;
  const auto generate_insert_sql = [](int n) -> std::string {
    std::vector<std::string> data;
    data.reserve(n);
    for (int i = 0; i < n; i++) {
      data.push_back(fmt::format("({}, {})", i, 0));
    }
    return fmt::format("INSERT INTO maintable VALUES {}", fmt::join(data, ","));
  };
  const int trials = 10;
  const int operation_cnt = 100;
  for (int n = 0; n < trials; n++) {
    auto bustub = std::make_unique<BusTubInstance>();
    EnsureIndexScan(*bustub);
    Execute(*bustub, "CREATE TABLE maintable(a int primary key, b int)");
    std::vector<std::thread> update_threads;
    Execute(*bustub, generate_insert_sql(number_cnt), false);
    TableHeapEntryNoMoreThan(*bustub, bustub->catalog_->GetTable("maintable").get(), number_cnt);
    update_threads.reserve(thread_cnt);
    std::map<int, std::vector<int>> operation_result;
    std::mutex result_mutex;
    fmt::println(stderr, "trial {}: running with {} threads with {} rows ", n + 1, thread_cnt, number_cnt);
    global_disable_execution_exception_print.store(true);
    for (int thread = 0; thread < thread_cnt; thread++) {
      update_threads.emplace_back([&bustub, thread, generate_sql, &result_mutex, &operation_result]() {
        NoopWriter writer;
        std::vector<int> result(number_cnt, 0);
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, number_cnt - 1);
        for (int i = 0; i < operation_cnt; i++) {
          int x = 0;
          int y = 0;
          do {
            x = dist(rng);
            y = dist(rng);
          } while (x == y);
          auto *txn = bustub->txn_manager_->Begin();
          auto sql = generate_sql(x);
          if (!bustub->ExecuteSqlTxn(sql, writer, txn)) {
            bustub->txn_manager_->Abort(txn);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
          }
          sql = generate_sql(y);
          if (!bustub->ExecuteSqlTxn(sql, writer, txn)) {
            bustub->txn_manager_->Abort(txn);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
          }
          BUSTUB_ENSURE(bustub->txn_manager_->Commit(txn), "cannot commit??");
          result[x] += 1;
          result[y] += 1;
        }
        {
          std::lock_guard<std::mutex> lck(result_mutex);
          operation_result.emplace(thread, std::move(result));
        }
      });
    }
    for (auto &&thread : update_threads) {
      thread.join();
    }
    global_disable_execution_exception_print.store(false);
    std::vector<std::vector<int>> expected_rows;
    for (int i = 0; i < number_cnt; i++) {
      int total = 0;
      for (int j = 0; j < thread_cnt; j++) {
        total += operation_result[j][i];
      }
      expected_rows.push_back({i, total});
      if (total < 10) {
        fmt::println(stderr, "abort rate too high, {} txn succeeded", total);
        std::terminate();
      }
    }
    auto table_info = bustub->catalog_->GetTable("maintable");
    auto query_txn = BeginTxn(*bustub, "query_txn");
    WithTxn(query_txn, QueryShowResult(*bustub, _var, _txn, "SELECT * FROM maintable", expected_rows));
    TableHeapEntryNoMoreThan(*bustub, table_info.get(), number_cnt);
    if (n >= trials - 2) {
      SimpleStreamWriter writer(std::cerr);
      fmt::println(stderr, "--- the following data might be manually inspected by TAs ---");
      bustub->ExecuteSqlTxn("SELECT * FROM maintable", writer, query_txn);
    }
  }
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
