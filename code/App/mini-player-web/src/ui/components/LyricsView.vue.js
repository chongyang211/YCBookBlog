import { computed, ref, watch } from 'vue';
import { usePlayerStore } from '@/service/playerService';
import { parseLrc, findLineIndex } from '@/service/lyricsParser';
import { toast } from '@/service/toast';
const player = usePlayerStore();
const lrcText = ref('');
const doc = ref({ lines: [], meta: {}, offset: 0 });
watch(lrcText, (txt) => {
    doc.value = parseLrc(txt);
});
// 曲目变化时清空上一首的歌词
watch(() => player.current?.id, () => {
    lrcText.value = '';
    doc.value = { lines: [], meta: {}, offset: 0 };
});
const activeIdx = computed(() => findLineIndex(doc.value.lines, player.position - doc.value.offset));
async function onLoadLrc(e) {
    const input = e.target;
    const file = input.files?.[0];
    if (!file)
        return;
    try {
        lrcText.value = await file.text();
        toast.success(`已加载歌词 (${doc.value.lines.length} 行)`, 1600);
    }
    catch {
        toast.error('歌词加载失败');
    }
    input.value = '';
}
function jumpTo(idx) {
    const line = doc.value.lines[idx];
    if (line)
        player.seek(line.time);
}
const translateY = computed(() => {
    // 当前行向中间居中，一行约 44px
    const ROW_H = 44;
    return `translateY(${-(activeIdx.value * ROW_H)}px)`;
});
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['load']} */ ;
/** @type {__VLS_StyleScopedClasses['load']} */ ;
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['placeholder']} */ ;
/** @type {__VLS_StyleScopedClasses['placeholder']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "lyrics" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "toolbar" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.label, __VLS_intrinsicElements.label)({
    ...{ class: "load" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
    ...{ onChange: (__VLS_ctx.onLoadLrc) },
    type: "file",
    accept: ".lrc,text/plain",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
if (__VLS_ctx.doc.lines.length) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "count" },
    });
    (__VLS_ctx.doc.lines.length);
}
if (__VLS_ctx.doc.lines.length) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "viewport" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.ul, __VLS_intrinsicElements.ul)({
        ...{ class: "list" },
        ...{ style: ({ transform: __VLS_ctx.translateY }) },
    });
    for (const [line, i] of __VLS_getVForSourceType((__VLS_ctx.doc.lines))) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.li, __VLS_intrinsicElements.li)({
            ...{ onClick: (...[$event]) => {
                    if (!(__VLS_ctx.doc.lines.length))
                        return;
                    __VLS_ctx.jumpTo(i);
                } },
            key: (i),
            ...{ class: ({ active: i === __VLS_ctx.activeIdx, past: i < __VLS_ctx.activeIdx }) },
        });
        (line.text || '♪');
    }
}
else {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "placeholder" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "icon" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.p, __VLS_intrinsicElements.p)({});
    __VLS_asFunctionalElement(__VLS_intrinsicElements.p, __VLS_intrinsicElements.p)({
        ...{ class: "hint" },
    });
}
/** @type {__VLS_StyleScopedClasses['lyrics']} */ ;
/** @type {__VLS_StyleScopedClasses['toolbar']} */ ;
/** @type {__VLS_StyleScopedClasses['load']} */ ;
/** @type {__VLS_StyleScopedClasses['count']} */ ;
/** @type {__VLS_StyleScopedClasses['viewport']} */ ;
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['placeholder']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['hint']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            doc: doc,
            activeIdx: activeIdx,
            onLoadLrc: onLoadLrc,
            jumpTo: jumpTo,
            translateY: translateY,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
