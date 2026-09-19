// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#include "network/entity/opentelemetry_entity.h"

#include "cross/comm/base64.h"

namespace palm {
namespace entity {

// ========== Helper Functions ==========

static std::string EncodeBytes(const std::vector<uint8_t>& bytes) {
  if (bytes.empty()) {
    return "";
  }
  return cross::comm::base64_encode(reinterpret_cast<const char*>(bytes.data()),
                                     bytes.size());
}

static std::vector<uint8_t> DecodeBytes(const std::string& base64_str) {
  if (base64_str.empty()) {
    return {};
  }
  std::string decoded =
      cross::comm::base64_decode(base64_str.data(), base64_str.size());
  return std::vector<uint8_t>(decoded.begin(), decoded.end());
}

// ========== Common Types Implementation ==========

nlohmann::json AnyValue::ToJson() const {
  nlohmann::json json;
  if (std::holds_alternative<std::string>(value)) {
    json["stringValue"] = std::get<std::string>(value);
  } else if (std::holds_alternative<bool>(value)) {
    json["boolValue"] = std::get<bool>(value);
  } else if (std::holds_alternative<double>(value)) {
    json["doubleValue"] = std::get<double>(value);
  } else if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value)) {
    auto arr = std::get<std::shared_ptr<ArrayValue>>(value);
    if (arr) {
      json["arrayValue"] = arr->ToJson();
    }
  } else if (std::holds_alternative<std::shared_ptr<KeyValueList>>(value)) {
    auto kvlist = std::get<std::shared_ptr<KeyValueList>>(value);
    if (kvlist) {
      json["kvlistValue"] = kvlist->ToJson();
    }
  } else if (std::holds_alternative<std::vector<uint8_t>>(value)) {
    json["bytesValue"] = EncodeBytes(std::get<std::vector<uint8_t>>(value));
  }
  return json;
}

AnyValue AnyValue::FromJson(const nlohmann::json& json) {
  AnyValue any;
  if (json.contains("stringValue")) {
    any.value = json["stringValue"].get<std::string>();
  } else if (json.contains("boolValue")) {
    any.value = json["boolValue"].get<bool>();
  } else if (json.contains("doubleValue")) {
    any.value = json["doubleValue"].get<double>();
  } else if (json.contains("arrayValue")) {
    auto arr = std::make_shared<ArrayValue>(
        ArrayValue::FromJson(json["arrayValue"]));
    any.value = arr;
  } else if (json.contains("kvlistValue")) {
    auto kvlist = std::make_shared<KeyValueList>(
        KeyValueList::FromJson(json["kvlistValue"]));
    any.value = kvlist;
  } else if (json.contains("bytesValue")) {
    any.value = DecodeBytes(json["bytesValue"].get<std::string>());
  }
  return any;
}

nlohmann::json ArrayValue::ToJson() const {
  nlohmann::json json;
  nlohmann::json vals = nlohmann::json::array();
  for (const auto& v : values) {
    vals.push_back(v.ToJson());
  }
  json["values"] = vals;
  return json;
}

ArrayValue ArrayValue::FromJson(const nlohmann::json& json) {
  ArrayValue arr;
  if (json.contains("values") && json["values"].is_array()) {
    for (const auto& v : json["values"]) {
      arr.values.push_back(AnyValue::FromJson(v));
    }
  }
  return arr;
}

nlohmann::json KeyValue::ToJson() const {
  nlohmann::json json;
  json["key"] = key;
  json["value"] = value.ToJson();
  return json;
}

KeyValue KeyValue::FromJson(const nlohmann::json& json) {
  KeyValue kv;
  if (json.contains("key")) {
    kv.key = json["key"].get<std::string>();
  }
  if (json.contains("value")) {
    kv.value = AnyValue::FromJson(json["value"]);
  }
  return kv;
}

nlohmann::json KeyValueList::ToJson() const {
  nlohmann::json json;
  nlohmann::json vals = nlohmann::json::array();
  for (const auto& kv : values) {
    vals.push_back(kv.ToJson());
  }
  json["values"] = vals;
  return json;
}

KeyValueList KeyValueList::FromJson(const nlohmann::json& json) {
  KeyValueList kvlist;
  if (json.contains("values") && json["values"].is_array()) {
    for (const auto& kv : json["values"]) {
      kvlist.values.push_back(KeyValue::FromJson(kv));
    }
  }
  return kvlist;
}

nlohmann::json InstrumentationScope::ToJson() const {
  nlohmann::json json;
  json["name"] = name;
  json["version"] = version;
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (dropped_attributes_count > 0) {
    json["droppedAttributesCount"] = dropped_attributes_count;
  }
  return json;
}

InstrumentationScope InstrumentationScope::FromJson(
    const nlohmann::json& json) {
  InstrumentationScope scope;
  if (json.contains("name")) {
    scope.name = json["name"].get<std::string>();
  }
  if (json.contains("version")) {
    scope.version = json["version"].get<std::string>();
  }
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      scope.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("droppedAttributesCount")) {
    scope.dropped_attributes_count =
        json["droppedAttributesCount"].get<uint32_t>();
  }
  return scope;
}

nlohmann::json Resource::ToJson() const {
  nlohmann::json json;
  nlohmann::json attrs = nlohmann::json::array();
  for (const auto& attr : attributes) {
    attrs.push_back(attr.ToJson());
  }
  json["attributes"] = attrs;
  if (dropped_attributes_count > 0) {
    json["droppedAttributesCount"] = dropped_attributes_count;
  }
  return json;
}

Resource Resource::FromJson(const nlohmann::json& json) {
  Resource resource;
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      resource.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("droppedAttributesCount")) {
    resource.dropped_attributes_count =
        json["droppedAttributesCount"].get<uint32_t>();
  }
  return resource;
}

// ========== Logs Implementation ==========

nlohmann::json LogRecord::ToJson() const {
  nlohmann::json json;
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  if (!observed_time_unix_nano.empty()) {
    json["observedTimeUnixNano"] = observed_time_unix_nano;
  }
  json["severityNumber"] = static_cast<int32_t>(severity_number);
  if (!severity_text.empty()) {
    json["severityText"] = severity_text;
  }
  json["body"] = body.ToJson();
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (dropped_attributes_count > 0) {
    json["droppedAttributesCount"] = dropped_attributes_count;
  }
  if (flags > 0) {
    json["flags"] = flags;
  }
  if (!trace_id.empty()) {
    json["traceId"] = EncodeBytes(trace_id);
  }
  if (!span_id.empty()) {
    json["spanId"] = EncodeBytes(span_id);
  }
  return json;
}

LogRecord LogRecord::FromJson(const nlohmann::json& json) {
  LogRecord record;
  if (json.contains("timeUnixNano")) {
    record.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("observedTimeUnixNano")) {
    record.observed_time_unix_nano = json["observedTimeUnixNano"].get<std::string>();
  }
  if (json.contains("severityNumber")) {
    record.severity_number =
        static_cast<SeverityNumber>(json["severityNumber"].get<int32_t>());
  }
  if (json.contains("severityText")) {
    record.severity_text = json["severityText"].get<std::string>();
  }
  if (json.contains("body")) {
    record.body = AnyValue::FromJson(json["body"]);
  }
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      record.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("droppedAttributesCount")) {
    record.dropped_attributes_count =
        json["droppedAttributesCount"].get<uint32_t>();
  }
  if (json.contains("flags")) {
    record.flags = json["flags"].get<uint32_t>();
  }
  if (json.contains("traceId")) {
    record.trace_id = DecodeBytes(json["traceId"].get<std::string>());
  }
  if (json.contains("spanId")) {
    record.span_id = DecodeBytes(json["spanId"].get<std::string>());
  }
  return record;
}

nlohmann::json ScopeLogs::ToJson() const {
  nlohmann::json json;
  json["scope"] = scope.ToJson();
  nlohmann::json records = nlohmann::json::array();
  for (const auto& record : log_records) {
    records.push_back(record.ToJson());
  }
  json["logRecords"] = records;
  if (!schema_url.empty()) {
    json["schemaUrl"] = schema_url;
  }
  return json;
}

ScopeLogs ScopeLogs::FromJson(const nlohmann::json& json) {
  ScopeLogs scope_logs;
  if (json.contains("scope")) {
    scope_logs.scope = InstrumentationScope::FromJson(json["scope"]);
  }
  if (json.contains("logRecords") && json["logRecords"].is_array()) {
    for (const auto& record : json["logRecords"]) {
      scope_logs.log_records.push_back(LogRecord::FromJson(record));
    }
  }
  if (json.contains("schemaUrl")) {
    scope_logs.schema_url = json["schemaUrl"].get<std::string>();
  }
  return scope_logs;
}

nlohmann::json ResourceLogs::ToJson() const {
  nlohmann::json json;
  json["resource"] = resource.ToJson();
  nlohmann::json scope_logs_array = nlohmann::json::array();
  for (const auto& sl : scope_logs) {
    scope_logs_array.push_back(sl.ToJson());
  }
  json["scopeLogs"] = scope_logs_array;
  if (!schema_url.empty()) {
    json["schemaUrl"] = schema_url;
  }
  return json;
}

ResourceLogs ResourceLogs::FromJson(const nlohmann::json& json) {
  ResourceLogs resource_logs;
  if (json.contains("resource")) {
    resource_logs.resource = Resource::FromJson(json["resource"]);
  }
  if (json.contains("scopeLogs") && json["scopeLogs"].is_array()) {
    for (const auto& sl : json["scopeLogs"]) {
      resource_logs.scope_logs.push_back(ScopeLogs::FromJson(sl));
    }
  }
  if (json.contains("schemaUrl")) {
    resource_logs.schema_url = json["schemaUrl"].get<std::string>();
  }
  return resource_logs;
}

nlohmann::json ExportLogsServiceReq::ToJson() const {
  nlohmann::json json;
  nlohmann::json resource_logs_array = nlohmann::json::array();
  for (const auto& rl : resource_logs) {
    resource_logs_array.push_back(rl.ToJson());
  }
  json["resourceLogs"] = resource_logs_array;
  return json;
}

ExportLogsServiceReq ExportLogsServiceReq::FromJson(
    const nlohmann::json& json) {
  ExportLogsServiceReq req;
  if (json.contains("resourceLogs") && json["resourceLogs"].is_array()) {
    for (const auto& rl : json["resourceLogs"]) {
      req.resource_logs.push_back(ResourceLogs::FromJson(rl));
    }
  }
  return req;
}

nlohmann::json LogsPartialSuccess::ToJson() const {
  nlohmann::json json;
  if (!rejected_log_records.empty()) {
    json["rejectedLogRecords"] = rejected_log_records;
  }
  if (!error_message.empty()) {
    json["errorMessage"] = error_message;
  }
  return json;
}

LogsPartialSuccess LogsPartialSuccess::FromJson(const nlohmann::json& json) {
  LogsPartialSuccess success;
  if (json.contains("rejectedLogRecords")) {
    success.rejected_log_records = json["rejectedLogRecords"].get<std::string>();
  }
  if (json.contains("errorMessage")) {
    success.error_message = json["errorMessage"].get<std::string>();
  }
  return success;
}

nlohmann::json ExportLogsServiceResp::ToJson() const {
  nlohmann::json json;
  json["partialSuccess"] = partial_success.ToJson();
  return json;
}

ExportLogsServiceResp ExportLogsServiceResp::FromJson(
    const nlohmann::json& json) {
  ExportLogsServiceResp resp;
  if (json.contains("partialSuccess")) {
    resp.partial_success =
        LogsPartialSuccess::FromJson(json["partialSuccess"]);
  }
  return resp;
}

// ========== Metrics Implementation ==========

nlohmann::json Exemplar::ToJson() const {
  nlohmann::json json;
  if (!filtered_attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : filtered_attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["filteredAttributes"] = attrs;
  }
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  if (std::holds_alternative<double>(value)) {
    json["asDouble"] = std::get<double>(value);
  } else if (std::holds_alternative<std::string>(value)) {
    json["asInt"] = std::get<std::string>(value);
  }
  if (!span_id.empty()) {
    json["spanId"] = EncodeBytes(span_id);
  }
  if (!trace_id.empty()) {
    json["traceId"] = EncodeBytes(trace_id);
  }
  return json;
}

Exemplar Exemplar::FromJson(const nlohmann::json& json) {
  Exemplar ex;
  if (json.contains("filteredAttributes") &&
      json["filteredAttributes"].is_array()) {
    for (const auto& attr : json["filteredAttributes"]) {
      ex.filtered_attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("timeUnixNano")) {
    ex.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("asDouble")) {
    ex.value = json["asDouble"].get<double>();
  } else if (json.contains("asInt")) {
    ex.value = json["asInt"].get<std::string>();
  }
  if (json.contains("spanId")) {
    ex.span_id = DecodeBytes(json["spanId"].get<std::string>());
  }
  if (json.contains("traceId")) {
    ex.trace_id = DecodeBytes(json["traceId"].get<std::string>());
  }
  return ex;
}

nlohmann::json NumberDataPoint::ToJson() const {
  nlohmann::json json;
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (!start_time_unix_nano.empty()) {
    json["startTimeUnixNano"] = start_time_unix_nano;
  }
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  if (std::holds_alternative<double>(value)) {
    json["asDouble"] = std::get<double>(value);
  } else if (std::holds_alternative<std::string>(value)) {
    json["asInt"] = std::get<std::string>(value);
  }
  if (!exemplars.empty()) {
    nlohmann::json exs = nlohmann::json::array();
    for (const auto& ex : exemplars) {
      exs.push_back(ex.ToJson());
    }
    json["exemplars"] = exs;
  }
  if (flags > 0) {
    json["flags"] = flags;
  }
  return json;
}

NumberDataPoint NumberDataPoint::FromJson(const nlohmann::json& json) {
  NumberDataPoint dp;
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      dp.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("startTimeUnixNano")) {
    dp.start_time_unix_nano = json["startTimeUnixNano"].get<std::string>();
  }
  if (json.contains("timeUnixNano")) {
    dp.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("asDouble")) {
    dp.value = json["asDouble"].get<double>();
  } else if (json.contains("asInt")) {
    dp.value = json["asInt"].get<std::string>();
  }
  if (json.contains("exemplars") && json["exemplars"].is_array()) {
    for (const auto& ex : json["exemplars"]) {
      dp.exemplars.push_back(Exemplar::FromJson(ex));
    }
  }
  if (json.contains("flags")) {
    dp.flags = json["flags"].get<uint32_t>();
  }
  return dp;
}

nlohmann::json HistogramDataPoint::ToJson() const {
  nlohmann::json json;
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (!start_time_unix_nano.empty()) {
    json["startTimeUnixNano"] = start_time_unix_nano;
  }
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  if (!count.empty()) {
    json["count"] = count;
  }
  if (sum.has_value()) {
    json["sum"] = sum.value();
  }
  if (!bucket_counts.empty()) {
    nlohmann::json counts = nlohmann::json::array();
    for (const auto& c : bucket_counts) {
      counts.push_back(c);
    }
    json["bucketCounts"] = counts;
  }
  if (!explicit_bounds.empty()) {
    json["explicitBounds"] = explicit_bounds;
  }
  if (!exemplars.empty()) {
    nlohmann::json exs = nlohmann::json::array();
    for (const auto& ex : exemplars) {
      exs.push_back(ex.ToJson());
    }
    json["exemplars"] = exs;
  }
  if (flags > 0) {
    json["flags"] = flags;
  }
  if (min.has_value()) {
    json["min"] = min.value();
  }
  if (max.has_value()) {
    json["max"] = max.value();
  }
  return json;
}

HistogramDataPoint HistogramDataPoint::FromJson(const nlohmann::json& json) {
  HistogramDataPoint dp;
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      dp.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("startTimeUnixNano")) {
    dp.start_time_unix_nano = json["startTimeUnixNano"].get<std::string>();
  }
  if (json.contains("timeUnixNano")) {
    dp.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("count")) {
    dp.count = json["count"].get<std::string>();
  }
  if (json.contains("sum")) {
    dp.sum = json["sum"].get<double>();
  }
  if (json.contains("bucketCounts") && json["bucketCounts"].is_array()) {
    for (const auto& c : json["bucketCounts"]) {
      dp.bucket_counts.push_back(c.get<std::string>());
    }
  }
  if (json.contains("explicitBounds") && json["explicitBounds"].is_array()) {
    for (const auto& b : json["explicitBounds"]) {
      dp.explicit_bounds.push_back(b.get<double>());
    }
  }
  if (json.contains("exemplars") && json["exemplars"].is_array()) {
    for (const auto& ex : json["exemplars"]) {
      dp.exemplars.push_back(Exemplar::FromJson(ex));
    }
  }
  if (json.contains("flags")) {
    dp.flags = json["flags"].get<uint32_t>();
  }
  if (json.contains("min")) {
    dp.min = json["min"].get<double>();
  }
  if (json.contains("max")) {
    dp.max = json["max"].get<double>();
  }
  return dp;
}

nlohmann::json ExponentialHistogramDataPoint::Buckets::ToJson() const {
  nlohmann::json json;
  json["offset"] = offset;
  if (!bucket_counts.empty()) {
    nlohmann::json counts = nlohmann::json::array();
    for (const auto& c : bucket_counts) {
      counts.push_back(c);
    }
    json["bucketCounts"] = counts;
  }
  return json;
}

ExponentialHistogramDataPoint::Buckets
ExponentialHistogramDataPoint::Buckets::FromJson(const nlohmann::json& json) {
  Buckets buckets;
  if (json.contains("offset")) {
    buckets.offset = json["offset"].get<int32_t>();
  }
  if (json.contains("bucketCounts") && json["bucketCounts"].is_array()) {
    for (const auto& c : json["bucketCounts"]) {
      buckets.bucket_counts.push_back(c.get<std::string>());
    }
  }
  return buckets;
}

nlohmann::json ExponentialHistogramDataPoint::ToJson() const {
  nlohmann::json json;
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (!start_time_unix_nano.empty()) {
    json["startTimeUnixNano"] = start_time_unix_nano;
  }
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  if (!count.empty()) {
    json["count"] = count;
  }
  if (sum.has_value()) {
    json["sum"] = sum.value();
  }
  json["scale"] = scale;
  if (!zero_count.empty()) {
    json["zeroCount"] = zero_count;
  }
  json["positive"] = positive.ToJson();
  json["negative"] = negative.ToJson();
  if (flags > 0) {
    json["flags"] = flags;
  }
  if (!exemplars.empty()) {
    nlohmann::json exs = nlohmann::json::array();
    for (const auto& ex : exemplars) {
      exs.push_back(ex.ToJson());
    }
    json["exemplars"] = exs;
  }
  if (min.has_value()) {
    json["min"] = min.value();
  }
  if (max.has_value()) {
    json["max"] = max.value();
  }
  if (zero_threshold != 0.0) {
    json["zeroThreshold"] = zero_threshold;
  }
  return json;
}

ExponentialHistogramDataPoint ExponentialHistogramDataPoint::FromJson(
    const nlohmann::json& json) {
  ExponentialHistogramDataPoint dp;
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      dp.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("startTimeUnixNano")) {
    dp.start_time_unix_nano = json["startTimeUnixNano"].get<std::string>();
  }
  if (json.contains("timeUnixNano")) {
    dp.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("count")) {
    dp.count = json["count"].get<std::string>();
  }
  if (json.contains("sum")) {
    dp.sum = json["sum"].get<double>();
  }
  if (json.contains("scale")) {
    dp.scale = json["scale"].get<int32_t>();
  }
  if (json.contains("zeroCount")) {
    dp.zero_count = json["zeroCount"].get<std::string>();
  }
  if (json.contains("positive")) {
    dp.positive = Buckets::FromJson(json["positive"]);
  }
  if (json.contains("negative")) {
    dp.negative = Buckets::FromJson(json["negative"]);
  }
  if (json.contains("flags")) {
    dp.flags = json["flags"].get<uint32_t>();
  }
  if (json.contains("exemplars") && json["exemplars"].is_array()) {
    for (const auto& ex : json["exemplars"]) {
      dp.exemplars.push_back(Exemplar::FromJson(ex));
    }
  }
  if (json.contains("min")) {
    dp.min = json["min"].get<double>();
  }
  if (json.contains("max")) {
    dp.max = json["max"].get<double>();
  }
  if (json.contains("zeroThreshold")) {
    dp.zero_threshold = json["zeroThreshold"].get<double>();
  }
  return dp;
}

nlohmann::json SummaryDataPoint::ValueAtQuantile::ToJson() const {
  nlohmann::json json;
  json["quantile"] = quantile;
  json["value"] = value;
  return json;
}

SummaryDataPoint::ValueAtQuantile SummaryDataPoint::ValueAtQuantile::FromJson(
    const nlohmann::json& json) {
  ValueAtQuantile vaq;
  if (json.contains("quantile")) {
    vaq.quantile = json["quantile"].get<double>();
  }
  if (json.contains("value")) {
    vaq.value = json["value"].get<double>();
  }
  return vaq;
}

nlohmann::json SummaryDataPoint::ToJson() const {
  nlohmann::json json;
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (!start_time_unix_nano.empty()) {
    json["startTimeUnixNano"] = start_time_unix_nano;
  }
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  if (!count.empty()) {
    json["count"] = count;
  }
  json["sum"] = sum;
  if (!quantile_values.empty()) {
    nlohmann::json quantiles = nlohmann::json::array();
    for (const auto& q : quantile_values) {
      quantiles.push_back(q.ToJson());
    }
    json["quantileValues"] = quantiles;
  }
  if (flags > 0) {
    json["flags"] = flags;
  }
  return json;
}

SummaryDataPoint SummaryDataPoint::FromJson(const nlohmann::json& json) {
  SummaryDataPoint dp;
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      dp.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("startTimeUnixNano")) {
    dp.start_time_unix_nano = json["startTimeUnixNano"].get<std::string>();
  }
  if (json.contains("timeUnixNano")) {
    dp.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("count")) {
    dp.count = json["count"].get<std::string>();
  }
  if (json.contains("sum")) {
    dp.sum = json["sum"].get<double>();
  }
  if (json.contains("quantileValues") && json["quantileValues"].is_array()) {
    for (const auto& q : json["quantileValues"]) {
      dp.quantile_values.push_back(ValueAtQuantile::FromJson(q));
    }
  }
  if (json.contains("flags")) {
    dp.flags = json["flags"].get<uint32_t>();
  }
  return dp;
}

nlohmann::json Gauge::ToJson() const {
  nlohmann::json json;
  nlohmann::json data_points_array = nlohmann::json::array();
  for (const auto& dp : data_points) {
    data_points_array.push_back(dp.ToJson());
  }
  json["dataPoints"] = data_points_array;
  return json;
}

Gauge Gauge::FromJson(const nlohmann::json& json) {
  Gauge gauge;
  if (json.contains("dataPoints") && json["dataPoints"].is_array()) {
    for (const auto& dp : json["dataPoints"]) {
      gauge.data_points.push_back(NumberDataPoint::FromJson(dp));
    }
  }
  return gauge;
}

nlohmann::json Sum::ToJson() const {
  nlohmann::json json;
  nlohmann::json data_points_array = nlohmann::json::array();
  for (const auto& dp : data_points) {
    data_points_array.push_back(dp.ToJson());
  }
  json["dataPoints"] = data_points_array;
  json["aggregationTemporality"] =
      static_cast<int32_t>(aggregation_temporality);
  json["isMonotonic"] = is_monotonic;
  return json;
}

Sum Sum::FromJson(const nlohmann::json& json) {
  Sum sum;
  if (json.contains("dataPoints") && json["dataPoints"].is_array()) {
    for (const auto& dp : json["dataPoints"]) {
      sum.data_points.push_back(NumberDataPoint::FromJson(dp));
    }
  }
  if (json.contains("aggregationTemporality")) {
    sum.aggregation_temporality = static_cast<AggregationTemporality>(
        json["aggregationTemporality"].get<int32_t>());
  }
  if (json.contains("isMonotonic")) {
    sum.is_monotonic = json["isMonotonic"].get<bool>();
  }
  return sum;
}

nlohmann::json Histogram::ToJson() const {
  nlohmann::json json;
  nlohmann::json data_points_array = nlohmann::json::array();
  for (const auto& dp : data_points) {
    data_points_array.push_back(dp.ToJson());
  }
  json["dataPoints"] = data_points_array;
  json["aggregationTemporality"] =
      static_cast<int32_t>(aggregation_temporality);
  return json;
}

Histogram Histogram::FromJson(const nlohmann::json& json) {
  Histogram histogram;
  if (json.contains("dataPoints") && json["dataPoints"].is_array()) {
    for (const auto& dp : json["dataPoints"]) {
      histogram.data_points.push_back(HistogramDataPoint::FromJson(dp));
    }
  }
  if (json.contains("aggregationTemporality")) {
    histogram.aggregation_temporality = static_cast<AggregationTemporality>(
        json["aggregationTemporality"].get<int32_t>());
  }
  return histogram;
}

nlohmann::json ExponentialHistogram::ToJson() const {
  nlohmann::json json;
  nlohmann::json data_points_array = nlohmann::json::array();
  for (const auto& dp : data_points) {
    data_points_array.push_back(dp.ToJson());
  }
  json["dataPoints"] = data_points_array;
  json["aggregationTemporality"] =
      static_cast<int32_t>(aggregation_temporality);
  return json;
}

ExponentialHistogram ExponentialHistogram::FromJson(
    const nlohmann::json& json) {
  ExponentialHistogram exp_hist;
  if (json.contains("dataPoints") && json["dataPoints"].is_array()) {
    for (const auto& dp : json["dataPoints"]) {
      exp_hist.data_points.push_back(
          ExponentialHistogramDataPoint::FromJson(dp));
    }
  }
  if (json.contains("aggregationTemporality")) {
    exp_hist.aggregation_temporality = static_cast<AggregationTemporality>(
        json["aggregationTemporality"].get<int32_t>());
  }
  return exp_hist;
}

nlohmann::json Summary::ToJson() const {
  nlohmann::json json;
  nlohmann::json data_points_array = nlohmann::json::array();
  for (const auto& dp : data_points) {
    data_points_array.push_back(dp.ToJson());
  }
  json["dataPoints"] = data_points_array;
  return json;
}

Summary Summary::FromJson(const nlohmann::json& json) {
  Summary summary;
  if (json.contains("dataPoints") && json["dataPoints"].is_array()) {
    for (const auto& dp : json["dataPoints"]) {
      summary.data_points.push_back(SummaryDataPoint::FromJson(dp));
    }
  }
  return summary;
}

nlohmann::json Metric::ToJson() const {
  nlohmann::json json;
  json["name"] = name;
  if (!description.empty()) {
    json["description"] = description;
  }
  if (!unit.empty()) {
    json["unit"] = unit;
  }
  if (!metadata.empty()) {
    nlohmann::json meta_array = nlohmann::json::array();
    for (const auto& m : metadata) {
      meta_array.push_back(m.ToJson());
    }
    json["metadata"] = meta_array;
  }
  // data (only one will be set)
  if (gauge) {
    json["gauge"] = gauge->ToJson();
  } else if (sum) {
    json["sum"] = sum->ToJson();
  } else if (histogram) {
    json["histogram"] = histogram->ToJson();
  } else if (exponential_histogram) {
    json["exponentialHistogram"] = exponential_histogram->ToJson();
  } else if (summary) {
    json["summary"] = summary->ToJson();
  }
  return json;
}

Metric Metric::FromJson(const nlohmann::json& json) {
  Metric metric;
  if (json.contains("name")) {
    metric.name = json["name"].get<std::string>();
  }
  if (json.contains("description")) {
    metric.description = json["description"].get<std::string>();
  }
  if (json.contains("unit")) {
    metric.unit = json["unit"].get<std::string>();
  }
  if (json.contains("metadata") && json["metadata"].is_array()) {
    for (const auto& m : json["metadata"]) {
      metric.metadata.push_back(KeyValue::FromJson(m));
    }
  }
  if (json.contains("gauge")) {
    metric.gauge = std::make_unique<Gauge>(Gauge::FromJson(json["gauge"]));
  } else if (json.contains("sum")) {
    metric.sum = std::make_unique<Sum>(Sum::FromJson(json["sum"]));
  } else if (json.contains("histogram")) {
    metric.histogram =
        std::make_unique<Histogram>(Histogram::FromJson(json["histogram"]));
  } else if (json.contains("exponentialHistogram")) {
    metric.exponential_histogram = std::make_unique<ExponentialHistogram>(
        ExponentialHistogram::FromJson(json["exponentialHistogram"]));
  } else if (json.contains("summary")) {
    metric.summary =
        std::make_unique<Summary>(Summary::FromJson(json["summary"]));
  }
  return metric;
}

nlohmann::json ScopeMetrics::ToJson() const {
  nlohmann::json json;
  json["scope"] = scope.ToJson();
  nlohmann::json metrics_array = nlohmann::json::array();
  for (const auto& m : metrics) {
    metrics_array.push_back(m.ToJson());
  }
  json["metrics"] = metrics_array;
  if (!schema_url.empty()) {
    json["schemaUrl"] = schema_url;
  }
  return json;
}

ScopeMetrics ScopeMetrics::FromJson(const nlohmann::json& json) {
  ScopeMetrics scope_metrics;
  if (json.contains("scope")) {
    scope_metrics.scope = InstrumentationScope::FromJson(json["scope"]);
  }
  if (json.contains("metrics") && json["metrics"].is_array()) {
    for (const auto& m : json["metrics"]) {
      scope_metrics.metrics.push_back(Metric::FromJson(m));
    }
  }
  if (json.contains("schemaUrl")) {
    scope_metrics.schema_url = json["schemaUrl"].get<std::string>();
  }
  return scope_metrics;
}

nlohmann::json ResourceMetrics::ToJson() const {
  nlohmann::json json;
  json["resource"] = resource.ToJson();
  nlohmann::json scope_metrics_array = nlohmann::json::array();
  for (const auto& sm : scope_metrics) {
    scope_metrics_array.push_back(sm.ToJson());
  }
  json["scopeMetrics"] = scope_metrics_array;
  if (!schema_url.empty()) {
    json["schemaUrl"] = schema_url;
  }
  return json;
}

ResourceMetrics ResourceMetrics::FromJson(const nlohmann::json& json) {
  ResourceMetrics resource_metrics;
  if (json.contains("resource")) {
    resource_metrics.resource = Resource::FromJson(json["resource"]);
  }
  if (json.contains("scopeMetrics") && json["scopeMetrics"].is_array()) {
    for (const auto& sm : json["scopeMetrics"]) {
      resource_metrics.scope_metrics.push_back(ScopeMetrics::FromJson(sm));
    }
  }
  if (json.contains("schemaUrl")) {
    resource_metrics.schema_url = json["schemaUrl"].get<std::string>();
  }
  return resource_metrics;
}

nlohmann::json ExportMetricsServiceReq::ToJson() const {
  nlohmann::json json;
  nlohmann::json resource_metrics_array = nlohmann::json::array();
  for (const auto& rm : resource_metrics) {
    resource_metrics_array.push_back(rm.ToJson());
  }
  json["resourceMetrics"] = resource_metrics_array;
  return json;
}

ExportMetricsServiceReq ExportMetricsServiceReq::FromJson(
    const nlohmann::json& json) {
  ExportMetricsServiceReq req;
  if (json.contains("resourceMetrics") && json["resourceMetrics"].is_array()) {
    for (const auto& rm : json["resourceMetrics"]) {
      req.resource_metrics.push_back(ResourceMetrics::FromJson(rm));
    }
  }
  return req;
}

nlohmann::json MetricsPartialSuccess::ToJson() const {
  nlohmann::json json;
  if (!rejected_data_points.empty()) {
    json["rejectedDataPoints"] = rejected_data_points;
  }
  if (!error_message.empty()) {
    json["errorMessage"] = error_message;
  }
  return json;
}

MetricsPartialSuccess MetricsPartialSuccess::FromJson(
    const nlohmann::json& json) {
  MetricsPartialSuccess success;
  if (json.contains("rejectedDataPoints")) {
    success.rejected_data_points = json["rejectedDataPoints"].get<std::string>();
  }
  if (json.contains("errorMessage")) {
    success.error_message = json["errorMessage"].get<std::string>();
  }
  return success;
}

nlohmann::json ExportMetricsServiceResp::ToJson() const {
  nlohmann::json json;
  json["partialSuccess"] = partial_success.ToJson();
  return json;
}

ExportMetricsServiceResp ExportMetricsServiceResp::FromJson(
    const nlohmann::json& json) {
  ExportMetricsServiceResp resp;
  if (json.contains("partialSuccess")) {
    resp.partial_success =
        MetricsPartialSuccess::FromJson(json["partialSuccess"]);
  }
  return resp;
}

// ========== Trace Implementation ==========

nlohmann::json Status::ToJson() const {
  nlohmann::json json;
  if (!message.empty()) {
    json["message"] = message;
  }
  json["code"] = static_cast<int32_t>(code);
  return json;
}

Status Status::FromJson(const nlohmann::json& json) {
  Status status;
  if (json.contains("message")) {
    status.message = json["message"].get<std::string>();
  }
  if (json.contains("code")) {
    status.code = static_cast<StatusCode>(json["code"].get<int32_t>());
  }
  return status;
}

nlohmann::json SpanEvent::ToJson() const {
  nlohmann::json json;
  if (!time_unix_nano.empty()) {
    json["timeUnixNano"] = time_unix_nano;
  }
  json["name"] = name;
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (dropped_attributes_count > 0) {
    json["droppedAttributesCount"] = dropped_attributes_count;
  }
  return json;
}

SpanEvent SpanEvent::FromJson(const nlohmann::json& json) {
  SpanEvent event;
  if (json.contains("timeUnixNano")) {
    event.time_unix_nano = json["timeUnixNano"].get<std::string>();
  }
  if (json.contains("name")) {
    event.name = json["name"].get<std::string>();
  }
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      event.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("droppedAttributesCount")) {
    event.dropped_attributes_count =
        json["droppedAttributesCount"].get<uint32_t>();
  }
  return event;
}

nlohmann::json SpanLink::ToJson() const {
  nlohmann::json json;
  json["traceId"] = EncodeBytes(trace_id);
  json["spanId"] = EncodeBytes(span_id);
  if (!trace_state.empty()) {
    json["traceState"] = trace_state;
  }
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (dropped_attributes_count > 0) {
    json["droppedAttributesCount"] = dropped_attributes_count;
  }
  if (flags > 0) {
    json["flags"] = flags;
  }
  return json;
}

SpanLink SpanLink::FromJson(const nlohmann::json& json) {
  SpanLink link;
  if (json.contains("traceId")) {
    link.trace_id = DecodeBytes(json["traceId"].get<std::string>());
  }
  if (json.contains("spanId")) {
    link.span_id = DecodeBytes(json["spanId"].get<std::string>());
  }
  if (json.contains("traceState")) {
    link.trace_state = json["traceState"].get<std::string>();
  }
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      link.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("droppedAttributesCount")) {
    link.dropped_attributes_count =
        json["droppedAttributesCount"].get<uint32_t>();
  }
  if (json.contains("flags")) {
    link.flags = json["flags"].get<uint32_t>();
  }
  return link;
}

nlohmann::json Span::ToJson() const {
  nlohmann::json json;
  json["traceId"] = EncodeBytes(trace_id);
  json["spanId"] = EncodeBytes(span_id);
  if (!trace_state.empty()) {
    json["traceState"] = trace_state;
  }
  if (!parent_span_id.empty()) {
    json["parentSpanId"] = EncodeBytes(parent_span_id);
  }
  if (flags > 0) {
    json["flags"] = flags;
  }
  json["name"] = name;
  json["kind"] = static_cast<int32_t>(kind);
  if (!start_time_unix_nano.empty()) {
    json["startTimeUnixNano"] = start_time_unix_nano;
  }
  if (!end_time_unix_nano.empty()) {
    json["endTimeUnixNano"] = end_time_unix_nano;
  }
  if (!attributes.empty()) {
    nlohmann::json attrs = nlohmann::json::array();
    for (const auto& attr : attributes) {
      attrs.push_back(attr.ToJson());
    }
    json["attributes"] = attrs;
  }
  if (dropped_attributes_count > 0) {
    json["droppedAttributesCount"] = dropped_attributes_count;
  }
  if (!events.empty()) {
    nlohmann::json evts = nlohmann::json::array();
    for (const auto& evt : events) {
      evts.push_back(evt.ToJson());
    }
    json["events"] = evts;
  }
  if (dropped_events_count > 0) {
    json["droppedEventsCount"] = dropped_events_count;
  }
  if (!links.empty()) {
    nlohmann::json lnks = nlohmann::json::array();
    for (const auto& lnk : links) {
      lnks.push_back(lnk.ToJson());
    }
    json["links"] = lnks;
  }
  if (dropped_links_count > 0) {
    json["droppedLinksCount"] = dropped_links_count;
  }
  json["status"] = status.ToJson();
  return json;
}

Span Span::FromJson(const nlohmann::json& json) {
  Span span;
  if (json.contains("traceId")) {
    span.trace_id = DecodeBytes(json["traceId"].get<std::string>());
  }
  if (json.contains("spanId")) {
    span.span_id = DecodeBytes(json["spanId"].get<std::string>());
  }
  if (json.contains("traceState")) {
    span.trace_state = json["traceState"].get<std::string>();
  }
  if (json.contains("parentSpanId")) {
    span.parent_span_id = DecodeBytes(json["parentSpanId"].get<std::string>());
  }
  if (json.contains("flags")) {
    span.flags = json["flags"].get<uint32_t>();
  }
  if (json.contains("name")) {
    span.name = json["name"].get<std::string>();
  }
  if (json.contains("kind")) {
    span.kind = static_cast<SpanKind>(json["kind"].get<int32_t>());
  }
  if (json.contains("startTimeUnixNano")) {
    span.start_time_unix_nano = json["startTimeUnixNano"].get<std::string>();
  }
  if (json.contains("endTimeUnixNano")) {
    span.end_time_unix_nano = json["endTimeUnixNano"].get<std::string>();
  }
  if (json.contains("attributes") && json["attributes"].is_array()) {
    for (const auto& attr : json["attributes"]) {
      span.attributes.push_back(KeyValue::FromJson(attr));
    }
  }
  if (json.contains("droppedAttributesCount")) {
    span.dropped_attributes_count =
        json["droppedAttributesCount"].get<uint32_t>();
  }
  if (json.contains("events") && json["events"].is_array()) {
    for (const auto& evt : json["events"]) {
      span.events.push_back(SpanEvent::FromJson(evt));
    }
  }
  if (json.contains("droppedEventsCount")) {
    span.dropped_events_count = json["droppedEventsCount"].get<uint32_t>();
  }
  if (json.contains("links") && json["links"].is_array()) {
    for (const auto& lnk : json["links"]) {
      span.links.push_back(SpanLink::FromJson(lnk));
    }
  }
  if (json.contains("droppedLinksCount")) {
    span.dropped_links_count = json["droppedLinksCount"].get<uint32_t>();
  }
  if (json.contains("status")) {
    span.status = Status::FromJson(json["status"]);
  }
  return span;
}

nlohmann::json ScopeSpans::ToJson() const {
  nlohmann::json json;
  json["scope"] = scope.ToJson();
  nlohmann::json spans_array = nlohmann::json::array();
  for (const auto& span : spans) {
    spans_array.push_back(span.ToJson());
  }
  json["spans"] = spans_array;
  if (!schema_url.empty()) {
    json["schemaUrl"] = schema_url;
  }
  return json;
}

ScopeSpans ScopeSpans::FromJson(const nlohmann::json& json) {
  ScopeSpans scope_spans;
  if (json.contains("scope")) {
    scope_spans.scope = InstrumentationScope::FromJson(json["scope"]);
  }
  if (json.contains("spans") && json["spans"].is_array()) {
    for (const auto& span : json["spans"]) {
      scope_spans.spans.push_back(Span::FromJson(span));
    }
  }
  if (json.contains("schemaUrl")) {
    scope_spans.schema_url = json["schemaUrl"].get<std::string>();
  }
  return scope_spans;
}

nlohmann::json ResourceSpans::ToJson() const {
  nlohmann::json json;
  json["resource"] = resource.ToJson();
  nlohmann::json scope_spans_array = nlohmann::json::array();
  for (const auto& ss : scope_spans) {
    scope_spans_array.push_back(ss.ToJson());
  }
  json["scopeSpans"] = scope_spans_array;
  if (!schema_url.empty()) {
    json["schemaUrl"] = schema_url;
  }
  return json;
}

ResourceSpans ResourceSpans::FromJson(const nlohmann::json& json) {
  ResourceSpans resource_spans;
  if (json.contains("resource")) {
    resource_spans.resource = Resource::FromJson(json["resource"]);
  }
  if (json.contains("scopeSpans") && json["scopeSpans"].is_array()) {
    for (const auto& ss : json["scopeSpans"]) {
      resource_spans.scope_spans.push_back(ScopeSpans::FromJson(ss));
    }
  }
  if (json.contains("schemaUrl")) {
    resource_spans.schema_url = json["schemaUrl"].get<std::string>();
  }
  return resource_spans;
}

nlohmann::json ExportTraceServiceReq::ToJson() const {
  nlohmann::json json;
  nlohmann::json resource_spans_array = nlohmann::json::array();
  for (const auto& rs : resource_spans) {
    resource_spans_array.push_back(rs.ToJson());
  }
  json["resourceSpans"] = resource_spans_array;
  return json;
}

ExportTraceServiceReq ExportTraceServiceReq::FromJson(
    const nlohmann::json& json) {
  ExportTraceServiceReq req;
  if (json.contains("resourceSpans") && json["resourceSpans"].is_array()) {
    for (const auto& rs : json["resourceSpans"]) {
      req.resource_spans.push_back(ResourceSpans::FromJson(rs));
    }
  }
  return req;
}

nlohmann::json TracePartialSuccess::ToJson() const {
  nlohmann::json json;
  if (!rejected_spans.empty()) {
    json["rejectedSpans"] = rejected_spans;
  }
  if (!error_message.empty()) {
    json["errorMessage"] = error_message;
  }
  return json;
}

TracePartialSuccess TracePartialSuccess::FromJson(const nlohmann::json& json) {
  TracePartialSuccess success;
  if (json.contains("rejectedSpans")) {
    success.rejected_spans = json["rejectedSpans"].get<std::string>();
  }
  if (json.contains("errorMessage")) {
    success.error_message = json["errorMessage"].get<std::string>();
  }
  return success;
}

nlohmann::json ExportTraceServiceResp::ToJson() const {
  nlohmann::json json;
  json["partialSuccess"] = partial_success.ToJson();
  return json;
}

ExportTraceServiceResp ExportTraceServiceResp::FromJson(
    const nlohmann::json& json) {
  ExportTraceServiceResp resp;
  if (json.contains("partialSuccess")) {
    resp.partial_success =
        TracePartialSuccess::FromJson(json["partialSuccess"]);
  }
  return resp;
}

}  // namespace entity
}  // namespace palm
