package com.campus.enums;

public enum ReservationStatus {
    PENDING("待审批"),
    APPROVED("已审批"),
    REJECTED("已驳回"),
    CANCELED("已取消");

    private final String displayName;

    ReservationStatus(String displayName) { this.displayName = displayName; }

    public String getDisplayName() { return displayName; }
}
