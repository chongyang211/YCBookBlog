# ES DSL Cookbook（zhiyan-log 查询模板）

所有 DSL 都是给 `zhiyan-log/SearchLog` 的 `json_content` 参数用。字段名参考 `references/fields.md`。

## 通用规则

- 走 `bool.filter`（不评分、更快），不走 `bool.must`
- `term`：精确匹配 keyword/long/date 字段
- `match_phrase`：短语匹配 text 字段（`msg`、`@message`）
- `wildcard`：慎用（性能差），只在没别的选择时用
- `range`：数值范围（`cost`、`status_code`、`response_size`）
- `size: 0 + aggs`：只要统计不要原文，永远优先
- `sort`：`@timestamp` desc（默认新到旧）；trace 链路场景用 asc

## 模式 A：Root Cause 定位模板

### A.1 量级探测（**永远先跑这个**）

```json
{
  "size": 0,
  "query": {
    "bool": {
      "filter": [
        {"term": {"level": "error"}}
      ]
    }
  }
}
```

响应里看 `hits.total.value`：
- 0 → 该窗口无 error，扩大条件（改 warn / 去掉 level filter）
- 1~50 → 直接拉原文（`size: 50`）
- >50 → 先按 `msg` 聚合再看 Top

### A.2 按 msg 聚合 Top N 错误

```json
{
  "size": 0,
  "query": {
    "bool": {
      "filter": [
        {"term": {"level": "error"}}
      ]
    }
  },
  "aggs": {
    "top_errors": {
      "terms": {"field": "msg", "size": 10}
    }
  }
}
```

> 注意：`msg` 是 keyword，能直接 aggs；`@message` 是 text，做不了 terms 聚合。

### A.3 拉某类 error 的完整日志

```json
{
  "size": 20,
  "query": {
    "bool": {
      "filter": [
        {"term": {"level": "error"}},
        {"match_phrase": {"msg": "signature verify failed"}}
      ]
    }
  },
  "sort": [{"@timestamp": {"order": "desc"}}]
}
```

### A.4 按 file:line 定位问题代码

```json
{
  "size": 0,
  "query": {
    "bool": {
      "filter": [{"term": {"level": "error"}}]
    }
  },
  "aggs": {
    "by_file": {
      "terms": {"field": "file", "size": 10},
      "aggs": {
        "by_line": {
          "terms": {"field": "line", "size": 5}
        }
      }
    }
  }
}
```

### A.5 关键词全文搜索（用户没说 level）

```json
{
  "size": 30,
  "query": {
    "bool": {
      "filter": [
        {"match_phrase": {"@message": "panic"}}
      ]
    }
  },
  "sort": [{"@timestamp": {"order": "desc"}}]
}
```

## 模式 B：Trace 链路模板

### B.1 按 trace_id 拉全链路

```json
{
  "size": 200,
  "query": {
    "bool": {
      "filter": [
        {"term": {"trace_id": "d2f3e730d86ec276963978ada66b01e8"}}
      ]
    }
  },
  "sort": [{"@timestamp": {"order": "asc"}}]
}
```

> `size: 200` 一般够；如果单个 trace 超过 200 条，说明业务逻辑有循环或者巨慢，直接告诉用户"数据量过大，只展示前 200 条"。

### B.2 trace 链路中只挑错误节点

```json
{
  "size": 50,
  "query": {
    "bool": {
      "filter": [
        {"term": {"trace_id": "xxx"}},
        {"terms": {"level": ["error", "warn"]}}
      ]
    }
  },
  "sort": [{"@timestamp": {"order": "asc"}}]
}
```

### B.3 按 span_id 拉某个具体 span

```json
{
  "size": 100,
  "query": {
    "bool": {
      "filter": [
        {"term": {"span_id": "121b6102a4a58778"}}
      ]
    }
  },
  "sort": [{"@timestamp": {"order": "asc"}}]
}
```

## 模式 C：健康巡检模板

### C.1 level 分布（错误率）

```json
{
  "size": 0,
  "aggs": {
    "by_level": {
      "terms": {"field": "level", "size": 10}
    }
  }
}
```

计算错误率：`by_level.buckets` 里 `error/warn` count / total。

### C.2 慢请求 TOP

```json
{
  "size": 10,
  "query": {
    "bool": {
      "filter": [
        {"range": {"cost": {"gte": 3000}}}
      ]
    }
  },
  "sort": [{"cost": {"order": "desc"}}]
}
```

阈值参考：
- 网关服务：>3s 算慢
- DB 密集型：>1s 算慢
- 识别/特征提取：>5s 算慢

### C.3 HTTP 状态码分布

```json
{
  "size": 0,
  "query": {
    "bool": {
      "filter": [
        {"range": {"status_code": {"gte": 400}}}
      ]
    }
  },
  "aggs": {
    "by_code": {
      "terms": {"field": "status_code", "size": 20}
    }
  }
}
```

### C.4 接口调用 TOP（按 url）

```json
{
  "size": 0,
  "aggs": {
    "by_url": {
      "terms": {"field": "url", "size": 20},
      "aggs": {
        "avg_cost": {"avg": {"field": "cost"}},
        "err_rate": {
          "filter": {"range": {"status_code": {"gte": 500}}}
        }
      }
    }
  }
}
```

### C.5 按 pod 拉错误分布（定位坏节点）

```json
{
  "size": 0,
  "query": {
    "bool": {
      "filter": [{"term": {"level": "error"}}]
    }
  },
  "aggs": {
    "by_pod": {
      "terms": {"field": "@podname", "size": 20}
    }
  }
}
```

### C.6 按时间桶分布错误（找异常时段）

```json
{
  "size": 0,
  "query": {
    "bool": {
      "filter": [{"term": {"level": "error"}}]
    }
  },
  "aggs": {
    "by_time": {
      "date_histogram": {
        "field": "@timestamp",
        "fixed_interval": "1m"
      }
    }
  }
}
```

## 组合技

### 某个 trace + 某个 pod 出错

```json
{
  "size": 100,
  "query": {
    "bool": {
      "filter": [
        {"term": {"trace_id": "xxx"}},
        {"term": {"@podname": "wecardpayapp-abc-xyz"}},
        {"term": {"level": "error"}}
      ]
    }
  },
  "sort": [{"@timestamp": {"order": "asc"}}]
}
```

### DB 慢查询排查（SQL 耗时 + rows）

```json
{
  "size": 20,
  "query": {
    "bool": {
      "filter": [
        {"exists": {"field": "sql"}},
        {"range": {"cost": {"gte": 1000}}}
      ]
    }
  },
  "sort": [{"cost": {"order": "desc"}}]
}
```

## 反模式（不要写的 DSL）

- ❌ `"query": {"match_all": {}}` + `"size": 1000`：一次拉太多，token 爆炸
- ❌ `"wildcard": {"msg": "*failed*"}`：性能极差，用 `match_phrase` 替代
- ❌ `"terms": {"field": "@message"}`：text 字段做不了 terms
- ❌ 不带 `sort` 拉 size>10 的原文：结果顺序不确定
- ❌ 一次查 >30 个接入名：MCP 大概率超时
