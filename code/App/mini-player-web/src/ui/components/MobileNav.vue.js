import { RouterLink, useRoute } from 'vue-router';
import { computed } from 'vue';
const route = useRoute();
const items = [
    { to: '/library/all', label: '媒体库', icon: '♪' },
    { to: '/playing', label: '播放', icon: '▶' },
    { to: '/settings', label: '设置', icon: '⚙' },
];
const activeTo = computed(() => route.path);
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.nav, __VLS_intrinsicElements.nav)({
    ...{ class: "mobile-nav glass" },
});
for (const [n] of __VLS_getVForSourceType((__VLS_ctx.items))) {
    const __VLS_0 = {}.RouterLink;
    /** @type {[typeof __VLS_components.RouterLink, typeof __VLS_components.RouterLink, ]} */ ;
    // @ts-ignore
    const __VLS_1 = __VLS_asFunctionalComponent(__VLS_0, new __VLS_0({
        key: (n.to),
        to: (n.to),
        ...{ class: "nav-item" },
        ...{ class: ({ active: __VLS_ctx.activeTo.startsWith(n.to.split('/').slice(0, 2).join('/')) }) },
    }));
    const __VLS_2 = __VLS_1({
        key: (n.to),
        to: (n.to),
        ...{ class: "nav-item" },
        ...{ class: ({ active: __VLS_ctx.activeTo.startsWith(n.to.split('/').slice(0, 2).join('/')) }) },
    }, ...__VLS_functionalComponentArgsRest(__VLS_1));
    __VLS_3.slots.default;
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "icon" },
    });
    (n.icon);
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "label" },
    });
    (n.label);
    var __VLS_3;
}
/** @type {__VLS_StyleScopedClasses['mobile-nav']} */ ;
/** @type {__VLS_StyleScopedClasses['glass']} */ ;
/** @type {__VLS_StyleScopedClasses['nav-item']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['label']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            RouterLink: RouterLink,
            items: items,
            activeTo: activeTo,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
