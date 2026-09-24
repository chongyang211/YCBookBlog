#include "mm/pipeline.hpp"
#include "mm/predictor.hpp"      // 阶段 6.3 才写,先前向声明
#include <variant>

namespace mm {

int PipelineSim::writeReg(const Inst& in) {
    return std::visit([](auto&& x) -> int {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, RForm> || std::is_same_v<T, IForm>) {
            // ADD/SUB/AND/OR/XOR/MOV 都写 rd
            if (x.op == Op::ADD || x.op == Op::SUB || x.op == Op::AND ||
                x.op == Op::OR  || x.op == Op::XOR || x.op == Op::MOV)
                return x.rd;
            return -1;
        } else if constexpr (std::is_same_v<T, MForm>) {
            return (x.op == Op::LOAD) ? x.rd : -1;
        } else {
            return -1;   // JForm/SForm 不写通用寄存器
        }
    }, in);
}

void PipelineSim::readRegs(const Inst& in, int r[2]) {
    r[0] = r[1] = -1;
    std::visit([&](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, RForm>) { r[0] = x.rs1; r[1] = x.rs2; }
        else if constexpr (std::is_same_v<T, IForm>) { r[0] = x.rs1; }
        else if constexpr (std::is_same_v<T, MForm>) { r[0] = x.base;
            if (x.op == Op::STORE) r[1] = x.rd;   // STORE 也要读 rd
        }
        else if constexpr (std::is_same_v<T, JForm>) {
            if (x.op == Op::JZ) r[0] = x.rs1;
        }
    }, in);
}

bool PipelineSim::isBranch(const Inst& in) {
    return std::visit([](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, JForm>) return x.op == Op::JZ;
        return false;
    }, in);
}
bool PipelineSim::isJump(const Inst& in) {
    return std::visit([](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, JForm>)
            return x.op == Op::JMP || x.op == Op::CALL;
        return false;
    }, in);
}

// 判断当前指令读的寄存器,是否和上一条(还在 EX 的指令)写的寄存器冲突
bool PipelineSim::hasRaw(const Inst& prev, const Inst& cur) {
    int prevRd = writeReg(prev);
    if (prevRd < 0) return false;
    int curR[2]; readRegs(cur, curR);
    return (curR[0] == prevRd || curR[1] == prevRd);
}

PipelineSim::Stats PipelineSim::simulate(const std::vector<InstMeta>& trace,
                                         bool useBranchPred) {
    Stats s;
    Predictor pred;    // 阶段 6.3 才定义,允许"无预测"退化模式

    // 基础拍数:填充流水线需要 4 拍(等第 1 条走完 IF/ID/EX/MEM),
    // 之后每条指令 1 拍,加冒险 bubble
    if (trace.empty()) return s;
    s.nCycles = 4;    // 填充拍数

    const InstMeta* prev = nullptr;
    for (auto& im : trace) {
        s.nCycles += 1;
        s.nInsts  += 1;

        // ---- 数据冒险:与前一条比较 rd vs rs ----
        if (prev && hasRaw(prev->inst, im.inst)) {
            s.nCycles += kRawBubble;
            s.nRaw    += 1;
        }
        // ---- 控制冒险:分支 & 预测挂 ----
        if (isBranch(im.inst)) {
            bool predicted = useBranchPred ? pred.predict(im.pc) : false;
            if (predicted != im.taken) {
                s.nCycles  += kControlBubble;
                s.nControl += 1;
            }
            pred.update(im.pc, im.taken);
        }
        // 无条件 JMP/CALL/RET: 硬件上目标地址其实 ID 阶段就能算出,
        // 简化教学:全部预测正确,不计入 bubble
        prev = &im;
    }
    return s;
}

}  // namespace mm
