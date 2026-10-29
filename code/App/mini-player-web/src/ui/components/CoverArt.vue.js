import { computed } from 'vue';
import { gradientFromId } from '@/utils/colorFromId';
const props = defineProps();
const bgStyle = computed(() => {
    if (props.src)
        return { backgroundImage: `url(${props.src})`, backgroundSize: 'cover', backgroundPosition: 'center' };
    return { background: gradientFromId(props.id ?? 'default') };
});
const sizeStyle = computed(() => {
    const s = props.size ?? 54;
    return { width: `${s}px`, height: `${s}px`, fontSize: `${s * 0.4}px` };
});
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['cover-art']} */ ;
/** @type {__VLS_StyleScopedClasses['cover-art']} */ ;
/** @type {__VLS_StyleScopedClasses['cover-art']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "cover-art" },
    ...{ class: ({ circle: __VLS_ctx.circle, spinning: __VLS_ctx.spinning && !!__VLS_ctx.src, 'is-placeholder': !__VLS_ctx.src }) },
    ...{ style: ({ ...__VLS_ctx.bgStyle, ...__VLS_ctx.sizeStyle }) },
});
if (!__VLS_ctx.src) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "ph" },
    });
    (__VLS_ctx.placeholder ?? '♪');
}
/** @type {__VLS_StyleScopedClasses['cover-art']} */ ;
/** @type {__VLS_StyleScopedClasses['ph']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            bgStyle: bgStyle,
            sizeStyle: sizeStyle,
        };
    },
    __typeProps: {},
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
    __typeProps: {},
});
; /* PartiallyEnd: #4569/main.vue */
