import { computed } from 'vue';
import { usePlayerStore } from '@/service/playerService';
import { formatTime } from '@/utils/format';
import CoverArt from './CoverArt.vue';
const props = defineProps();
const emit = defineEmits();
const player = usePlayerStore();
const currentId = computed(() => player.current?.id ?? '');
function play(id) {
    const idx = player.queue.findIndex(m => m.id === id);
    if (idx >= 0)
        player.loadQueue(player.queue, idx, true);
}
function remove(e, id) {
    e.stopPropagation();
    player.removeFromQueue(id);
}
function clearAll() {
    if (player.queue.length === 0)
        return;
    if (!confirm('清空当前播放队列？'))
        return;
    player.clearQueue();
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['link-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['link-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['close-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['sub']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['x-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['x-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['empty']} */ ;
/** @type {__VLS_StyleScopedClasses['empty']} */ ;
/** @type {__VLS_StyleScopedClasses['drawer']} */ ;
/** @type {__VLS_StyleScopedClasses['drawer']} */ ;
/** @type {__VLS_StyleScopedClasses['drawer-enter-from']} */ ;
/** @type {__VLS_StyleScopedClasses['drawer-leave-to']} */ ;
// CSS variable injection 
// CSS variable injection end 
const __VLS_0 = {}.Transition;
/** @type {[typeof __VLS_components.Transition, typeof __VLS_components.Transition, ]} */ ;
// @ts-ignore
const __VLS_1 = __VLS_asFunctionalComponent(__VLS_0, new __VLS_0({
    name: "drawer",
}));
const __VLS_2 = __VLS_1({
    name: "drawer",
}, ...__VLS_functionalComponentArgsRest(__VLS_1));
__VLS_3.slots.default;
if (props.open) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ onClick: (...[$event]) => {
                if (!(props.open))
                    return;
                __VLS_ctx.emit('close');
            } },
        ...{ class: "overlay" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.aside, __VLS_intrinsicElements.aside)({
        ...{ onClick: () => { } },
        ...{ class: "drawer glass" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.header, __VLS_intrinsicElements.header)({});
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "title" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.h3, __VLS_intrinsicElements.h3)({});
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "count" },
    });
    (__VLS_ctx.player.queue.length);
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "actions" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (__VLS_ctx.clearAll) },
        ...{ class: "link-btn" },
        disabled: (__VLS_ctx.player.queue.length === 0),
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (...[$event]) => {
                if (!(props.open))
                    return;
                __VLS_ctx.emit('close');
            } },
        ...{ class: "close-btn" },
        'aria-label': "关闭",
    });
    if (__VLS_ctx.player.queue.length) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "list" },
        });
        for (const [m] of __VLS_getVForSourceType((__VLS_ctx.player.queue))) {
            __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
                ...{ onClick: (...[$event]) => {
                        if (!(props.open))
                            return;
                        if (!(__VLS_ctx.player.queue.length))
                            return;
                        __VLS_ctx.play(m.id);
                    } },
                key: (m.id),
                ...{ class: "row" },
                ...{ class: ({ active: m.id === __VLS_ctx.currentId }) },
            });
            /** @type {[typeof CoverArt, ]} */ ;
            // @ts-ignore
            const __VLS_4 = __VLS_asFunctionalComponent(CoverArt, new CoverArt({
                src: (m.coverUrl),
                id: (m.id),
                size: (40),
            }));
            const __VLS_5 = __VLS_4({
                src: (m.coverUrl),
                id: (m.id),
                size: (40),
            }, ...__VLS_functionalComponentArgsRest(__VLS_4));
            __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
                ...{ class: "info" },
            });
            __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
                ...{ class: "title-txt ellipsis" },
            });
            (m.title);
            __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
                ...{ class: "sub ellipsis" },
            });
            (m.artist);
            __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
                ...{ class: "dur" },
            });
            (__VLS_ctx.formatTime(m.duration));
            __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
                ...{ onClick: (...[$event]) => {
                        if (!(props.open))
                            return;
                        if (!(__VLS_ctx.player.queue.length))
                            return;
                        __VLS_ctx.remove($event, m.id);
                    } },
                ...{ class: "x-btn" },
                'aria-label': "移除",
            });
        }
    }
    else {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "empty" },
        });
        __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
            ...{ class: "emoji" },
        });
        __VLS_asFunctionalElement(__VLS_intrinsicElements.p, __VLS_intrinsicElements.p)({});
        __VLS_asFunctionalElement(__VLS_intrinsicElements.p, __VLS_intrinsicElements.p)({
            ...{ class: "hint" },
        });
    }
}
var __VLS_3;
/** @type {__VLS_StyleScopedClasses['overlay']} */ ;
/** @type {__VLS_StyleScopedClasses['drawer']} */ ;
/** @type {__VLS_StyleScopedClasses['glass']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['count']} */ ;
/** @type {__VLS_StyleScopedClasses['actions']} */ ;
/** @type {__VLS_StyleScopedClasses['link-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['close-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['list']} */ ;
/** @type {__VLS_StyleScopedClasses['row']} */ ;
/** @type {__VLS_StyleScopedClasses['info']} */ ;
/** @type {__VLS_StyleScopedClasses['title-txt']} */ ;
/** @type {__VLS_StyleScopedClasses['ellipsis']} */ ;
/** @type {__VLS_StyleScopedClasses['sub']} */ ;
/** @type {__VLS_StyleScopedClasses['ellipsis']} */ ;
/** @type {__VLS_StyleScopedClasses['dur']} */ ;
/** @type {__VLS_StyleScopedClasses['x-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['empty']} */ ;
/** @type {__VLS_StyleScopedClasses['emoji']} */ ;
/** @type {__VLS_StyleScopedClasses['hint']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            formatTime: formatTime,
            CoverArt: CoverArt,
            emit: emit,
            player: player,
            currentId: currentId,
            play: play,
            remove: remove,
            clearAll: clearAll,
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
