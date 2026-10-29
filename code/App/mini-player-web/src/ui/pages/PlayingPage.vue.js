import { computed } from 'vue';
import { usePlayerStore } from '@/service/playerService';
import { formatTime } from '@/utils/format';
import LyricsView from '@/ui/components/LyricsView.vue';
import ProgressBar from '@/ui/components/ProgressBar.vue';
import CoverArt from '@/ui/components/CoverArt.vue';
import { gradientFromId } from '@/utils/colorFromId';
const player = usePlayerStore();
const bgStyle = computed(() => {
    const url = player.current?.coverUrl;
    if (url)
        return { backgroundImage: `url(${url})` };
    const id = player.current?.id ?? 'default';
    return { background: gradientFromId(id) };
});
const isFav = computed(() => player.current ? !!player.favoriteOf[player.current.id] : false);
function toggleFullscreen() {
    const el = player.videoEl;
    if (!el)
        return;
    if (document.fullscreenElement)
        document.exitFullscreen();
    else
        el.requestFullscreen().catch(() => { });
}
const loopIcon = computed(() => {
    switch (player.loopMode) {
        case 'single': return '🔂';
        case 'random': return '🔀';
        default: return '🔁';
    }
});
function cycleLoop() {
    const order = ['order', 'single', 'random'];
    const idx = order.indexOf(player.loopMode);
    player.setLoopMode(order[(idx + 1) % order.length]);
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play-main']} */ ;
/** @type {__VLS_StyleScopedClasses['play-main']} */ ;
/** @type {__VLS_StyleScopedClasses['fs-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['empty']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-link']} */ ;
/** @type {__VLS_StyleScopedClasses['content']} */ ;
/** @type {__VLS_StyleScopedClasses['big-cover']} */ ;
/** @type {__VLS_StyleScopedClasses['content']} */ ;
/** @type {__VLS_StyleScopedClasses['big-cover']} */ ;
/** @type {__VLS_StyleScopedClasses['track-title']} */ ;
/** @type {__VLS_StyleScopedClasses['play-main']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['lyrics-area']} */ ;
/** @type {__VLS_StyleScopedClasses['content']} */ ;
/** @type {__VLS_StyleScopedClasses['big-cover']} */ ;
/** @type {__VLS_StyleScopedClasses['controls']} */ ;
// CSS variable injection 
// CSS variable injection end 
if (__VLS_ctx.player.current) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.section, __VLS_intrinsicElements.section)({
        ...{ class: "playing" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
        ...{ class: "bg" },
        ...{ style: (__VLS_ctx.bgStyle) },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
        ...{ class: "mask" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "content" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "cover-area" },
    });
    /** @type {[typeof CoverArt, ]} */ ;
    // @ts-ignore
    const __VLS_0 = __VLS_asFunctionalComponent(CoverArt, new CoverArt({
        ...{ class: "big-cover" },
        src: (__VLS_ctx.player.current.coverUrl),
        id: (__VLS_ctx.player.current.id),
        size: (300),
        spinning: (__VLS_ctx.player.isPlaying && __VLS_ctx.player.current.type === 'audio'),
        circle: (__VLS_ctx.player.current.type === 'audio'),
    }));
    const __VLS_1 = __VLS_0({
        ...{ class: "big-cover" },
        src: (__VLS_ctx.player.current.coverUrl),
        id: (__VLS_ctx.player.current.id),
        size: (300),
        spinning: (__VLS_ctx.player.isPlaying && __VLS_ctx.player.current.type === 'audio'),
        circle: (__VLS_ctx.player.current.type === 'audio'),
    }, ...__VLS_functionalComponentArgsRest(__VLS_0));
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "meta" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.h2, __VLS_intrinsicElements.h2)({
        ...{ class: "track-title" },
        title: (__VLS_ctx.player.current.title),
    });
    (__VLS_ctx.player.current.title);
    __VLS_asFunctionalElement(__VLS_intrinsicElements.p, __VLS_intrinsicElements.p)({
        ...{ class: "track-sub" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
    (__VLS_ctx.player.current.artist);
    if (__VLS_ctx.player.current.album) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
            ...{ class: "dot-sep" },
        });
    }
    if (__VLS_ctx.player.current.album) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
        (__VLS_ctx.player.current.album);
    }
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "progress-row" },
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
        tall: true,
        position: (__VLS_ctx.player.position),
        duration: (__VLS_ctx.player.duration),
    }));
    const __VLS_4 = __VLS_3({
        ...{ 'onSeeking': {} },
        ...{ 'onSeek': {} },
        tall: true,
        position: (__VLS_ctx.player.position),
        duration: (__VLS_ctx.player.duration),
    }, ...__VLS_functionalComponentArgsRest(__VLS_3));
    let __VLS_6;
    let __VLS_7;
    let __VLS_8;
    const __VLS_9 = {
        onSeeking: (...[$event]) => {
            if (!(__VLS_ctx.player.current))
                return;
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
        ...{ class: "controls" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (__VLS_ctx.cycleLoop) },
        ...{ class: "icon-btn" },
        title: (__VLS_ctx.player.loopMode),
    });
    (__VLS_ctx.loopIcon);
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (__VLS_ctx.player.previous) },
        ...{ class: "icon-btn" },
        title: "上一首",
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (__VLS_ctx.player.togglePlay) },
        ...{ class: "play-main" },
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
                if (!(__VLS_ctx.player.current))
                    return;
                __VLS_ctx.player.next(false);
            } },
        ...{ class: "icon-btn" },
        title: "下一首",
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (__VLS_ctx.player.toggleFavorite) },
        ...{ class: "icon-btn" },
        ...{ class: ({ fav: __VLS_ctx.isFav }) },
        title: (__VLS_ctx.isFav ? '取消收藏' : '加入收藏'),
    });
    if (__VLS_ctx.isFav) {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
    }
    else {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
    }
    if (__VLS_ctx.player.current.type === 'video') {
        __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
            ...{ onClick: (__VLS_ctx.toggleFullscreen) },
            ...{ class: "fs-btn" },
        });
    }
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "lyrics-area" },
    });
    /** @type {[typeof LyricsView, ]} */ ;
    // @ts-ignore
    const __VLS_11 = __VLS_asFunctionalComponent(LyricsView, new LyricsView({}));
    const __VLS_12 = __VLS_11({}, ...__VLS_functionalComponentArgsRest(__VLS_11));
}
else {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.section, __VLS_intrinsicElements.section)({
        ...{ class: "empty" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "empty-icon" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.h3, __VLS_intrinsicElements.h3)({});
    __VLS_asFunctionalElement(__VLS_intrinsicElements.p, __VLS_intrinsicElements.p)({});
    const __VLS_14 = {}.RouterLink;
    /** @type {[typeof __VLS_components.RouterLink, typeof __VLS_components.RouterLink, ]} */ ;
    // @ts-ignore
    const __VLS_15 = __VLS_asFunctionalComponent(__VLS_14, new __VLS_14({
        to: "/library/all",
        ...{ class: "btn-link" },
    }));
    const __VLS_16 = __VLS_15({
        to: "/library/all",
        ...{ class: "btn-link" },
    }, ...__VLS_functionalComponentArgsRest(__VLS_15));
    __VLS_17.slots.default;
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({});
    var __VLS_17;
}
/** @type {__VLS_StyleScopedClasses['playing']} */ ;
/** @type {__VLS_StyleScopedClasses['bg']} */ ;
/** @type {__VLS_StyleScopedClasses['mask']} */ ;
/** @type {__VLS_StyleScopedClasses['content']} */ ;
/** @type {__VLS_StyleScopedClasses['cover-area']} */ ;
/** @type {__VLS_StyleScopedClasses['big-cover']} */ ;
/** @type {__VLS_StyleScopedClasses['meta']} */ ;
/** @type {__VLS_StyleScopedClasses['track-title']} */ ;
/** @type {__VLS_StyleScopedClasses['track-sub']} */ ;
/** @type {__VLS_StyleScopedClasses['dot-sep']} */ ;
/** @type {__VLS_StyleScopedClasses['progress-row']} */ ;
/** @type {__VLS_StyleScopedClasses['time']} */ ;
/** @type {__VLS_StyleScopedClasses['time']} */ ;
/** @type {__VLS_StyleScopedClasses['controls']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['play-main']} */ ;
/** @type {__VLS_StyleScopedClasses['spinner']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['icon-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['fs-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['lyrics-area']} */ ;
/** @type {__VLS_StyleScopedClasses['empty']} */ ;
/** @type {__VLS_StyleScopedClasses['empty-icon']} */ ;
/** @type {__VLS_StyleScopedClasses['btn-link']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            formatTime: formatTime,
            LyricsView: LyricsView,
            ProgressBar: ProgressBar,
            CoverArt: CoverArt,
            player: player,
            bgStyle: bgStyle,
            isFav: isFav,
            toggleFullscreen: toggleFullscreen,
            loopIcon: loopIcon,
            cycleLoop: cycleLoop,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
