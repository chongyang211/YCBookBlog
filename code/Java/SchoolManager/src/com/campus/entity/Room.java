package com.campus.entity;

import com.campus.enums.RoomType;

import java.util.ArrayList;
import java.util.List;

/**
 * 房间（implements Comparable：容量降序自然排序）。
 */
public class Room implements Comparable<Room> {
    private final String id;
    private RoomType type;
    private int capacity;
    private final List<String> equipments;   // 投影 / 白板 / 计算机 ...

    public Room(String id, RoomType type, int capacity, List<String> equipments) {
        this.id = id;
        this.type = type;
        this.capacity = capacity;
        this.equipments = new ArrayList<>(equipments);  // 防御性拷贝
    }

    // ===== getter =====
    public String getId()    { return id; }
    public RoomType getType(){ return type; }
    public int getCapacity() { return capacity; }
    public List<String> getEquipments() {
        return new ArrayList<>(equipments);  // 防御性拷贝，外部改不到内部
    }

    public void addEquipment(String e) { equipments.add(e); }

    // ===== Comparable：容量降序，容量相同按 ID 升序 =====
    @Override
    public int compareTo(Room o) {
        int byCap = Integer.compare(o.capacity, this.capacity);  // 注意 o 在前才是降序
        if (byCap != 0) return byCap;
        return this.id.compareTo(o.id);
    }

    @Override
    public String toString() {
        return String.format("Room[id=%s, type=%s, cap=%d, eq=%s]",
                id, type.getDisplayName(), capacity, equipments);
    }
}
