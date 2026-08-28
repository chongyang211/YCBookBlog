#!/usr/bin/env bash
#
# skills/mr-spec-review/scripts/prepare-src.sh
# ---------------------------------------------------------------------------
# 同步 $COSPEC_ROOT/src 下所有业务代码仓的「基线分支」到远端最新，供
# mr-spec-review skill 做只读代码验证（对应 SKILL.md 的 Step 0.2）。
#
# 归属说明：本脚本是 skill 的一部分（不是项目 CI 脚本）。CoSpec 的评审流水线
# 在外部事件驱动地调用 AI agent 触发 skill，skill 在 Step 0.2 运行本脚本准备
# 代码环境。因此脚本随 skill 走，保持 skill 自包含、可移植。
#
# 设计要点（并发安全）：
#   - 每个仓只「停在基线」并 reset --hard 对齐 origin —— 幂等，多条 MR 评审
#     并发跑、共用同一 src/ 目录时都收敛到同一 commit，互不踩踏。
#   - 绝不把共享仓切到某个 MR 专属分支（那会破坏其他并发会话的基线假设）。
#   - 缺失的仓自动 clone；已存在的 fetch。best-effort：单仓失败不中断整体。
#   - token 只经 http.extraHeader 命令级注入，不写进 .git/config，避免密钥落盘。
#
# 性能（并行同步）：
#   - 各仓目录互相独立、reset --hard 幂等，天然可并行 → 多个仓后台并发同步，
#     总耗时从「Σ(各仓)」降到「max(各仓)」。并发度由 PREPARE_SRC_JOBS 控制。
#   - 每个仓的日志先写到独占临时文件，最后按固定顺序聚合输出，避免并发交错。
#   - 可选按需过滤：设 REPOS_FILTER（空格/逗号分隔的目录名）只同步指定子集，
#     配合 skill 端识别只拉本次 MR 命中端的仓，进一步省时（不设=全量同步）。
#
# 环境变量（env-only）：
#   GONGFENG_TOKEN  可选。工蜂访问令牌，用于 HTTPS clone 缺失的私有仓（如无人值守
#                   CI 场景）。本地若仓库已存在且用 SSH remote，可不设（走 SSH key）。
#   CI_PROJECT_DIR / COSPEC_ROOT  可选。CoSpec 仓根目录；都没有则回退到 git 顶层。
#   PREPARE_SRC_JOBS  可选。并行同步的最大并发数，默认 8。
#   REPOS_FILTER      可选。只同步这些目录（空格或逗号分隔）；不设=全量。
# ---------------------------------------------------------------------------

set -uo pipefail

ROOT="${COSPEC_ROOT:-${CI_PROJECT_DIR:-$(git rev-parse --show-toplevel 2>/dev/null || pwd)}}"
SRC_DIR="${ROOT}/src"
mkdir -p "$SRC_DIR"

# 仅当提供了 token 时，构造命令级鉴权头（Basic base64("oauth2:<token>")）。
# 不持久化到 remote URL；对 SSH remote 无影响（HTTP-only header）。
GIT_AUTH=()
if [ -n "${GONGFENG_TOKEN:-}" ]; then
  AUTH_HEADER="Authorization: Basic $(printf 'oauth2:%s' "$GONGFENG_TOKEN" | base64 | tr -d '\n')"
  GIT_AUTH=(-c "http.extraHeader=${AUTH_HEADER}")
fi

# ---------------------------------------------------------------------------
# 本地目录名（相对 src/） => "基线分支|工蜂路径(不含 scheme，末尾 .git)"
# 基线分支与地址应与 docs/git-workflow.md 保持一致。
#
# 目录约定：
#   - mmpay_palm_recognition 的 palm_global / proto 逻辑已合并进 palm/weixin
#     的 palm_global / proto（src/palm_global、src/proto），原 palm_pipeline 分组
#     下的 palm_global / proto 已废弃删除。
#   - palm_algorithm 仍来自 mmpay_palm_recognition，直接平铺到 src/palm_algorithm。
# ---------------------------------------------------------------------------
declare -A REPOS=(
  # —— palm/palmpay ——
  ["palm_local"]="develop|git.woa.com/palm/palmpay/palm_local.git"
  ["palm_proto"]="master|git.woa.com/palm/palmpay/palm_proto.git"

  # —— palm/weixin ——
  ["palm_saas_fe"]="develop|git.woa.com/palm/weixin/palm_saas_fe.git"
  ["device_manage"]="develop|git.woa.com/palm/weixin/device_manage.git"
  ["palm_global"]="develop|git.woa.com/palm/weixin/palm_global.git"
  ["proto"]="master|git.woa.com/palm/weixin/proto.git"
  ["infrastructure"]="develop|git.woa.com/palm/weixin/infrastructure.git"

  # —— mmpay_palm_recognition（算法仓，平铺到 src/palm_algorithm）——
  ["palm_algorithm"]="master|git.woa.com/mmpay_palm_recognition/palm_algorithm.git"

  # —— wxPalmPaaS ——
  ["paymax_device"]="develop|git.woa.com/wxPalmPaaS/O4/paymax_device.git"
  ["palm-wepay"]="develop|git.woa.com/wxPalmPaaS/O1/palm-wepay.git"
  ["cloud_palm_activation"]="develop|git.woa.com/wxPalmPaaS/O1/cloud_palm_activation.git"
  ["cloud_IoTService"]="develop|git.woa.com/wxPalmPaaS/O1/cloud_IoTService.git"
  ["iotservice_linux"]="develop|git.woa.com/wxPalmPaaS/iotservice_linux.git"
  ["PaymaxPalmSdk"]="master|git.woa.com/wxPalmPaaS/PaymaxPalmSdk.git"

  # —— palm ——
  ["palm-register-demo"]="master|git.woa.com/palm/palm-register-demo.git"
)

sync_repo() {
  local dir="$1" baseline="$2" host_path="$3"
  local path="${SRC_DIR}/${dir}"

  if [ -d "${path}/.git" ]; then
    # 已存在：fetch（SSH remote 走本地 key；HTTPS remote 走 extraHeader）
    git "${GIT_AUTH[@]}" -C "$path" fetch --all --prune --quiet || return 1
  else
    # 缺失：HTTPS clone（需要 token）
    if [ ${#GIT_AUTH[@]} -eq 0 ]; then
      echo "   跳过 clone（未设 GONGFENG_TOKEN，无法拉取私有仓 ${host_path}）" >&2
      return 1
    fi
    mkdir -p "$(dirname "$path")"
    git "${GIT_AUTH[@]}" clone --quiet --depth 50 --branch "$baseline" \
      "https://${host_path}" "$path" || return 1
  fi
  # 停在基线并对齐远端最新（只读评审，不保留本地改动）
  git -C "$path" checkout --quiet "$baseline" || return 1
  git -C "$path" reset --hard --quiet "origin/${baseline}" || return 1
  return 0
}

# ---------------------------------------------------------------------------
# 计算本次要同步的目录清单：默认全量；若设了 REPOS_FILTER 则取交集（只同步子集）。
# ---------------------------------------------------------------------------
TARGET_DIRS=()
if [ -n "${REPOS_FILTER:-}" ]; then
  # 支持空格或逗号分隔
  IFS=', ' read -r -a _want <<< "${REPOS_FILTER}"
  for d in "${_want[@]}"; do
    [ -z "$d" ] && continue
    if [ -n "${REPOS[$d]:-}" ]; then
      TARGET_DIRS+=("$d")
    else
      echo "   忽略未知过滤项（不在 REPOS 映射中）: ${d}" >&2
    fi
  done
  echo "== REPOS_FILTER 生效：只同步 ${#TARGET_DIRS[@]} 个仓：${TARGET_DIRS[*]} =="
else
  TARGET_DIRS=("${!REPOS[@]}")
fi

# 每个仓的同步在子进程里执行，日志与状态各写到独占临时文件（避免并发交错）。
LOGDIR="$(mktemp -d "${TMPDIR:-/tmp}/prepare-src.XXXXXX")"
trap 'rm -rf "$LOGDIR"' EXIT

sync_one_bg() {
  local dir="$1" baseline="$2" host_path="$3"
  local slug="${dir//\//_}"
  local logf="${LOGDIR}/${slug}.log"
  local statf="${LOGDIR}/${slug}.status"
  {
    echo "== syncing ${dir}  (${baseline}  ${host_path}) =="
    if sync_repo "$dir" "$baseline" "$host_path"; then
      echo "ok" > "$statf"
    else
      echo "!! sync failed: ${dir}"
      echo "fail" > "$statf"
    fi
  } > "$logf" 2>&1
}

MAX_JOBS="${PREPARE_SRC_JOBS:-8}"
[ "$MAX_JOBS" -ge 1 ] 2>/dev/null || MAX_JOBS=8

# 受控并发：最多 MAX_JOBS 个后台任务在跑，跑满就 wait -n 回收一个再继续。
running=0
for dir in "${TARGET_DIRS[@]}"; do
  IFS='|' read -r baseline host_path <<< "${REPOS[$dir]}"
  sync_one_bg "$dir" "$baseline" "$host_path" &
  running=$((running + 1))
  if [ "$running" -ge "$MAX_JOBS" ]; then
    # wait -n 需 bash 4.3+；不可用则退回等全部当前批次。
    wait -n 2>/dev/null || wait
    running=$((running - 1))
  fi
done
wait  # 收尾：等剩余任务全部完成

# 按固定顺序聚合各仓日志 + 统计成败（顺序输出，杜绝并发交错）。
ok=()
failed=()
for dir in "${TARGET_DIRS[@]}"; do
  slug="${dir//\//_}"
  [ -f "${LOGDIR}/${slug}.log" ] && cat "${LOGDIR}/${slug}.log"
  if [ "$(cat "${LOGDIR}/${slug}.status" 2>/dev/null)" = "ok" ]; then
    ok+=("$dir")
  else
    failed+=("$dir")
  fi
done

echo
echo "== prepare-src 汇总：成功 ${#ok[@]} / 失败 ${#failed[@]}（共 ${#TARGET_DIRS[@]}，并发 ${MAX_JOBS}）=="
if [ "${#failed[@]}" -gt 0 ]; then
  printf '   失败仓库: %s\n' "${failed[*]}" >&2
  echo "   （评审仍会继续；这些仓库的代码验证可能不完整，agent 会在总评注明）" >&2
fi

# best-effort：即便部分仓失败也返回 0，让评审继续。
# 如需严格模式（任一仓失败即失败），可改为： [ "${#failed[@]}" -eq 0 ]
exit 0
