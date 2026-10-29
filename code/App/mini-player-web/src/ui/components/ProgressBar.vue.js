import { ref, computed } from 'vue';
import { formatTime } from '@/utils/format';
const props = defineProps();
const emit = defineEmits();
const draftMs = ref(0);
const dragging = ref(false);
const hovering = ref(false);
const hoverRatio = ref(0); // 0-1
const displayMs = computed(() => (dragging.value ? draftMs.value : props.position));
const percent = computed(() => {
    if (!props.duration)
        return 0;
    return Math.min(100, Math.max(0, (displayMs.value / props.duration) * 100));
});
const hoverMs = computed(() => Math.floor(hoverRatio.value * props.duration));
function onInput(e) {
    const val = Number(e.target.value);
    draftMs.value = val;
    if (!dragging.value) {
        dragging.value = true;
        emit('seeking', true);
    }
}
function onChange(e) {
    const val = Number(e.target.value);
    dragging.value = false;
    emit('seek', val);
    emit('seeking', false);
}
function onMouseMove(e) {
    if (props.disabled)
        return;
    const rect = e.currentTarget.getBoundingClientRect();
    hoverRatio.value = Math.min(1, Math.max(0, (e.clientX - rect.left) / rect.width));
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['tall']} */ ;
/** @type {__VLS_StyleScopedClasses['track']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['track']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['track']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['tall']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['dragging']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ onMouseenter: (...[$event]) => {
            __VLS_ctx.hovering = !__VLS_ctx.disabled;
        } },
    ...{ onMouseleave: (...[$event]) => {
            __VLS_ctx.hovering = false;
        } },
    ...{ onMousemove: (__VLS_ctx.onMouseMove) },
    ...{ class: "progress" },
    ...{ class: ({ disabled: __VLS_ctx.disabled, dragging: __VLS_ctx.dragging, tall: __VLS_ctx.tall }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
    ...{ onInput: (__VLS_ctx.onInput) },
    ...{ onChange: (__VLS_ctx.onChange) },
    type: "range",
    min: (0),
    max: (__VLS_ctx.duration || 1),
    value: (__VLS_ctx.displayMs),
    disabled: (__VLS_ctx.disabled),
    'aria-label': "播放进度",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "track" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
    ...{ class: "fill" },
    ...{ style: ({ width: __VLS_ctx.percent + '%' }) },
});
if (__VLS_ctx.hovering && !__VLS_ctx.dragging && __VLS_ctx.duration > 0) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "hover-tip" },
        ...{ style: ({ left: (__VLS_ctx.hoverRatio * 100) + '%' }) },
    });
    (__VLS_ctx.formatTime(__VLS_ctx.hoverMs));
}
/** @type {__VLS_StyleScopedClasses['progress']} */ ;
/** @type {__VLS_StyleScopedClasses['track']} */ ;
/** @type {__VLS_StyleScopedClasses['fill']} */ ;
/** @type {__VLS_StyleScopedClasses['hover-tip']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            formatTime: formatTime,
            dragging: dragging,
            hovering: hovering,
            hoverRatio: hoverRatio,
            displayMs: displayMs,
            percent: percent,
            hoverMs: hoverMs,
            onInput: onInput,
            onChange: onChange,
            onMouseMove: onMouseMove,
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
