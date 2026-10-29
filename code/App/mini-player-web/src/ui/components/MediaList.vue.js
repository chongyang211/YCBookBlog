import { computed } from 'vue';
import { formatTime } from '@/utils/format';
import { usePlayerStore } from '@/service/playerService';
import { repository } from '@/data/repository';
import { fileCache } from '@/service/fileCache';
import { toast } from '@/service/toast';
import CoverArt from './CoverArt.vue';
const props = defineProps();
const emit = defineEmits();
const player = usePlayerStore();
const currentId = computed(() => player.current?.id ?? '');
function onPlay(index) {
    emit('play', index);
}
async function onDelete(e, id, title) {
    e.stopPropagation();
    if (!confirm(`从媒体库移除 "${title}"？`))
        return;
    await repository.deleteMedia(id);
    fileCache.remove(id);
    toast.success('已从媒体库移除', 1600);
    emit('reload');
}
function fileAvailable(item) {
    return !!(item.file || fileCache.has(item.id) || item.externalUrl);
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['bars']} */ ;
/** @type {__VLS_StyleScopedClasses['bars']} */ ;
/** @type {__VLS_StyleScopedClasses['bars']} */ ;
/** @type {__VLS_StyleScopedClasses['bars']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['sub']} */ ;
/** @type {__VLS_StyleScopedClasses['play-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['more-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['more-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['play-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['more-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['play-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['type-badge']} */ ;
/** @type {__VLS_StyleScopedClasses['dur']} */ ;
/** @type {__VLS_StyleScopedClasses['row-actions']} */ ;
/** @type {__VLS_StyleScopedClasses['more-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play-btn']} */ ;
// CSS variable injection 
// CSS variable injection end 
if (props.items.length) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "list" },
    });
    for (const [it, i] of __VLS_getVForSourceType((props.items))) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ onDblclick: (...[$event]) => {
                    if (!(props.items.length))
                        return;
                    __VLS_ctx.onPlay(i);
                } },
            key: (it.id),
            ...{ class: "row" },
            ...{ class: ({ active: it.id === __VLS_ctx.currentId, 'is-missing': !__VLS_ctx.fileAvailable(it) }) },
        });
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "idx" },
        });
        if (it.id !== __VLS_ctx.currentId) {
            __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
            (i + 1);
        }
        else {
            __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
                ...{ class: "playing-icon" },
            });
            if (__VLS_ctx.player.isPlaying) {
                __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
                    ...{ class: "bars" },
                });
                __VLS_asFunctionalElement(__VLS_intrinsicElements.i)({});
                __VLS_asFunctionalElement(__VLS_intrinsicElements.i)({});
                __VLS_asFunctionalElement(__VLS_intrinsicElements.i)({});
            }
            else {
                __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
            }
        }
        /** @type {[typeof CoverArt, ]} */ ;
        // @ts-ignore
        const __VLS_0 = __VLS_asFunctionalComponent(CoverArt, new CoverArt({
            src: (it.coverUrl),
            id: (it.id),
            size: (44),
            placeholder: (it.type === 'video' ? '▸' : '♪'),
        }));
        const __VLS_1 = __VLS_0({
            src: (it.coverUrl),
            id: (it.id),
            size: (44),
            placeholder: (it.type === 'video' ? '▸' : '♪'),
        }, ...__VLS_functionalComponentArgsRest(__VLS_0));
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "info" },
        });
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "title ellipsis" },
            title: (it.title),
        });
        if (!__VLS_ctx.fileAvailable(it)) {
            __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
                ...{ class: "missing-badge" },
                title: "文件已失效",
            });
        }
        (it.title);
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "sub ellipsis" },
        });
        (it.artist);
        if (it.album) {
            __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
            (it.album);
        }
        if (it.type === 'video') {
            __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
                ...{ class: "type-badge" },
            });
        }
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "dur" },
        });
        (__VLS_ctx.formatTime(it.duration));
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "row-actions" },
        });
        __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
            ...{ onClick: (...[$event]) => {
                    if (!(props.items.length))
                        return;
                    __VLS_ctx.onPlay(i);
                } },
            ...{ class: "play-btn" },
            title: "播放",
        });
        __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
            ...{ onClick: (...[$event]) => {
                    if (!(props.items.length))
                        return;
                    __VLS_ctx.onDelete($event, it.id, it.title);
                } },
            ...{ class: "more-btn" },
            title: "删除",
        });
    }
}
else {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "empty-row" },
    });
    (props.emptyText ?? '媒体库为空');
}
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['idx']} */ ;
/** @type {__VLS_StyleScopedClasses['playing-icon']} */ ;
/** @type {__VLS_StyleScopedClasses['bars']} */ ;
/** @type {__VLS_StyleScopedClasses['info']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['ellipsis']} */ ;
/** @type {__VLS_StyleScopedClasses['missing-badge']} */ ;
/** @type {__VLS_StyleScopedClasses['sub']} */ ;
/** @type {__VLS_StyleScopedClasses['ellipsis']} */ ;
/** @type {__VLS_StyleScopedClasses['type-badge']} */ ;
/** @type {__VLS_StyleScopedClasses['dur']} */ ;
/** @type {__VLS_StyleScopedClasses['row-actions']} */ ;
/** @type {__VLS_StyleScopedClasses['play-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['more-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['empty-row']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            formatTime: formatTime,
            CoverArt: CoverArt,
            player: player,
            currentId: currentId,
            onPlay: onPlay,
            onDelete: onDelete,
            fileAvailable: fileAvailable,
        };
    },
    __typeEmits: {},
    __typeProps: {},
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
    __typeEmits: {},
    __typeProps: {},
});
; /* PartiallyEnd: #4569/main.vue */
