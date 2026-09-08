package com.campus.entity;

import java.time.LocalDateTime;

public class Speech {
    private final String id;
    private String title;
    private final String speakerId;
    private String department;        // 演讲者所在系别（用于按系别分组）
    private double score;             // 评分（用于 Top-K 排行榜）
    private final LocalDateTime createdAt;

    public Speech(String id, String title, String speakerId,
                  String department, double score) {
        this.id = id;
        this.title = title;
        this.speakerId = speakerId;
        this.department = department;
        this.score = score;
        this.createdAt = LocalDateTime.now();
    }

    public String getId()        { return id; }
    public String getTitle()     { return title; }
    public String getSpeakerId() { return speakerId; }
    public String getDepartment(){ return department; }
    public double getScore()     { return score; }
    public LocalDateTime getCreatedAt() { return createdAt; }

    public void setScore(double score) { this.score = score; }

    @Override
    public String toString() {
        return String.format("Speech[%s | %s | speaker=%s | dept=%s | score=%.1f]",
                id, title, speakerId, department, score);
    }
}
