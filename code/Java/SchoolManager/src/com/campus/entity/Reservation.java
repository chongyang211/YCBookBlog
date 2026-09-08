package com.campus.entity;

import com.campus.enums.ReservationStatus;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class Reservation {
    private final String id;
    private final String userId;
    private final String roomId;
    private final LocalDateTime startTime;
    private final LocalDateTime endTime;
    private ReservationStatus status;     // 唯一可变字段（审批/取消会改）

    public Reservation(String id, String userId, String roomId,
                       LocalDateTime startTime, LocalDateTime endTime) {
        if (!endTime.isAfter(startTime)) {
            throw new IllegalArgumentException("结束时间必须晚于开始时间");
        }
        this.id = id;
        this.userId = userId;
        this.roomId = roomId;
        this.startTime = startTime;
        this.endTime = endTime;
        this.status = ReservationStatus.PENDING;
    }

    public String getId()       { return id; }
    public String getUserId()   { return userId; }
    public String getRoomId()   { return roomId; }
    public LocalDateTime getStartTime() { return startTime; }
    public LocalDateTime getEndTime()   { return endTime; }
    public ReservationStatus getStatus(){ return status; }

    public void approve() { this.status = ReservationStatus.APPROVED; }
    public void reject()  { this.status = ReservationStatus.REJECTED; }
    public void cancel()  { this.status = ReservationStatus.CANCELED; }

    private static final DateTimeFormatter FMT =
            DateTimeFormatter.ofPattern("MM-dd HH:mm");

    @Override
    public String toString() {
        return String.format("Reservation[%s | user=%s | room=%s | %s~%s | %s]",
                id, userId, roomId,
                startTime.format(FMT), endTime.format(FMT),
                status.getDisplayName());
    }
}
