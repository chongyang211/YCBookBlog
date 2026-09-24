package com.yc.orderforge.v3lspisp;

/** 运营/管理员角色接口：改价/调库存/导报表。 */
public interface AdminOperations {
    void modifyPrice();
    void adjustStock();
    void exportFinancialReport();
}
