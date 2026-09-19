// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// ========== Common Types ==========

// Forward declarations
struct AnyValue;
struct ArrayValue;
struct KeyValueList;

// AnyValue - 支持多种类型的值
struct AnyValue {
  std::variant<std::string, bool, double, std::shared_ptr<ArrayValue>,
               std::shared_ptr<KeyValueList>, std::vector<uint8_t>, std::monostate>
      value;

  nlohmann::json ToJson() const;
  static AnyValue FromJson(const nlohmann::json& json);
};

// ArrayValue - 值数组
struct ArrayValue {
  std::vector<AnyValue> values;

  nlohmann::json ToJson() const;
  static ArrayValue FromJson(const nlohmann::json& json);
};

// KeyValue - 键值对
struct KeyValue {
  std::string key;
  AnyValue value;

  nlohmann::json ToJson() const;
  static KeyValue FromJson(const nlohmann::json& json);
};

// KeyValueList - 键值对列表
struct KeyValueList {
  std::vector<KeyValue> values;

  nlohmann::json ToJson() const;
  static KeyValueList FromJson(const nlohmann::json& json);
};

// InstrumentationScope - 仪表作用域
struct InstrumentationScope {
  std::string name;
  std::string version;
  std::vector<KeyValue> attributes;
  uint32_t dropped_attributes_count = 0;

  nlohmann::json ToJson() const;
  static InstrumentationScope FromJson(const nlohmann::json& json);
};

// Resource - 资源信息
struct Resource {
  std::vector<KeyValue> attributes;
  uint32_t dropped_attributes_count = 0;

  nlohmann::json ToJson() const;
  static Resource FromJson(const nlohmann::json& json);
};

// ========== OpenTelemetry Logs ==========

// SeverityNumber 枚举
enum class SeverityNumber : int32_t {
  UNSPECIFIED = 0,
  TRACE = 1,
  TRACE2 = 2,
  TRACE3 = 3,
  TRACE4 = 4,
  DEBUG = 5,
  DEBUG2 = 6,
  DEBUG3 = 7,
  DEBUG4 = 8,
  INFO = 9,
  INFO2 = 10,
  INFO3 = 11,
  INFO4 = 12,
  WARN = 13,
  WARN2 = 14,
  WARN3 = 15,
  WARN4 = 16,
  ERROR = 17,
  ERROR2 = 18,
  ERROR3 = 19,
  ERROR4 = 20,
  FATAL = 21,
  FATAL2 = 22,
  FATAL3 = 23,
  FATAL4 = 24
};

// LogRecord - 日志记录
struct LogRecord {
  std::string time_unix_nano;
  std::string observed_time_unix_nano;
  SeverityNumber severity_number = SeverityNumber::UNSPECIFIED;
  std::string severity_text;
  AnyValue body;
  std::vector<KeyValue> attributes;
  uint32_t dropped_attributes_count = 0;
  uint32_t flags = 0;
  std::vector<uint8_t> trace_id;  // 16 bytes
  std::vector<uint8_t> span_id;   // 8 bytes

  nlohmann::json ToJson() const;
  static LogRecord FromJson(const nlohmann::json& json);
};

// ScopeLogs - 作用域日志
struct ScopeLogs {
  InstrumentationScope scope;
  std::vector<LogRecord> log_records;
  std::string schema_url;

  nlohmann::json ToJson() const;
  static ScopeLogs FromJson(const nlohmann::json& json);
};

// ResourceLogs - 资源日志
struct ResourceLogs {
  Resource resource;
  std::vector<ScopeLogs> scope_logs;
  std::string schema_url;

  nlohmann::json ToJson() const;
  static ResourceLogs FromJson(const nlohmann::json& json);
};

// ExportLogsServiceRequest 请求
struct ExportLogsServiceReq {
  std::vector<ResourceLogs> resource_logs;

  nlohmann::json ToJson() const;
  static ExportLogsServiceReq FromJson(const nlohmann::json& json);
};

// PartialSuccess（Logs）
struct LogsPartialSuccess {
  std::string rejected_log_records;
  std::string error_message;

  nlohmann::json ToJson() const;
  static LogsPartialSuccess FromJson(const nlohmann::json& json);
};

// ExportLogsServiceResponse 响应
struct ExportLogsServiceResp {
  LogsPartialSuccess partial_success;

  nlohmann::json ToJson() const;
  static ExportLogsServiceResp FromJson(const nlohmann::json& json);
};

// ========== OpenTelemetry Metrics ==========

// AggregationTemporality 枚举
enum class AggregationTemporality : int32_t {
  UNSPECIFIED = 0,
  DELTA = 1,
  CUMULATIVE = 2
};

// DataPointFlags 枚举
enum class DataPointFlags : uint32_t {
  DO_NOT_USE = 0,
  NO_RECORDED_VALUE_MASK = 1
};

// Forward declarations for Metrics
struct Gauge;
struct Sum;
struct Histogram;
struct ExponentialHistogram;
struct Summary;

// Exemplar - 示例值
struct Exemplar {
  std::vector<KeyValue> filtered_attributes;
  std::string time_unix_nano;
  std::variant<double, std::string, std::monostate> value;
  std::vector<uint8_t> span_id;   // 8 bytes
  std::vector<uint8_t> trace_id;  // 16 bytes

  nlohmann::json ToJson() const;
  static Exemplar FromJson(const nlohmann::json& json);
};

// NumberDataPoint - 数值数据点
struct NumberDataPoint {
  std::vector<KeyValue> attributes;
  std::string start_time_unix_nano;
  std::string time_unix_nano;
  std::variant<double, std::string, std::monostate> value;
  std::vector<Exemplar> exemplars;
  uint32_t flags = 0;

  nlohmann::json ToJson() const;
  static NumberDataPoint FromJson(const nlohmann::json& json);
};

// HistogramDataPoint - 直方图数据点
struct HistogramDataPoint {
  std::vector<KeyValue> attributes;
  std::string start_time_unix_nano;
  std::string time_unix_nano;
  std::string count;
  std::optional<double> sum;
  std::vector<std::string> bucket_counts;
  std::vector<double> explicit_bounds;
  std::vector<Exemplar> exemplars;
  uint32_t flags = 0;
  std::optional<double> min;
  std::optional<double> max;

  nlohmann::json ToJson() const;
  static HistogramDataPoint FromJson(const nlohmann::json& json);
};

// ExponentialHistogramDataPoint - 指数直方图数据点
struct ExponentialHistogramDataPoint {
  struct Buckets {
    int32_t offset = 0;
    std::vector<std::string> bucket_counts;

    nlohmann::json ToJson() const;
    static Buckets FromJson(const nlohmann::json& json);
  };

  std::vector<KeyValue> attributes;
  std::string start_time_unix_nano;
  std::string time_unix_nano;
  std::string count;
  std::optional<double> sum;
  int32_t scale = 0;
  std::string zero_count;
  Buckets positive;
  Buckets negative;
  uint32_t flags = 0;
  std::vector<Exemplar> exemplars;
  std::optional<double> min;
  std::optional<double> max;
  double zero_threshold = 0.0;

  nlohmann::json ToJson() const;
  static ExponentialHistogramDataPoint FromJson(const nlohmann::json& json);
};

// SummaryDataPoint - 摘要数据点
struct SummaryDataPoint {
  struct ValueAtQuantile {
    double quantile = 0.0;
    double value = 0.0;

    nlohmann::json ToJson() const;
    static ValueAtQuantile FromJson(const nlohmann::json& json);
  };

  std::vector<KeyValue> attributes;
  std::string start_time_unix_nano;
  std::string time_unix_nano;
  std::string count;
  double sum = 0.0;
  std::vector<ValueAtQuantile> quantile_values;
  uint32_t flags = 0;

  nlohmann::json ToJson() const;
  static SummaryDataPoint FromJson(const nlohmann::json& json);
};

// Gauge - 仪表盘指标
struct Gauge {
  std::vector<NumberDataPoint> data_points;

  nlohmann::json ToJson() const;
  static Gauge FromJson(const nlohmann::json& json);
};

// Sum - 求和指标
struct Sum {
  std::vector<NumberDataPoint> data_points;
  AggregationTemporality aggregation_temporality =
      AggregationTemporality::UNSPECIFIED;
  bool is_monotonic = false;

  nlohmann::json ToJson() const;
  static Sum FromJson(const nlohmann::json& json);
};

// Histogram - 直方图指标
struct Histogram {
  std::vector<HistogramDataPoint> data_points;
  AggregationTemporality aggregation_temporality =
      AggregationTemporality::UNSPECIFIED;

  nlohmann::json ToJson() const;
  static Histogram FromJson(const nlohmann::json& json);
};

// ExponentialHistogram - 指数直方图指标
struct ExponentialHistogram {
  std::vector<ExponentialHistogramDataPoint> data_points;
  AggregationTemporality aggregation_temporality =
      AggregationTemporality::UNSPECIFIED;

  nlohmann::json ToJson() const;
  static ExponentialHistogram FromJson(const nlohmann::json& json);
};

// Summary - 摘要指标
struct Summary {
  std::vector<SummaryDataPoint> data_points;

  nlohmann::json ToJson() const;
  static Summary FromJson(const nlohmann::json& json);
};

// Metric - 指标（支持多种类型）
struct Metric {
  std::string name;
  std::string description;
  std::string unit;
  std::vector<KeyValue> metadata;

  // 使用 optional 语义，只有一个会被设置
  std::unique_ptr<Gauge> gauge;
  std::unique_ptr<Sum> sum;
  std::unique_ptr<Histogram> histogram;
  std::unique_ptr<ExponentialHistogram> exponential_histogram;
  std::unique_ptr<Summary> summary;

  nlohmann::json ToJson() const;
  static Metric FromJson(const nlohmann::json& json);
};

// ScopeMetrics - 作用域指标
struct ScopeMetrics {
  InstrumentationScope scope;
  std::vector<Metric> metrics;
  std::string schema_url;

  nlohmann::json ToJson() const;
  static ScopeMetrics FromJson(const nlohmann::json& json);
};

// ResourceMetrics - 资源指标
struct ResourceMetrics {
  Resource resource;
  std::vector<ScopeMetrics> scope_metrics;
  std::string schema_url;

  nlohmann::json ToJson() const;
  static ResourceMetrics FromJson(const nlohmann::json& json);
};

// ExportMetricsServiceRequest 请求
struct ExportMetricsServiceReq {
  std::vector<ResourceMetrics> resource_metrics;

  nlohmann::json ToJson() const;
  static ExportMetricsServiceReq FromJson(const nlohmann::json& json);
};

// PartialSuccess（Metrics）
struct MetricsPartialSuccess {
  std::string rejected_data_points;
  std::string error_message;

  nlohmann::json ToJson() const;
  static MetricsPartialSuccess FromJson(const nlohmann::json& json);
};

// ExportMetricsServiceResponse 响应
struct ExportMetricsServiceResp {
  MetricsPartialSuccess partial_success;

  nlohmann::json ToJson() const;
  static ExportMetricsServiceResp FromJson(const nlohmann::json& json);
};

// ========== OpenTelemetry Trace ==========

// SpanKind 枚举
enum class SpanKind : int32_t {
  UNSPECIFIED = 0,
  INTERNAL = 1,
  SERVER = 2,
  CLIENT = 3,
  PRODUCER = 4,
  CONSUMER = 5
};

// Status Code 枚举
enum class StatusCode : int32_t {
  UNSET = 0,
  OK = 1,
  ERROR = 2
};

// Status - 状态
struct Status {
  std::string message;
  StatusCode code = StatusCode::UNSET;

  nlohmann::json ToJson() const;
  static Status FromJson(const nlohmann::json& json);
};

// Span::Event - Span事件
struct SpanEvent {
  std::string time_unix_nano;
  std::string name;
  std::vector<KeyValue> attributes;
  uint32_t dropped_attributes_count = 0;

  nlohmann::json ToJson() const;
  static SpanEvent FromJson(const nlohmann::json& json);
};

// Span::Link - Span链接
struct SpanLink {
  std::vector<uint8_t> trace_id;  // 16 bytes
  std::vector<uint8_t> span_id;   // 8 bytes
  std::string trace_state;
  std::vector<KeyValue> attributes;
  uint32_t dropped_attributes_count = 0;
  uint32_t flags = 0;

  nlohmann::json ToJson() const;
  static SpanLink FromJson(const nlohmann::json& json);
};

// Span - 跟踪span
struct Span {
  std::vector<uint8_t> trace_id;  // 16 bytes
  std::vector<uint8_t> span_id;   // 8 bytes
  std::string trace_state;
  std::vector<uint8_t> parent_span_id;  // 8 bytes
  uint32_t flags = 0;
  std::string name;
  SpanKind kind = SpanKind::UNSPECIFIED;
  std::string start_time_unix_nano;
  std::string end_time_unix_nano;
  std::vector<KeyValue> attributes;
  uint32_t dropped_attributes_count = 0;
  std::vector<SpanEvent> events;
  uint32_t dropped_events_count = 0;
  std::vector<SpanLink> links;
  uint32_t dropped_links_count = 0;
  Status status;

  nlohmann::json ToJson() const;
  static Span FromJson(const nlohmann::json& json);
};

// ScopeSpans - 作用域span
struct ScopeSpans {
  InstrumentationScope scope;
  std::vector<Span> spans;
  std::string schema_url;

  nlohmann::json ToJson() const;
  static ScopeSpans FromJson(const nlohmann::json& json);
};

// ResourceSpans - 资源span
struct ResourceSpans {
  Resource resource;
  std::vector<ScopeSpans> scope_spans;
  std::string schema_url;

  nlohmann::json ToJson() const;
  static ResourceSpans FromJson(const nlohmann::json& json);
};

// ExportTraceServiceRequest 请求
struct ExportTraceServiceReq {
  std::vector<ResourceSpans> resource_spans;

  nlohmann::json ToJson() const;
  static ExportTraceServiceReq FromJson(const nlohmann::json& json);
};

// PartialSuccess（Trace）
struct TracePartialSuccess {
  std::string rejected_spans;
  std::string error_message;

  nlohmann::json ToJson() const;
  static TracePartialSuccess FromJson(const nlohmann::json& json);
};

// ExportTraceServiceResponse 响应
struct ExportTraceServiceResp {
  TracePartialSuccess partial_success;

  nlohmann::json ToJson() const;
  static ExportTraceServiceResp FromJson(const nlohmann::json& json);
};

}  // namespace entity
}  // namespace palm
