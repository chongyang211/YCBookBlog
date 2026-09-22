package flowkit;

import flowkit.facade.WorkflowFacade;

import java.util.Scanner;

/**
 * REPL 入口 —— 纯壳：所有逻辑都在 WorkflowFacade 后面。
 * 它同时是整个项目的终身回归测试器：五次会话的每次重构都靠它回归。
 */
public class App {
    private static final Scanner SC = new Scanner(System.in);

    public static void main(String[] args) {
        try {
            WorkflowFacade facade = new WorkflowFacade();
            System.out.println("FlowKit v1.0  (23 patterns loaded)");
            System.out.println("命令: submit <type> <amount(分)> \"<reason>\" | status | approve <人名>");
            System.out.println("      reject <人名> <理由> | addsign <人名> | withdraw | undo | attach <文件>");
            System.out.println("      expr <表达式> | notify | stats | history | replay | flow <key> | quit");
            if (facade.hasLogContent()) {
                System.out.println("(检测到历史命令日志，可输入 replay 恢复上次现场)");
            }
            loop(facade);
        } catch (Exception e) {
            System.out.println("[启动失败] " + e.getMessage());
        }
    }

    private static void loop(WorkflowFacade facade) {
        while (true) {
            System.out.print("flowkit> ");
            if (!SC.hasNextLine()) break;
            String line = SC.nextLine().trim();
            if (line.isEmpty()) continue;
            String[] t = line.split("\\s+", 2);
            String cmd = t[0];
            String rest = t.length > 1 ? t[1] : "";
            try {
                switch (cmd) {
                    case "submit"  -> facade.submit(parseType(rest), parseAmount(rest), parseReason(rest));
                    case "status"  -> facade.status();
                    case "approve" -> facade.approve(rest);
                    case "reject"  -> facade.reject(parseOperator(rest), parseReasonPart(rest));
                    case "addsign" -> facade.addsign(rest);
                    case "withdraw"-> facade.withdraw();
                    case "undo"    -> facade.undo();
                    case "attach"  -> facade.attach(rest);
                    case "expr"    -> facade.expr(rest);
                    case "notify"  -> facade.notifyAllChannels();
                    case "stats"   -> facade.stats();
                    case "history" -> facade.history();
                    case "replay"  -> facade.replay();
                    case "flow"    -> facade.flow(rest);
                    case "quit", "exit" -> { facade.shutdown(); return; }
                    case "help"    -> System.out.println("submit/status/approve/reject/addsign/withdraw/undo/attach/expr/notify/stats/history/replay/flow/quit");
                    default        -> System.out.println("未知命令: " + cmd + "（help 查看命令表）");
                }
            } catch (Exception e) {
                System.out.println("[错误] " + e.getMessage());
            }
        }
    }

    // submit refund 4800 "键盘进水"  →  [refund, 4800, "键盘进水"]
    private static String parseType(String rest) {
        return rest.split("\\s+", 3)[0];
    }
    private static long parseAmount(String rest) {
        return Long.parseLong(rest.split("\\s+", 3)[1]);
    }
    private static String parseReason(String rest) {
        String[] p = rest.split("\\s+", 3);
        return p.length > 2 ? p[2].replace("\"", "") : "";
    }

    // reject lisi 金额存疑  →  [lisi, 金额存疑]
    private static String parseOperator(String rest) {
        return rest.split("\\s+", 2)[0];
    }
    private static String parseReasonPart(String rest) {
        String[] p = rest.split("\\s+", 2);
        return p.length > 1 ? p[1] : "";
    }
}
