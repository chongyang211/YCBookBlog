export function parseVtt(text) {
  const lines = text.split(/\r?\n/);
  if (!lines[0].startsWith('WEBVTT')) throw new Error('非合法 WebVTT');

  const cues = [];
  let i = 1;
  while (i < lines.length) {
    const line = lines[i].trim();
    if (!line || line.startsWith('NOTE')) { i++; continue; }

    // 可能有 cue id（一整行非时间码），跳过
    const arrow = line.includes('-->') ? line : (i++, lines[i]?.trim() ?? '');
    if (!arrow.includes('-->')) { i++; continue; }

    const [a, b] = arrow.split('-->').map(s => parseTime(s.trim()));
    i++;
    const buf = [];
    while (i < lines.length && lines[i].trim()) {
      buf.push(lines[i]); i++;
    }
    cues.push({ start: a, end: b, text: buf.join('\n') });
  }
  return cues;
}

function parseTime(s) {
  // hh:mm:ss.ms 或 mm:ss.ms
  const parts = s.split(':');
  let h = 0, m = 0, sec = 0;
  if (parts.length === 3) [h, m, sec] = parts.map(Number);
  else                    [m, sec]    = parts.map(Number);
  return h * 3600 + m * 60 + sec;
}
