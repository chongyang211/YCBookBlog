import { ref, computed } from 'vue';
import { formatTime } from '@/utils/format';
const props = defineProps();
const emit = defineEmits();
const rootEl = ref(null);
const dragging = ref(false);
const hovering = ref(false);
const dragMs = ref(0);
const hoverRatio = ref(0); // 0-1
/** 拖动期间显示 draft，否则显示引擎的 position */
const displayMs = computed(() => (dragging.value ? dragMs.value : props.position));
const percent = computed(() => {
    if (!props.duration)
        return 0;
    return Math.min(100, Math.max(0, (displayMs.value / props.duration) * 100));
});
const hoverMs = computed(() => Math.floor(hoverRatio.value * props.duration));
/** 气泡防溢出：两端 clamp 到 6%-94%，避免半截气泡 */
const tipLeft = computed(() => `${Math.min(94, Math.max(6, hoverRatio.value * 100))}%`);
function ratioFromEvent(e) {
    const el = rootEl.value;
    if (!el)
        return 0;
    const rect = el.getBoundingClientRect();
    return Math.min(1, Math.max(0, (e.clientX - rect.left) / rect.width));
}
function onPointerDown(e) {
    if (props.disabled || props.duration <= 0)
        return;
    dragging.value = true;
    dragMs.value = ratioFromEvent(e) * props.duration;
    emit('seeking', true);
    e.currentTarget.setPointerCapture(e.pointerId);
}
function onPointerMove(e) {
    if (props.disabled)
        return;
    const r = ratioFromEvent(e);
    hoverRatio.value = r;
    if (dragging.value)
        dragMs.value = r * props.duration;
}
function onPointerUp(e) {
    if (!dragging.value)
        return;
    const ms = ratioFromEvent(e) * props.duration;
    dragging.value = false;
    emit('seek', ms);
    emit('seeking', false);
}
/** 键盘无障碍：role="slider" 的标准交互 */
function onKeydown(e) {
    if (props.disabled || props.duration <= 0)
        return;
    const step = props.duration * 0.05;
    switch (e.key) {
        case 'ArrowLeft':
            e.preventDefault();
            emit('seek', Math.max(0, props.position - step));
            break;
        case 'ArrowRight':
            e.preventDefault();
            emit('seek', Math.min(props.duration, props.position + step));
            break;
        case 'Home':
            e.preventDefault();
            emit('seek', 0);
            break;
        case 'End':
            e.preventDefault();
            emit('seek', props.duration);
            break;
    }
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['tall']} */ ;
/** @type {__VLS_StyleScopedClasses['rail']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['rail']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['rail']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['tall']} */ ;
/** @type {__VLS_StyleScopedClasses['rail']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['tall']} */ ;
/** @type {__VLS_StyleScopedClasses['dragging']} */ ;
/** @type {__VLS_StyleScopedClasses['rail']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['dragging']} */ ;
/** @type {__VLS_StyleScopedClasses['fill']} */ ;
/** @type {__VLS_StyleScopedClasses['fill']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['dragging']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['dragging']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['dragging']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ onPointerdown: (__VLS_ctx.onPointerDown) },
    ...{ onPointermove: (__VLS_ctx.onPointerMove) },
    ...{ onPointerup: (__VLS_ctx.onPointerUp) },
    ...{ onPointercancel: (__VLS_ctx.onPointerUp) },
    ...{ onMouseenter: (...[$event]) => {
            __VLS_ctx.hovering = true;
        } },
    ...{ onMouseleave: (...[$event]) => {
            __VLS_ctx.hovering = false;
        } },
    ...{ onKeydown: (__VLS_ctx.onKeydown) },
    ref: "rootEl",
    ...{ class: "pbar" },
    ...{ class: ({ disabled: __VLS_ctx.disabled, dragging: __VLS_ctx.dragging, hovering: __VLS_ctx.hovering, tall: __VLS_ctx.tall }) },
    role: "slider",
    'aria-label': "播放进度",
    'aria-valuemin': (0),
    'aria-valuemax': (Math.round(__VLS_ctx.duration)),
    'aria-valuenow': (Math.round(__VLS_ctx.displayMs)),
    'aria-disabled': (__VLS_ctx.disabled),
    tabindex: (__VLS_ctx.disabled ? -1 : 0),
});
/** @type {typeof __VLS_ctx.rootEl} */ ;
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "rail" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
    ...{ class: "fill" },
    ...{ style: ({ width: __VLS_ctx.percent + '%' }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div)({
    ...{ class: "thumb" },
    ...{ style: ({ left: __VLS_ctx.percent + '%' }) },
});
if (__VLS_ctx.hovering && !__VLS_ctx.dragging && __VLS_ctx.duration > 0) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        ...{ class: "tip" },
        ...{ style: ({ left: __VLS_ctx.tipLeft }) },
    });
    (__VLS_ctx.formatTime(__VLS_ctx.hoverMs));
}
/** @type {__VLS_StyleScopedClasses['pbar']} */ ;
/** @type {__VLS_StyleScopedClasses['rail']} */ ;
/** @type {__VLS_StyleScopedClasses['fill']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['tip']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            formatTime: formatTime,
            rootEl: rootEl,
            dragging: dragging,
            hovering: hovering,
            displayMs: displayMs,
            percent: percent,
            hoverMs: hoverMs,
            tipLeft: tipLeft,
            onPointerDown: onPointerDown,
            onPointerMove: onPointerMove,
            onPointerUp: onPointerUp,
            onKeydown: onKeydown,
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
