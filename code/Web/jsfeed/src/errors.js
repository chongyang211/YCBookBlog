/** 自定义 Error 基类——统一打印格式 */
export class FeedError extends Error {
  constructor(message, opts = {}) {
    super(message, opts);          // ⭐ ES2022：opts.cause 链式错误
    this.name = this.constructor.name;
    // ⭐ 让 stack trace 干净（V8 专有，其它引擎降级也无碍）
    if (Error.captureStackTrace) Error.captureStackTrace(this, this.constructor);
  }
}

export class NetworkError extends FeedError {
  constructor(url, cause) {
    super(`网络请求失败: ${url}`, { cause });
    this.url = url;
  }
}

export class TimeoutError extends FeedError {
  constructor(url, timeout) {
    super(`请求超时 ${timeout}ms: ${url}`);
    this.url = url;
    this.timeout = timeout;
  }
}

export class ParseError extends FeedError {
  constructor(url, cause) {
    super(`解析失败: ${url}`, { cause });
    this.url = url;
  }
}

/** 熔断错误：不可重试 */
export class CircuitBreakError extends FeedError {
  constructor(url) { super(`熔断器打开: ${url}`); this.url = url; }
}
