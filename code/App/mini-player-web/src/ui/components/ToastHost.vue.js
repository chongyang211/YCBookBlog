import { toastState, toast } from '@/service/toast';
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['success']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['error']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['warn']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
// CSS variable injection 
// CSS variable injection end 
const __VLS_0 = {}.TransitionGroup;
/** @type {[typeof __VLS_components.TransitionGroup, typeof __VLS_components.TransitionGroup, ]} */ ;
// @ts-ignore
const __VLS_1 = __VLS_asFunctionalComponent(__VLS_0, new __VLS_0({
    name: "toast",
    tag: "div",
    ...{ class: "toast-host" },
}));
const __VLS_2 = __VLS_1({
    name: "toast",
    tag: "div",
    ...{ class: "toast-host" },
}, ...__VLS_functionalComponentArgsRest(__VLS_1));
var __VLS_4 = {};
__VLS_3.slots.default;
for (const [t] of __VLS_getVForSourceType((__VLS_ctx.toastState.list))) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ onClick: (...[$event]) => {
                __VLS_ctx.toast.dismiss(t.id);
            } },
        key: (t.id),
        ...{ class: "toast" },
        ...{ class: (t.kind) },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "icon" },
    });
    if (t.kind === 'success') {
    }
    else if (t.kind === 'error') {
    }
    else if (t.kind === 'warn') {
    }
    else {
    }
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "msg" },
    });
    (t.message);
}
var __VLS_3;
/** @type {__VLS_StyleScopedClasses['toast-host']} */ ;
/** @type {__VLS_StyleScopedClasses['toast']} */ ;
/** @type {__VLS_StyleScopedClasses['icon']} */ ;
/** @type {__VLS_StyleScopedClasses['msg']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            toastState: toastState,
            toast: toast,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
