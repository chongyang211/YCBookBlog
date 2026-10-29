import { ref, onMounted } from 'vue';
import { equalizer, EQ_FREQUENCIES, EQ_PRESETS, applyAndPersistEq } from '@/service/equalizer';
import { usePlayerStore } from '@/service/playerService';
import { repository } from '@/data/repository';
import { toast } from '@/service/toast';
const player = usePlayerStore();
const enabled = ref(false);
const preset = ref('flat');
const gains = ref(new Array(10).fill(0));
onMounted(async () => {
    const s = await repository.loadSettings();
    enabled.value = s.eqEnabled;
    preset.value = s.eqPreset;
    gains.value = [...s.eqGains];
});
async function toggleEq() {
    // 自动播放策略：必须在"用户手势"里 resume AudioContext
    try {
        if (player.audioEl) {
            await equalizer.setup(player.audioEl);
            await equalizer.ensureRunning();
        }
        enabled.value = !enabled.value;
        await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: preset.value });
        toast.success(enabled.value ? '均衡器已启用' : '均衡器已关闭', 1200);
    }
    catch (e) {
        console.error(e);
        toast.error('均衡器启用失败');
    }
}
async function pickPreset(name) {
    preset.value = name;
    gains.value = [...(EQ_PRESETS[name] ?? EQ_PRESETS.flat)];
    await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: name });
}
async function onBandChange(index, v) {
    gains.value[index] = v;
    preset.value = 'custom';
    await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: 'custom' });
}
async function reset() {
    gains.value = new Array(10).fill(0);
    preset.value = 'flat';
    await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: 'flat' });
}
function formatFreq(f) {
    return f >= 1000 ? `${f / 1000}k` : `${f}`;
}
debugger; /* PartiallyEnd: #3632/scriptSetup.vue */
const __VLS_ctx = {};
let __VLS_components;
let __VLS_directives;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['track']} */ ;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['on']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['presets']} */ ;
/** @type {__VLS_StyleScopedClasses['presets']} */ ;
/** @type {__VLS_StyleScopedClasses['presets']} */ ;
/** @type {__VLS_StyleScopedClasses['active']} */ ;
/** @type {__VLS_StyleScopedClasses['presets']} */ ;
/** @type {__VLS_StyleScopedClasses['presets']} */ ;
/** @type {__VLS_StyleScopedClasses['bands']} */ ;
/** @type {__VLS_StyleScopedClasses['gain']} */ ;
/** @type {__VLS_StyleScopedClasses['gain']} */ ;
// CSS variable injection 
// CSS variable injection end 
__VLS_asFunctionalElement(__VLS_intrinsicElements.section, __VLS_intrinsicElements.section)({
    ...{ class: "eq card" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.header, __VLS_intrinsicElements.header)({});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "title-row" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.h3, __VLS_intrinsicElements.h3)({});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "preset-tag" },
});
(__VLS_ctx.preset);
__VLS_asFunctionalElement(__VLS_intrinsicElements.label, __VLS_intrinsicElements.label)({
    ...{ class: "switch" },
    ...{ class: ({ on: __VLS_ctx.enabled }) },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
    ...{ onChange: (__VLS_ctx.toggleEq) },
    type: "checkbox",
    checked: (__VLS_ctx.enabled),
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "track" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span)({
    ...{ class: "thumb" },
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
    ...{ class: "label" },
});
(__VLS_ctx.enabled ? '已启用' : '已关闭');
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "presets" },
});
for (const [name] of __VLS_getVForSourceType((Object.keys(__VLS_ctx.EQ_PRESETS)))) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
        ...{ onClick: (...[$event]) => {
                __VLS_ctx.pickPreset(name);
            } },
        key: (name),
        ...{ class: ({ active: __VLS_ctx.preset === name }) },
    });
    (name);
}
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ class: ({ active: __VLS_ctx.preset === 'custom' }) },
    disabled: true,
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.button, __VLS_intrinsicElements.button)({
    ...{ onClick: (__VLS_ctx.reset) },
    ...{ class: "reset" },
    title: "重置为 flat",
});
__VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
    ...{ class: "bands" },
    ...{ class: ({ disabled: !__VLS_ctx.enabled }) },
});
for (const [f, i] of __VLS_getVForSourceType((__VLS_ctx.EQ_FREQUENCIES))) {
    __VLS_asFunctionalElement(__VLS_intrinsicElements.div, __VLS_intrinsicElements.div)({
        key: (f),
        ...{ class: "band" },
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "gain" },
        ...{ class: ({ positive: __VLS_ctx.gains[i] > 0, negative: __VLS_ctx.gains[i] < 0 }) },
    });
    (__VLS_ctx.gains[i] > 0 ? '+' : '');
    (__VLS_ctx.gains[i]);
    __VLS_asFunctionalElement(__VLS_intrinsicElements.input)({
        ...{ onInput: (...[$event]) => {
                __VLS_ctx.onBandChange(i, Number($event.target.value));
            } },
        ...{ class: "vslider" },
        type: "range",
        min: "-12",
        max: "12",
        step: "1",
        value: (__VLS_ctx.gains[i]),
        disabled: (!__VLS_ctx.enabled),
    });
    __VLS_asFunctionalElement(__VLS_intrinsicElements.span, __VLS_intrinsicElements.span)({
        ...{ class: "freq" },
    });
    (__VLS_ctx.formatFreq(f));
}
/** @type {__VLS_StyleScopedClasses['eq']} */ ;
/** @type {__VLS_StyleScopedClasses['card']} */ ;
/** @type {__VLS_StyleScopedClasses['title-row']} */ ;
/** @type {__VLS_StyleScopedClasses['preset-tag']} */ ;
/** @type {__VLS_StyleScopedClasses['switch']} */ ;
/** @type {__VLS_StyleScopedClasses['track']} */ ;
/** @type {__VLS_StyleScopedClasses['thumb']} */ ;
/** @type {__VLS_StyleScopedClasses['label']} */ ;
/** @type {__VLS_StyleScopedClasses['presets']} */ ;
/** @type {__VLS_StyleScopedClasses['reset']} */ ;
/** @type {__VLS_StyleScopedClasses['bands']} */ ;
/** @type {__VLS_StyleScopedClasses['band']} */ ;
/** @type {__VLS_StyleScopedClasses['gain']} */ ;
/** @type {__VLS_StyleScopedClasses['vslider']} */ ;
/** @type {__VLS_StyleScopedClasses['freq']} */ ;
var __VLS_dollars;
const __VLS_self = (await import('vue')).defineComponent({
    setup() {
        return {
            EQ_FREQUENCIES: EQ_FREQUENCIES,
            EQ_PRESETS: EQ_PRESETS,
            enabled: enabled,
            preset: preset,
            gains: gains,
            toggleEq: toggleEq,
            pickPreset: pickPreset,
            onBandChange: onBandChange,
            reset: reset,
            formatFreq: formatFreq,
        };
    },
});
export default (await import('vue')).defineComponent({
    setup() {
        return {};
    },
});
; /* PartiallyEnd: #4569/main.vue */
