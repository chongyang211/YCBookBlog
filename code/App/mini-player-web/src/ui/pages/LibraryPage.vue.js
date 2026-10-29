import { ref, computed, onMounted, watch } from 'vue';
import { useRoute, useRouter } from 'vue-router';
import { repository } from '@/data/repository';
import { filesToMedia, walkDirectoryHandle } from '@/data/scanner';
import { fileCache } from '@/service/fileCache';
import { toast } from '@/service/toast';
import { usePlayerStore } from '@/service/playerService';
import { debounce } from '@/utils/debounce';
import MediaList from '@/ui/components/MediaList.vue';
import EmptyState from '@/ui/components/EmptyState.vue';
const player = usePlayerStore();
const route = useRoute();
const router = useRouter();
const TAB_LABELS = {
    all: '全部',
    audio: '音频',
    video: '视频',
    favorite: '我喜欢',
    history: '最近播放',
};
const tab = computed(() => route.params.tab ?? 'all');
const search = ref('');
const items = ref([]);
const scanning = ref(false);
const dropHover = ref(false);
const totalCount = ref(0);
const supportsDirPicker = typeof window.showDirectoryPicker === 'function';
function tabToType(t) {
    if (t === 'audio')
        return 'audio';
    if (t === 'video')
        return 'video';
    return undefined;
}
async function reload() {
    if (search.value.trim()) {
        items.value = fileCache.hydrateAll(await repository.searchMedia(search.value));
        return;
    }
    if (tab.value === 'favorite') {
        items.value = fileCache.hydrateAll(await repository.listFavorites());
    }
    else if (tab.value === 'history') {
        items.value = fileCache.hydrateAll(await repository.listHistory());
    }
    else {
        items.value = fileCache.hydrateAll(await repository.listMedia(tabToType(tab.value)));
    }
    totalCount.value = (await repository.listMedia()).length;
}
const reloadDebounced = debounce(reload, 220);
watch(search, () => reloadDebounced());
watch(tab, reload);
onMounted(reload);
/* ---------------- 导入 ---------------- */
const fileInputRef = ref(null);
function pickFiles() { fileInputRef.value?.click(); }
async function onFilesPicked(e) {
    const files = Array.from(e.target.files ?? []);
    if (!files.length)
        return;
    await importFiles(files, 'dropped');
    e.target.value = '';
}
async function importFiles(files, source) {
    scanning.value = true;
    const total = files.length;
    const toastId = toast.info(`正在解析 ${total} 个文件...`, 0);
    try {
        const list = await filesToMedia(files, source);
        if (list.length)
            await repository.addMediaBatch(list);
        toast.dismiss(toastId);
        const ignored = total - list.length;
        if (list.length) {
            toast.success(ignored > 0
                ? `已导入 ${list.length} 条，忽略 ${ignored} 个不支持文件`
                : `已导入 ${list.length} 条曲目`, 2600);
        }
        else {
            toast.warn('未找到支持的音视频文件');
        }
        await reload();
    }
    catch (e) {
        toast.dismiss(toastId);
        toast.error(`导入失败：${String(e)}`);
    }
    finally {
        scanning.value = false;
    }
}
async function pickDirectory() {
    if (!supportsDirPicker) {
        toast.warn('当前浏览器不支持目录选择，请使用「选择文件」或拖拽');
        return;
    }
    try {
        const dir = await window.showDirectoryPicker({ mode: 'read' });
        scanning.value = true;
        const toastId = toast.info('正在扫描目录...', 0);
        const files = [];
        for await (const f of walkDirectoryHandle(dir))
            files.push(f);
        toast.dismiss(toastId);
        await importFiles(files, 'directory');
    }
    catch (e) {
        if (e?.name !== 'AbortError') {
            console.error(e);
            toast.error('目录读取失败');
        }
        scanning.value = false;
    }
}
/* ---------------- 拖拽 ---------------- */
function onDragOver(e) {
    e.preventDefault();
    dropHover.value = true;
}
function onDragLeave(e) {
    // 只有真正离开根 dropzone 才清除
    if (e.currentTarget === e.target)
        dropHover.value = false;
}
async function onDrop(e) {
    e.preventDefault();
    dropHover.value = false;
    const dt = e.dataTransfer;
    if (!dt)
        return;
    const files = await collectDroppedFiles(dt);
    if (files.length)
        await importFiles(files, 'dropped');
    else
        toast.warn('拖入的文件未识别');
}
async function collectDroppedFiles(dt) {
    const out = [];
    const items = Array.from(dt.items);
    const dirEntries = [];
    for (const it of items) {
        const entry = it
            .webkitGetAsEntry?.();
        if (entry) {
            if (entry.isDirectory)
                dirEntries.push(entry);
            else if (entry.isFile) {
                const f = await filePromise(entry);
                if (f)
                    out.push(f);
            }
        }
        else if (it.kind === 'file') {
            const f = it.getAsFile();
            if (f)
                out.push(f);
        }
    }
    for (const dir of dirEntries) {
        await walkEntry(dir, out);
    }
    return out;
}
function filePromise(entry) {
    return new Promise(res => entry.file(f => res(f), () => res(null)));
}
async function walkEntry(entry, out) {
    const reader = entry.createReader();
    while (true) {
        const batch = await new Promise((res, rej) => reader.readEntries(res, rej));
        if (!batch.length)
            break;
        for (const e of batch) {
            if (e.isFile) {
                const f = await filePromise(e);
                if (f)
                    out.push(f);
            }
            else if (e.isDirectory) {
                await walkEntry(e, out);
            }
        }
    }
}
/* ---------------- 列表交互 ---------------- */
function switchTab(t) {
    router.push(`/library/${t}`);
}
function play(index) {
    // 用当前列表作为队列
    const target = items.value[index];
    if (!target)
        return;
    if (!target.file && !fileCache.has(target.id) && !target.externalUrl) {
        toast.warn(`"${target.title}" 文件已失效，请重新导入`);
        return;
    }
    player.loadQueue(items.value, index, true);
}
const emptyIsFullLibrary = computed(() => totalCount.value === 0 && !search.value.trim() && tab.value !== 'favorite' && tab.value !== 'history');
const emptyText = computed(() => {
    if (search.value)
        return `无匹配 "${search.value}" 的结果`;
    if (tab.value === 'favorite')
        return '尚未收藏任何曲目';
    if (tab.value === 'history')
        return '暂无播放历史';
    if (tab.value === 'audio')
        return '媒体库中还没有音频';
    if (tab.value === 'video')
        return '媒体库中还没有视频';
    return '媒体库为空';
});
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['title-area']} */ ;
/** @type {__VLS_StyleScopedClasses['search']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-primary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-primary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-primary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-secondary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-secondary']} */ ;
/** @type {__VLS_StyleScopedClasses['tabs']} */ ;
/** @type {__VLS_StyleScopedClasses['tabs']} */ ;
/** @type {__VLS_StyleScopedClasses['tabs']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['tabs']} */ ;
/** @type {__VLS_StyleScopedClasses['library']} */ ;
/** @type {__VLS_StyleScopedClasses['drop-tip']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-primary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-secondary']} */ ;
/** @type {__VLS_StyleScopedClasses['search']} */ ;
/** @type {__VLS_StyleScopedClasses['library']} */ ;
/** @type {__VLS_StyleScopedClasses['page-header']} */ ;
/** @type {__VLS_StyleScopedClasses['search']} */ ;
/** @type {__VLS_StyleScopedClasses['header-actions']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ onDragover: (__VLS_ctx.onDragOver) },
    ...{ onDragleave: (__VLS_ctx.onDragLeave) },
    ...{ onDrop: (__VLS_ctx.onDrop) },
    ...{ class: "library" },
    ...{ class: ({ 'drag-over': __VLS_ctx.dropHover }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.header, __VLS_intrinsicElements.header)({
    ...{ class: "page-header" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "title-area" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.h2, __VLS_intrinsicElements.h2)({});
(__VLS_ctx.TAB_LABELS[__VLS_ctx.tab]);
if (__VLS_ctx.items.length) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "count" },
    });
    (__VLS_ctx.items.length);
}
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "header-actions" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
    type: "search",
    ...{ class: "search" },
    placeholder: "搜索标题 / 艺术家 / 专辑...",
    'aria-label': "搜索",
});
(__VLS_ctx.search);
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.pickDirectory) },
    ...{ class: "btn-primary" },
    disabled: (__VLS_ctx.scanning),
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "btn-text" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.pickFiles) },
    ...{ class: "btn-secondary" },
    disabled: (__VLS_ctx.scanning),
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "btn-text" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
    ...{ onChange: (__VLS_ctx.onFilesPicked) },
    ref: "fileInputRef",
    type: "file",
    multiple: true,
    accept: "audio/*,video/*,.mp3,.flac,.m4a,.wav,.ogg,.mp4,.mkv,.mov,.webm",
    ...{ style: {} },
});
/** @type {typeof __VLS_ctx.fileInputRef} */ ;
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "tabs" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.switchTab('all');
        } },
    ...{ class: ({ active: __VLS_ctx.tab === 'all' }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.switchTab('audio');
        } },
    ...{ class: ({ active: __VLS_ctx.tab === 'audio' }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span)({
    ...{ class: "dot" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.switchTab('video');
        } },
    ...{ class: ({ active: __VLS_ctx.tab === 'video' }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span)({
    ...{ class: "dot" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span)({
    ...{ class: "tab-sep" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.switchTab('favorite');
        } },
    ...{ class: ({ active: __VLS_ctx.tab === 'favorite' }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.switchTab('history');
        } },
    ...{ class: ({ active: __VLS_ctx.tab === 'history' }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "list-area" },
});
if (__VLS_ctx.emptyIsFullLibrary) {
    /** @type {[typeof EmptyState, ]} */ ;
    // @ts-ignore
    const __VLS_0 = __VLS_asFunctionalComponent(EmptyState, new EmptyState({
        ...{ 'onImportDir': {} },
        ...{ 'onImportFile': {} },
    }));
    const __VLS_1 = __VLS_0({
        ...{ 'onImportDir': {} },
        ...{ 'onImportFile': {} },
    }, ...__VLS_functionalComponentArgsRest(__VLS_0));
    let __VLS_3;
    let __VLS_4;
    let __VLS_5;
    const __VLS_6 = {
        onImportDir: (__VLS_ctx.pickDirectory)
    };
    const __VLS_7 = {
        onImportFile: (__VLS_ctx.pickFiles)
    };
    var __VLS_2;
}
else {
    /** @type {[typeof MediaList, ]} */ ;
    // @ts-ignore
    const __VLS_8 = __VLS_asFunctionalComponent(MediaList, new MediaList({
        ...{ 'onPlay': {} },
        ...{ 'onReload': {} },
        items: (__VLS_ctx.items),
        emptyText: (__VLS_ctx.emptyText),
    }));
    const __VLS_9 = __VLS_8({
        ...{ 'onPlay': {} },
        ...{ 'onReload': {} },
        items: (__VLS_ctx.items),
        emptyText: (__VLS_ctx.emptyText),
    }, ...__VLS_functionalComponentArgsRest(__VLS_8));
    let __VLS_11;
    let __VLS_12;
    let __VLS_13;
    const __VLS_14 = {
        onPlay: (__VLS_ctx.play)
    };
    const __VLS_15 = {
        onReload: (__VLS_ctx.reload)
    };
    var __VLS_10;
}
const __VLS_16 = {}.Transition;
/** @type {[typeof __VLS_components.Transition, typeof __VLS_components.Transition, ]} */ ;
// @ts-ignore
const __VLS_17 = __VLS_asFunctionalComponent(__VLS_16, new __VLS_16({
    name: "drop-fade",
}));
const __VLS_18 = __VLS_17({
    name: "drop-fade",
}, ...__VLS_functionalComponentArgsRest(__VLS_17));
__VLS_19.slots.default;
if (__VLS_ctx.dropHover) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "drop-overlay" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "drop-tip glass" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "emoji" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({});
}
var __VLS_19;
/** @type {__VLS_StyleScopedClasses['library']} */ ;
/** @type {__VLS_StyleScopedClasses['page-header']} */ ;
/** @type {__VLS_StyleScopedClasses['title-area']} */ ;
/** @type {__VLS_StyleScopedClasses['count']} */ ;
/** @type {__VLS_StyleScopedClasses['header-actions']} */ ;
/** @type {__VLS_StyleScopedClasses['search']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-primary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-text']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-secondary']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-text']} */ ;
/** @type {__VLS_StyleScopedClasses['tabs']} */ ;
/** @type {__VLS_StyleScopedClasses['dot']} */ ;
/** @type {__VLS_StyleScopedClasses['dot']} */ ;
/** @type {__VLS_StyleScopedClasses['tab-sep']} */ ;
/** @type {__VLS_StyleScopedClasses['list-area']} */ ;
/** @type {__VLS_StyleScopedClasses['drop-overlay']} */ ;
/** @type {__VLS_StyleScopedClasses['drop-tip']} */ ;
/** @type {__VLS_StyleScopedClasses['glass']} */ ;
/** @type {__VLS_StyleScopedClasses['emoji']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            MediaList: MediaList,
            EmptyState: EmptyState,
            TAB_LABELS: TAB_LABELS,
            tab: tab,
            search: search,
            items: items,
            scanning: scanning,
            dropHover: dropHover,
            reload: reload,
            fileInputRef: fileInputRef,
            pickFiles: pickFiles,
            onFilesPicked: onFilesPicked,
            pickDirectory: pickDirectory,
            onDragOver: onDragOver,
            onDragLeave: onDragLeave,
            onDrop: onDrop,
            switchTab: switchTab,
            play: play,
            emptyIsFullLibrary: emptyIsFullLibrary,
            emptyText: emptyText,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
