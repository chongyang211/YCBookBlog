import { onMounted, onBeforeUnmount, computed, ref } from 'vue';
import { RouterLink, RouterView, useRoute } from 'vue-router';
import { usePlayerStore } from '@/service/playerService';
import { bindMediaSession, unbindMediaSession } from '@/service/mediaSession';
import { useGlobalShortcuts } from '@/utils/useGlobalShortcuts';
import PlayerBar from '@/ui/components/PlayerBar.vue';
import ToastHost from '@/ui/components/ToastHost.vue';
import AutoplayBanner from '@/ui/components/AutoplayBanner.vue';
import MobileNav from '@/ui/components/MobileNav.vue';
const player = usePlayerStore();
const route = useRoute();
useGlobalShortcuts(player);
const navItems = [
    { to: '/library/all', label: '媒体库', icon: '♪' },
    { to: '/library/favorite', label: '我喜欢', icon: '♥' },
    { to: '/library/history', label: '最近播放', icon: '⟲' },
    { to: '/playing', label: '正在播放', icon: '▶' },
    { to: '/settings', label: '设置', icon: '⚙' },
];
const audioHidden = computed(() => player.current?.type !== 'video');
const isPlayingPage = computed(() => route.path.startsWith('/playing'));
const bgCoverStyle = computed(() => {
    const url = player.current?.coverUrl;
    if (!url)
        return {};
    return { backgroundImage: `url(${url})` };
});
const sidebarCollapsed = ref(false);
function onAudioRef(el) {
    if (el instanceof HTMLAudioElement)
        player.attachAudioElement(el);
}
function onVideoRef(el) {
    if (el instanceof HTMLVideoElement)
        player.attachVideoElement(el);
}
onMounted(async () => {
    await player.initialize();
    bindMediaSession(player);
});
onBeforeUnmount(() => {
    unbindMediaSession();
});
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['app-shell']} */ ;
/** @type {__VLS_StyleScopedClasses['bg-cover']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['collapsed']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['collapsed']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['collapsed']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['collapsed']} */ ;
/** @type {__VLS_StyleScopedClasses['collapse-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['router-link-active']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar-footer']} */ ;
/** @type {__VLS_StyleScopedClasses['video-canvas']} */ ;
/** @type {__VLS_StyleScopedClasses['video-canvas']} */ ;
/** @type {__VLS_StyleScopedClasses['app-shell']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['logo-text']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-label']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar-footer']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['collapse-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['logo-row']} */ ;
/** @type {__VLS_StyleScopedClasses['app-shell']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['video-canvas']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "app-shell" },
    ...{ class: ({ 'has-cover': !!__VLS_ctx.player.current?.coverUrl }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
    ...{ class: "bg-cover" },
    ...{ style: (__VLS_ctx.bgCoverStyle) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
    ...{ class: "bg-mask" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.aside, __VLS_intrinsicElements.aside)({
    ...{ class: "sidebar glass" },
    ...{ class: ({ collapsed: __VLS_ctx.sidebarCollapsed }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "logo-row" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "logo" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span)({
    ...{ class: "logo-dot" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "logo-text" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (...[$event]) => {
            __VLS_ctx.sidebarCollapsed = !__VLS_ctx.sidebarCollapsed;
        } },
    ...{ class: "collapse-btn" },
    title: (__VLS_ctx.sidebarCollapsed ? '展开' : '折叠'),
});
(__VLS_ctx.sidebarCollapsed ? '»' : '«');
__VLS_asFunctionalElement(__VLS_intrinsicElements.nav, __VLS_intrinsicElements.nav)({});
for (const [n] of __VLS_getVForSourceType((__VLS_ctx.navItems))) {
    const __VLS_0 = {}.RouterLink;
    /** @type {[typeof __VLS_components.RouterLink, typeof __VLS_components.RouterLink, ]} */ ;
    // @ts-ignore
    const __VLS_1 = __VLS_asFunctionalComponent(__VLS_0, new __VLS_0({
        key: (n.to),
        to: (n.to),
        ...{ class: "nav-item" },
        title: (n.label),
    }));
    const __VLS_2 = __VLS_1({
        key: (n.to),
        to: (n.to),
        ...{ class: "nav-item" },
        title: (n.label),
    }, ...__VLS_functionalComponentArgsRest(__VLS_1));
    __VLS_3.slots.default;
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "nav-icon" },
    });
    (n.icon);
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "nav-label" },
    });
    (n.label);
    var __VLS_3;
}
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "sidebar-footer" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "hint" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.main, __VLS_intrinsicElements.main)({
    ...{ class: "main" },
});
const __VLS_4 = {}.RouterView;
/** @type {[typeof __VLS_components.RouterView, typeof __VLS_components.RouterView, ]} */ ;
// @ts-ignore
const __VLS_5 = __VLS_asFunctionalComponent(__VLS_4, new __VLS_4({}));
const __VLS_6 = __VLS_5({}, ...__VLS_functionalComponentArgsRest(__VLS_5));
{
    const { default: __VLS_thisSlot } = __VLS_7.slots;
    const [{ Component }] = __VLS_getSlotParams(__VLS_thisSlot);
    const __VLS_8 = {}.Transition;
    /** @type {[typeof __VLS_components.Transition, typeof __VLS_components.Transition, ]} */ ;
    // @ts-ignore
    const __VLS_9 = __VLS_asFunctionalComponent(__VLS_8, new __VLS_8({
        name: "page",
        mode: "out-in",
    }));
    const __VLS_10 = __VLS_9({
        name: "page",
        mode: "out-in",
    }, ...__VLS_functionalComponentArgsRest(__VLS_9));
    __VLS_11.slots.default;
    const __VLS_12 = ((Component));
    // @ts-ignore
    const __VLS_13 = __VLS_asFunctionalComponent(__VLS_12, new __VLS_12({}));
    const __VLS_14 = __VLS_13({}, ...__VLS_functionalComponentArgsRest(__VLS_13));
    var __VLS_11;
    __VLS_7.slots['' /* empty slot name completion */];
}
var __VLS_7;
/** @type {[typeof PlayerBar, ]} */ ;
// @ts-ignore
const __VLS_16 = __VLS_asFunctionalComponent(PlayerBar, new PlayerBar({}));
const __VLS_17 = __VLS_16({}, ...__VLS_functionalComponentArgsRest(__VLS_16));
/** @type {[typeof MobileNav, ]} */ ;
// @ts-ignore
const __VLS_19 = __VLS_asFunctionalComponent(MobileNav, new MobileNav({}));
const __VLS_20 = __VLS_19({}, ...__VLS_functionalComponentArgsRest(__VLS_19));
__VLS_asFunctionalElement(__VLS_intrinsicElements.audio)({
    ref: (__VLS_ctx.onAudioRef),
    preload: "metadata",
    crossorigin: "anonymous",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.video)({
    ref: (__VLS_ctx.onVideoRef),
    ...{ class: "video-canvas" },
    ...{ class: ({ hidden: __VLS_ctx.audioHidden, 'in-playing': __VLS_ctx.isPlayingPage }) },
    preload: "metadata",
    playsinline: true,
    crossorigin: "anonymous",
});
/** @type {[typeof ToastHost, ]} */ ;
// @ts-ignore
const __VLS_22 = __VLS_asFunctionalComponent(ToastHost, new ToastHost({}));
const __VLS_23 = __VLS_22({}, ...__VLS_functionalComponentArgsRest(__VLS_22));
/** @type {[typeof AutoplayBanner, ]} */ ;
// @ts-ignore
const __VLS_25 = __VLS_asFunctionalComponent(AutoplayBanner, new AutoplayBanner({}));
const __VLS_26 = __VLS_25({}, ...__VLS_functionalComponentArgsRest(__VLS_25));
/** @type {__VLS_StyleScopedClasses['app-shell']} */ ;
/** @type {__VLS_StyleScopedClasses['bg-cover']} */ ;
/** @type {__VLS_StyleScopedClasses['bg-mask']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar']} */ ;
/** @type {__VLS_StyleScopedClasses['glass']} */ ;
/** @type {__VLS_StyleScopedClasses['logo-row']} */ ;
/** @type {__VLS_StyleScopedClasses['logo']} */ ;
/** @type {__VLS_StyleScopedClasses['logo-dot']} */ ;
/** @type {__VLS_StyleScopedClasses['logo-text']} */ ;
/** @type {__VLS_StyleScopedClasses['collapse-btn']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-icon']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-label']} */ ;
/** @type {__VLS_StyleScopedClasses['sidebar-footer']} */ ;
/** @type {__VLS_StyleScopedClasses['hint']} */ ;
/** @type {__VLS_StyleScopedClasses['main']} */ ;
/** @type {__VLS_StyleScopedClasses['video-canvas']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            RouterLink: RouterLink,
            RouterView: RouterView,
            PlayerBar: PlayerBar,
            ToastHost: ToastHost,
            AutoplayBanner: AutoplayBanner,
            MobileNav: MobileNav,
            player: player,
            navItems: navItems,
            audioHidden: audioHidden,
            isPlayingPage: isPlayingPage,
            bgCoverStyle: bgCoverStyle,
            sidebarCollapsed: sidebarCollapsed,
            onAudioRef: onAudioRef,
            onVideoRef: onVideoRef,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
