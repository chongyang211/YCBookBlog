const map = new Map();
export const fileCache = {
    /** 单条注册 */
    put(id, file) {
        map.set(id, file);
    },
    /** 批量注册（扫描完成后调一次） */
    registerAll(items) {
        for (const it of items) {
            if (it.file)
                map.set(it.id, it.file);
        }
    },
    /** 取 File（没有就 null） */
    get(id) {
        return map.get(id) ?? null;
    },
    has(id) {
        return map.has(id);
    },
    /** 把 item.file 补上（如果 cache 有的话） */
    hydrate(item) {
        if (!item.file) {
            const f = map.get(item.id);
            if (f)
                item.file = f;
        }
        return item;
    },
    /** 批量 hydrate */
    hydrateAll(items) {
        for (const it of items)
            this.hydrate(it);
        return items;
    },
    /** 删除一条 */
    remove(id) {
        map.delete(id);
    },
    clear() {
        map.clear();
    },
    get size() {
        return map.size;
    },
};
