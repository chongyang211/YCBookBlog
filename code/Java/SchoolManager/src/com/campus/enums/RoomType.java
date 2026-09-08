package com.campus.enums;

public enum RoomType {
    COMPUTER("机房"),
    MEETING("会议室"),
    LAB("实验室");

    private final String displayName;

    RoomType(String displayName) { this.displayName = displayName; }

    public String getDisplayName() { return displayName; }
}
