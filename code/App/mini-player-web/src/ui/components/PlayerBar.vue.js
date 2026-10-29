import { computed, ref } from 'vue';
import { useRouter } from 'vue-router';
import { usePlayerStore } from '@/service/playerService';
import { formatTime } from '@/utils/format';
import ProgressBar from './ProgressBar.vue';
import CoverArt from './CoverArt.vue';
import QueueDrawer from './QueueDrawer.vue';
const player = usePlayerStore();
const router = useRouter();
const queueOpen = ref(false);
const hasMedia = computed(() => !!player.current);
const isFav = computed(() => player.current ? !!player.favoriteOf[player.current.id] : false);
const loopIcon = computed(() => {
    switch (player.loopMode) {
        case 'single': return '🔂';
        case 'random': return '🔀';
        default: return '🔁';
    }
});
const loopLabel = computed(() => {
    switch (player.loopMode) {
        case 'single': return '单曲循环';
        case 'random': return '随机播放';
        default: return '顺序循环';
    }
});
function cycleLoop() {
    const order = ['order', 'single', 'random'];
    const idx = order.indexOf(player.loopMode);
    player.setLoopMode(order[(idx + 1) % order.length]);
}
function openPlaying() { router.push('/playing'); }
function toggleMuteClick(e) {
    e.stopPropagation();
    player.toggleMute();
}
function openQueue(e) {
    e.stopPropagation();
    queueOpen.value = true;
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['info']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['fav-mini']} */ ;
/** @type {__VLS_StyleScopedClasses['fav-mini']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play']} */ ;
/** @type {__VLS_StyleScopedClasses['play']} */ ;
/** @type {__VLS_StyleScopedClasses['play']} */ ;
/** @type {__VLS_StyleScopedClasses['volume']} */ ;
/** @type {__VLS_StyleScopedClasses['player-bar']} */ ;
/** @type {__VLS_StyleScopedClasses['volume']} */ ;
/** @type {__VLS_StyleScopedClasses['player-bar']} */ ;
/** @type {__VLS_StyleScopedClasses['info']} */ ;
/** @type {__VLS_StyleScopedClasses['info']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['artist']} */ ;
/** @type {__VLS_StyleScopedClasses['fav-mini']} */ ;
/** @type {__VLS_StyleScopedClasses['controls']} */ ;
/** @type {__VLS_StyleScopedClasses['buttons']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play']} */ ;
/** @type {__VLS_StyleScopedClasses['time']} */ ;
/** @type {__VLS_StyleScopedClasses['progress-wrap']} */ ;
/** @type {__VLS_StyleScopedClasses['volume']} */ ;
/** @type {__VLS_StyleScopedClasses['volume']} */ ;
/** @type {__VLS_StyleScopedClasses['mute']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.footer, __VLS_intrinsicElements.footer)({
    ...{ class: "player-bar glass" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ onClick: (__VLS_ctx.openPlaying) },
    ...{ class: "info" },
});
/** @type {[typeof CoverArt, ]} */ ;
// @ts-ignore
const __VLS_0 = __VLS_asFunctionalComponent(CoverArt, new CoverArt({
    src: (__VLS_ctx.player.current?.coverUrl),
    id: (__VLS_ctx.player.current?.id),
    size: (54),
}));
const __VLS_1 = __VLS_0({
    src: (__VLS_ctx.player.current?.coverUrl),
    id: (__VLS_ctx.player.current?.id),
    size: (54),
}, ...__VLS_functionalComponentArgsRest(__VLS_0));
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "meta" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "title ellipsis" },
});
(__VLS_ctx.player.current?.title ?? '未选择曲目');
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "artist ellipsis" },
});
(__VLS_ctx.player.current?.artist ?? '从媒体库选一首歌开始播放');
if (__VLS_ctx.hasMedia) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (__VLS_ctx.player.toggleFavorite) },
        ...{ class: "fav-mini" },
        ...{ class: ({ fav: __VLS_ctx.isFav }) },
        title: (__VLS_ctx.isFav ? '取消收藏' : '加入收藏'),
    });
    if (__VLS_ctx.isFav) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
    }
    else {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
    }
}
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "controls" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "buttons" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.cycleLoop) },
    ...{ class: "icon-btn" },
    disabled: (!__VLS_ctx.hasMedia),
    title: (__VLS_ctx.loopLabel),
});
(__VLS_ctx.loopIcon);
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.player.previous) },
    ...{ class: "icon-btn" },
    disabled: (!__VLS_ctx.hasMedia),
    title: "上一首",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.player.togglePlay) },
    ...{ class: "play" },
    disabled: (!__VLS_ctx.hasMedia),
    title: (__VLS_ctx.player.isPlaying ? '暂停' : '播放'),
});
if (__VLS_ctx.player.status === 'loading') {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span)({
        ...{ class: "spinner" },
    });
}
else if (__VLS_ctx.player.isPlaying) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
}
else {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
}
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.player.next(false);
        } },
    ...{ class: "icon-btn" },
    disabled: (!__VLS_ctx.hasMedia),
    title: "下一首",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.openQueue) },
    ...{ class: "icon-btn" },
    disabled: (!__VLS_ctx.hasMedia),
    title: "播放队列",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "progress-wrap" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "time" },
});
(__VLS_ctx.formatTime(__VLS_ctx.player.position));
/** @type {[typeof ProgressBar, ]} */ ;
// @ts-ignore
const __VLS_3 = __VLS_asFunctionalComponent(ProgressBar, new ProgressBar({
    ...{ 'onSeeking': {} },
    ...{ 'onSeek': {} },
    position: (__VLS_ctx.player.position),
    duration: (__VLS_ctx.player.duration),
    disabled: (!__VLS_ctx.hasMedia),
}));
const __VLS_4 = __VLS_3({
    ...{ 'onSeeking': {} },
    ...{ 'onSeek': {} },
    position: (__VLS_ctx.player.position),
    duration: (__VLS_ctx.player.duration),
    disabled: (!__VLS_ctx.hasMedia),
}, ...__VLS_functionalComponentArgsRest(__VLS_3));
let __VLS_6;
let __VLS_7;
let __VLS_8;
const __VLS_9 = {
    onSeeking: (...[$event]) => {
        __VLS_ctx.player.seeking = $event;
    }
};
const __VLS_10 = {
    onSeek: (__VLS_ctx.player.seek)
};
var __VLS_5;
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "time" },
});
(__VLS_ctx.formatTime(__VLS_ctx.player.duration));
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "volume" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.toggleMuteClick) },
    ...{ class: "icon-btn mute" },
    title: (__VLS_ctx.player.muted ? '取消静音' : '静音'),
});
(__VLS_ctx.player.muted ? '🔇' : __VLS_ctx.player.volume > 0.5 ? '🔊' : __VLS_ctx.player.volume > 0 ? '🔉' : '🔈');
__VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
    ...{ onInput: (...[$event]) => {
            __VLS_ctx.player.setVolume(Number($event.target.value));
        } },
    type: "range",
    min: "0",
    max: "1",
    step: "0.01",
    value: (__VLS_ctx.player.muted ? 0 : __VLS_ctx.player.volume),
    'aria-label': "音量",
});
/** @type {[typeof QueueDrawer, ]} */ ;
// @ts-ignore
const __VLS_11 = __VLS_asFunctionalComponent(QueueDrawer, new QueueDrawer({
    ...{ 'onClose': {} },
    open: (__VLS_ctx.queueOpen),
}));
const __VLS_12 = __VLS_11({
    ...{ 'onClose': {} },
    open: (__VLS_ctx.queueOpen),
}, ...__VLS_functionalComponentArgsRest(__VLS_11));
let __VLS_14;
let __VLS_15;
let __VLS_16;
const __VLS_17 = {
    onClose: (...[$event]) => {
        __VLS_ctx.queueOpen = false;
    }
};
var __VLS_13;
/** @type {__VLS_StyleScopedClasses['player-bar']} */ ;
/** @type {__VLS_StyleScopedClasses['glass']} */ ;
/** @type {__VLS_StyleScopedClasses['info']} */ ;
/** @type {__VLS_StyleScopedClasses['meta']} */ ;
/** @type {__VLS_StyleScopedClasses['title']} */ ;
/** @type {__VLS_StyleScopedClasses['ellipsis']} */ ;
/** @type {__VLS_StyleScopedClasses['artist']} */ ;
/** @type {__VLS_StyleScopedClasses['ellipsis']} */ ;
/** @type {__VLS_StyleScopedClasses['fav-mini']} */ ;
/** @type {__VLS_StyleScopedClasses['controls']} */ ;
/** @type {__VLS_StyleScopedClasses['buttons']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play']} */ ;
/** @type {__VLS_StyleScopedClasses['spinner']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['progress-wrap']} */ ;
/** @type {__VLS_StyleScopedClasses['time']} */ ;
/** @type {__VLS_StyleScopedClasses['time']} */ ;
/** @type {__VLS_StyleScopedClasses['volume']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['mute']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            formatTime: formatTime,
            ProgressBar: ProgressBar,
            CoverArt: CoverArt,
            QueueDrawer: QueueDrawer,
            player: player,
            queueOpen: queueOpen,
            hasMedia: hasMedia,
            isFav: isFav,
            loopIcon: loopIcon,
            loopLabel: loopLabel,
            cycleLoop: cycleLoop,
            openPlaying: openPlaying,
            toggleMuteClick: toggleMuteClick,
            openQueue: openQueue,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
